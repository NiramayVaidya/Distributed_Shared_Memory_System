#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <ucontext.h>
#include <grpcpp/grpcpp.h>
#include <malloc.h>

#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <map>
#include <tuple>
#include <mutex>
#include <chrono>
#include <iterator>

#include "psu_dsm.h"
#include "dsm.grpc.pb.h"

using namespace std;
using namespace grpc;
using namespace dsm;

fstream logFile;

int port = 50000;
string dirHost;
string nodeListFilename = "node_list.txt";
string gRPCLogFilename = "gRPC_log.txt";

uint64_t pageNum = 0;
vector<vector<tuple<string, uint64_t>>> dsmData;
vector<int> ownerVec;
map<string, map<string, vector<uint64_t>>> allocateDsmData;
map<string, string> ownerMap;

mutex dsmDataLock;
mutex ownerVecLock;
mutex allocateDsmDataLock;
mutex ownerMapLock;
mutex getLatestLock;
mutex fetchLatestLock;
mutex dirUpdLock;
mutex logLock;

bool initialized = false;

#if DEBUG
static void printDsmData() {
	cout << "dsmData ->" << endl;
	for (int i = 0; i < dsmData.size(); i++) {
		for (int j = 0; j < dsmData[i].size(); j++) {
			cout << to_string(i) + ", " + to_string(j) + ", host = " + get<0>(dsmData[i][j]) + ", pageAddr = " + to_string(get<1>(dsmData[i][j])) << endl;
		}
	}
}

static void printAllocDsmData() {
	cout << "allocDsmData ->" << endl;
	map<string, map<string, vector<uint64_t>>>::iterator it;
	for (it = allocateDsmData.begin(); it != allocateDsmData.end(); it++) {
		cout << "name = " + it->first << endl;
		map<string, vector<uint64_t>>::iterator itnew;
		map<string, vector<uint64_t>> hostPointers = it->second;
		for (itnew = hostPointers.begin(); itnew != hostPointers.end(); itnew++) {
			cout << "host = " + itnew->first + ", pointers = ";
			vector<uint64_t> pointers = itnew->second;
			for (int i = 0; i < pointers.size(); i++) {
				cout << to_string(pointers[i]) + " ";
			}
			cout << endl;
		}
	}
}

static void printOwnerVec() {
	cout << "ownerVec ->" << endl;
	for (int i = 0; i < ownerVec.size() - 1; i++) {
		cout << to_string(ownerVec[i]) + "\t";
	}
	cout << to_string(ownerVec[ownerVec.size() - 1]) << endl;
}

static void printInvalidateVec(vector<tuple<string, uint64_t>> invalidateVec) {
	cout << "invalidateVec ->" << endl;
	for (int i = 0; i < invalidateVec.size(); i++) {
		cout << "host = " + get<0>(invalidateVec[i]) + ", pageAddr = " + to_string(get<1>(invalidateVec[i])) << endl;
	}
}

static void printOwnerMap() {
	cout << "ownerMap ->" << endl;
	map<string, string>::iterator it;
	for (it = ownerMap.begin(); it != ownerMap.end(); it++) {
		cout << "name = " + it->first + ", host = " + it->second << endl;
	}
}
#endif

static string GetHostName() {
	char host[256];
	gethostname(host, sizeof(host));
	string hostName(host);
	return hostName;
}

class PollClient {
	public:
		PollClient(shared_ptr<Channel> channel) : stub_(Poll::NewStub(channel)) {}
		
		bool poll() {
			PollRequest pollRequest;

			PollReply pollReply;

			ClientContext context;

			Status status = stub_->poll(&context, pollRequest, &pollReply);

			while(!status.ok()) {
				ClientContext context;
				status = stub_->poll(&context, pollRequest, &pollReply);
			}

			if (status.ok()) {
				return pollReply.active();
			}
			else {
				cout << status.error_code() << ": " << status.error_message() << endl;
				return "Poll RPC Failed";
			}
		}

	 private:
		std::unique_ptr<Poll::Stub> stub_;
};

class RegDataSegClient {
	public:
		RegDataSegClient(shared_ptr<Channel> channel) : stub_(RegDataSeg::NewStub(channel)) {}
		
		void regDataSeg(string hostName, uint64_t pageAddr, uint32_t size) {
			RegRequest regRequest;
			regRequest.set_host(hostName);
			regRequest.set_pagenum(pageNum);
			regRequest.set_pageaddr(pageAddr);
			regRequest.set_size(size);

			RegReply regReply;

			ClientContext context;

			Status status = stub_->regDataSeg(&context, regRequest, &regReply);

			pageNum = regReply.pagenum();
		}

	 private:
		std::unique_ptr<RegDataSeg::Stub> stub_;
};

class DirUpdClient {
	public:
		DirUpdClient(shared_ptr<Channel> channel) : stub_(DirUpd::NewStub(channel)) {}
		
		void dirUpd(string hostName, uint64_t pageAddr) {
			UpdRequest updRequest;
			updRequest.set_host(hostName);
			updRequest.set_pageaddr(pageAddr);

			UpdReply updReply;

			ClientContext context;

			Status status = stub_->dirUpd(&context, updRequest, &updReply);
		}

	 private:
		std::unique_ptr<DirUpd::Stub> stub_;
};

class DirUpdAllocClient {
	public:
		DirUpdAllocClient(shared_ptr<Channel> channel) : stub_(DirUpdAlloc::NewStub(channel)) {}
		
		void dirUpdAlloc(string hostName, uint64_t pointer) {
			UpdAllocRequest updAllocRequest;
			updAllocRequest.set_host(hostName);
			updAllocRequest.set_pointer(pointer);

			UpdAllocReply updAllocReply;

			ClientContext context;

			Status status = stub_->dirUpdAlloc(&context, updAllocRequest, &updAllocReply);
		}

	 private:
		std::unique_ptr<DirUpdAlloc::Stub> stub_;
};

class InvalidateClient {
	public:
		InvalidateClient(shared_ptr<Channel> channel) : stub_(Invalidate::NewStub(channel)) {}
		
		void invalidate(uint64_t pageAddr) {
			InvalidateRequest invalidateRequest;
			invalidateRequest.set_pageaddr(pageAddr);

			InvalidateReply invalidateReply;

			ClientContext context;
			chrono::time_point<chrono::system_clock> deadline = chrono::system_clock::now() + chrono::milliseconds(10);
			// HACK
			context.set_deadline(deadline);

			Status status = stub_->invalidate(&context, invalidateRequest, &invalidateReply);
		}

	 private:
		std::unique_ptr<Invalidate::Stub> stub_;
};

class InvalidateAllocClient {
	public:
		InvalidateAllocClient(shared_ptr<Channel> channel) : stub_(InvalidateAlloc::NewStub(channel)) {}
		
		void invalidateAlloc(uint64_t pointer) {
			InvalidateAllocRequest invalidateAllocRequest;
			invalidateAllocRequest.set_pointer(pointer);

			InvalidateAllocReply invalidateAllocReply;

			ClientContext context;
			chrono::time_point<chrono::system_clock> deadline = chrono::system_clock::now() + chrono::milliseconds(10);
			// HACK
			context.set_deadline(deadline);

			Status status = stub_->invalidateAlloc(&context, invalidateAllocRequest, &invalidateAllocReply);
		}

	 private:
		std::unique_ptr<InvalidateAlloc::Stub> stub_;
};

class GetLatestClient {
	public:
		GetLatestClient(shared_ptr<Channel> channel) : stub_(GetLatest::NewStub(channel)) {}
		
		void getLatest(string hostName, uint64_t pageAddr) {
			LatestRequest latestRequest;
			latestRequest.set_host(hostName);
			latestRequest.set_pageaddr(pageAddr);

			LatestReply latestReply;

			ClientContext context;

			Status status = stub_->getLatest(&context, latestRequest, &latestReply);

			uint64_t *startAddr = (uint64_t *) pageAddr;
#if DEBUG
			cout << "0th old value in page at requester = " + to_string((uint64_t) startAddr[0]) << endl;
#endif
			for (int i = 0; i < latestReply.pagecontent_size(); i++) {
				startAddr[i] = latestReply.pagecontent(i);
			}
#if DEBUG
			cout << "0th updated value in page at requester = " + to_string((uint64_t) startAddr[0]) << endl;
#endif
		}

	 private:
		std::unique_ptr<GetLatest::Stub> stub_;
};

class GetLatestAllocClient {
	public:
		GetLatestAllocClient(shared_ptr<Channel> channel) : stub_(GetLatestAlloc::NewStub(channel)) {}
		
		void getLatestAlloc(string hostName, uint64_t pointer) {
			LatestAllocRequest latestAllocRequest;
			latestAllocRequest.set_host(hostName);
			latestAllocRequest.set_pointer(pointer);

			LatestAllocReply latestAllocReply;

			ClientContext context;

			Status status = stub_->getLatestAlloc(&context, latestAllocRequest, &latestAllocReply);

			uint64_t *initialPointer = (uint64_t *) pointer;
#if DEBUG
			cout << "0th old value at pointer at requester = " + to_string((uint64_t) initialPointer[0]) << endl;
#endif
			for (int i = 0; i < latestAllocReply.pagecontent_size(); i++) {
				initialPointer[i] = latestAllocReply.pagecontent(i);
			}
#if DEBUG
			cout << "0th updated value at pointer at requester = " + to_string((uint64_t) initialPointer[0]) << endl;
#endif
		}

	 private:
		std::unique_ptr<GetLatestAlloc::Stub> stub_;
};

class FetchLatestClient {
	public:
		FetchLatestClient(shared_ptr<Channel> channel) : stub_(FetchLatest::NewStub(channel)) {}
		
		void fetchLatest(uint64_t pageAddr, LatestReply *latestReply) {
			FetchRequest fetchRequest;
			fetchRequest.set_pageaddr(pageAddr);

			FetchReply fetchReply;

			ClientContext context;

			Status status = stub_->fetchLatest(&context, fetchRequest, &fetchReply);

			for (int i = 0; i < fetchReply.pagecontent_size(); i++) {
				latestReply->add_pagecontent(fetchReply.pagecontent(i));
			}
		}

	 private:
		std::unique_ptr<FetchLatest::Stub> stub_;
};

class FetchLatestAllocClient {
	public:
		FetchLatestAllocClient(shared_ptr<Channel> channel) : stub_(FetchLatestAlloc::NewStub(channel)) {}
		
		void fetchLatestAlloc(uint64_t pointer, LatestAllocReply *latestAllocReply) {
			FetchAllocRequest fetchAllocRequest;
			fetchAllocRequest.set_pointer(pointer);

			FetchAllocReply fetchAllocReply;

			ClientContext context;

			Status status = stub_->fetchLatestAlloc(&context, fetchAllocRequest, &fetchAllocReply);

			for (int i = 0; i < fetchAllocReply.pagecontent_size(); i++) {
				latestAllocReply->add_pagecontent(fetchAllocReply.pagecontent(i));
			}
		}

	 private:
		std::unique_ptr<FetchLatestAlloc::Stub> stub_;
};

class CheckAllocateClient {
	public:
		CheckAllocateClient(shared_ptr<Channel> channel) : stub_(CheckAllocate::NewStub(channel)) {}
		
		bool checkAllocate(string hostName, string name) {
			CheckRequest checkRequest;
			checkRequest.set_host(hostName);
			checkRequest.set_name(name);

			CheckReply checkReply;

			ClientContext context;

			Status status = stub_->checkAllocate(&context, checkRequest, &checkReply);

			return checkReply.present();
		}

	 private:
		std::unique_ptr<CheckAllocate::Stub> stub_;
};

class CheckAllocatePointerClient {
	public:
		CheckAllocatePointerClient(shared_ptr<Channel> channel) : stub_(CheckAllocatePointer::NewStub(channel)) {}
		
		bool checkAllocatePointer(string hostName, uint64_t pointer) {
			CheckPointerRequest checkPointerRequest;
			checkPointerRequest.set_host(hostName);
			checkPointerRequest.set_pointer(pointer);

			CheckPointerReply checkPointerReply;

			ClientContext context;

			Status status = stub_->checkAllocatePointer(&context, checkPointerRequest, &checkPointerReply);

			return checkPointerReply.present();
		}

	 private:
		std::unique_ptr<CheckAllocatePointer::Stub> stub_;
};

class GetPointerClient {
	public:
		GetPointerClient(shared_ptr<Channel> channel) : stub_(GetPointer::NewStub(channel)) {}
		
		uint64_t getPointer(string hostName, string name) {
			PointerRequest pointerRequest;
			pointerRequest.set_host(hostName);
			pointerRequest.set_name(name);

			PointerReply pointerReply;

			ClientContext context;

			Status status = stub_->getPointer(&context, pointerRequest, &pointerReply);

			return (uint64_t) pointerReply.pointer();
		}

	 private:
		std::unique_ptr<GetPointer::Stub> stub_;
};

class AllocateClient {
	public:
		AllocateClient(shared_ptr<Channel> channel) : stub_(Allocate::NewStub(channel)) {}
		
		void allocate(string hostName, string name, uint64_t pointer, uint32_t size) {
			AllocRequest allocRequest;
			allocRequest.set_host(hostName);
			allocRequest.set_name(name);
			allocRequest.set_pointer(pointer);
			allocRequest.set_size(size);

			AllocReply allocReply;

			ClientContext context;

			Status status = stub_->allocate(&context, allocRequest, &allocReply);
		}

	 private:
		std::unique_ptr<Allocate::Stub> stub_;
};

class PollServiceImpl final : public Poll::Service {
	Status poll(ServerContext* context, const PollRequest* request, PollReply* reply) override {
#if DEBUG
		cout << "In poll" << endl;
#endif
		reply->set_active(true);
		return Status::OK;
	}
};

class RegDataSegServiceImpl final : public RegDataSeg::Service {
	Status regDataSeg(ServerContext* context, const RegRequest* request, RegReply* reply) override {
#if DEBUG
		cout << "In regDataSeg" << endl;
		cout << "Call from host = " + request->host() << endl;
#endif
		int numPages = request->size() / PAGE_SIZE;
		uint32_t iter = request->pagenum();
#if DEBUG
		cout << "startAddr = " + to_string((uint64_t) request->pageaddr()) << endl;
		cout << "size = " + to_string((uint32_t) request->size()) << endl;
		cout << "Obtained pageNum = " + to_string(iter) << endl;
		cout << "numPages = " + to_string(numPages) << endl;
#endif
		int i = 0;
		while (iter < request->pagenum() + numPages) {
			tuple<string, uint64_t> hostPageAddr;
			hostPageAddr = make_tuple(request->host(), request->pageaddr() + (i * PAGE_SIZE));
			dsmDataLock.lock();
			if (dsmData.size() > iter) {
				dsmData[iter].push_back(hostPageAddr);
			}
			else {
				vector<tuple<string, uint64_t>> pageNumLevel;
				pageNumLevel.push_back(hostPageAddr);
				dsmData.push_back(pageNumLevel);
				ownerVecLock.lock();
				ownerVec.push_back(-1);
				ownerVecLock.unlock();
			}
			dsmDataLock.unlock();
			iter++;
			i++;
		}
		reply->set_pagenum(iter);
#if DEBUG
		dsmDataLock.lock();
		printDsmData();
		dsmDataLock.unlock();
		ownerVecLock.lock();
		printOwnerVec();
		ownerVecLock.unlock();
		cout << "Updated pageNum = " + to_string(iter) << endl;
#endif
		return Status::OK;
	}
};

class DirUpdServiceImpl final : public DirUpd::Service {
	Status dirUpd(ServerContext* context, const UpdRequest* request, UpdReply* reply) override {
		dirUpdLock.lock();
#if DEBUG
		cout << "In dirUpd" << endl;
		cout << "Call from host = " + request->host() << endl;
		cout << "pageAddr = " + to_string((uint64_t) request->pageaddr()) << endl;
#endif
		int outerLoc = -1;
		int innerLoc = -1;
		bool found = false;
		tuple<string, uint64_t> owner;
		dsmDataLock.lock();
		for (int i = 0; i < dsmData.size(); i++) {
			for (int j = 0; j < dsmData[i].size(); j++) {
				string hostName = get<0>(dsmData[i][j]);
				uint64_t pageAddr = get<1>(dsmData[i][j]);
				if (hostName == request->host() && pageAddr == request->pageaddr()) {
					outerLoc = i;
					innerLoc = j;
					owner = dsmData[i][j];
					found = true;
					break;
				}
			}
			if (found) {
				break;
			}
		}
		dsmDataLock.unlock();
		if (found) {
			ownerVecLock.lock();
			ownerVec[outerLoc] = innerLoc;
			ownerVecLock.unlock();
#if DEBUG
			cout << to_string(outerLoc) + ", " + to_string(innerLoc) + ", owner host = " + get<0>(owner) + ", pageAddr = " + to_string((uint64_t) get<1>(owner)) << endl;
			ownerVecLock.lock();
			printOwnerVec();
			ownerVecLock.unlock();
#endif
			dsmDataLock.lock();
			vector<tuple<string, uint64_t>> invalidateVec = dsmData[outerLoc];
			dsmDataLock.unlock();
			invalidateVec.erase(remove(invalidateVec.begin(), invalidateVec.end(), owner), invalidateVec.end());
#if DEBUG
			printInvalidateVec(invalidateVec);
#endif
			if (invalidateVec.size() > 0) {
				string dirHost = GetHostName();
				logLock.lock();
				if (!logFile.is_open()) {
					logFile.open(dirHost + "_" + gRPCLogFilename, fstream::out | fstream::app);
				}
				logLock.unlock();
				for (int i = 0; i < invalidateVec.size(); i++) {
					logLock.lock();
					logFile << "RPC call from " + dirHost + " to " + get<0>(invalidateVec[i]) + " for invalidate with arguments pageAddr = " + to_string((uint64_t) get<1>(invalidateVec[i])) << endl;
					logLock.unlock();
					InvalidateClient invalidateClient(CreateChannel(get<0>(invalidateVec[i]) + ":" + to_string(port), InsecureChannelCredentials()));
					invalidateClient.invalidate((uint64_t) get<1>(invalidateVec[i]));
				}
				logLock.lock();
				if (logFile.is_open()) {
					logFile.close();
				}
				logLock.unlock();
			}
		}
		dirUpdLock.unlock();
		return Status::OK;
	}
};

class DirUpdAllocServiceImpl final : public DirUpdAlloc::Service {
	Status dirUpdAlloc(ServerContext* context, const UpdAllocRequest* request, UpdAllocReply* reply) override {
		dirUpdLock.lock();
#if DEBUG
		cout << "In dirUpdAlloc" << endl;
		cout << "Call from host = " + request->host() << endl;
		cout << "pointer = " + to_string((uint64_t) request->pointer()) << endl;
#endif
		bool found = false;
		string name = "";
		int iter = -1;
		allocateDsmDataLock.lock();
		map<string, map<string, vector<uint64_t>>>::iterator it;
		for (it = allocateDsmData.begin(); it != allocateDsmData.end(); it++) {
			if (it->second.count(request->host())) {
				vector<uint64_t> pointers = it->second[request->host()];
				for (int i = 0; i < pointers.size(); i++) {
					if (pointers[i] == ((uint64_t) request->pointer())) {
						iter = i;
						name = it->first;
						ownerMapLock.lock();
						ownerMap[it->first] = request->host();
						ownerMapLock.unlock();
						found = true;
						break;
					}
				}
				if (found) {
					break;
				}
			}
		}
		allocateDsmDataLock.unlock();
		if (found) {
#if DEBUG
			ownerMapLock.lock();
			cout << "owner name = " + name + ", owner host = " + ownerMap[name] << endl;
			printOwnerMap();
			ownerMapLock.unlock();
#endif
			map<string, vector<uint64_t>>::iterator itnew;
			allocateDsmDataLock.lock();
			map<string, vector<uint64_t>> hostPointers = allocateDsmData[name];
			allocateDsmDataLock.unlock();

			string dirHost = GetHostName();

			logLock.lock();
			if (!logFile.is_open()) {
				logFile.open(dirHost + "_" + gRPCLogFilename, fstream::out | fstream::app);
			}
			logLock.unlock();

			for (itnew = hostPointers.begin(); itnew != hostPointers.end(); itnew++) {
				if (itnew->first != request->host()) {
					logLock.lock();
					logFile << "RPC call from " + dirHost + " to " + itnew->first + " for invalidateAlloc with arguments pointer = " + to_string(itnew->second[iter]) << endl;
					logLock.unlock();
					InvalidateAllocClient invalidateAllocClient(CreateChannel(itnew->first + ":" + to_string(port), InsecureChannelCredentials()));
					invalidateAllocClient.invalidateAlloc(itnew->second[iter]);
				}
			}
			
			logLock.lock();
			if (logFile.is_open()) {
				logFile.close();
			}
			logLock.unlock();
		}
		dirUpdLock.unlock();
		return Status::OK;
	}
};

class InvalidateServiceImpl final : public Invalidate::Service {
	Status invalidate(ServerContext* context, const InvalidateRequest* request, InvalidateReply* reply) override {
#if DEBUG
		cout << "In invalidate" << endl;
		cout << "pageAddr = " + to_string((uint64_t) request->pageaddr()) << endl;
#endif
		mprotect((void *) request->pageaddr(), PAGE_SIZE, PROT_NONE);
#if DEBUG 
		cout << "mprotect done" << endl;
#endif
		return Status::OK;
	}
};

class InvalidateAllocServiceImpl final : public InvalidateAlloc::Service {
	Status invalidateAlloc(ServerContext* context, const InvalidateAllocRequest* request, InvalidateAllocReply* reply) override {
#if DEBUG
		cout << "In invalidateAlloc" << endl;
		cout << "pointer = " + to_string((uint64_t) request->pointer()) << endl;
#endif
		mprotect((void *) request->pointer(), PAGE_SIZE, PROT_NONE);
#if DEBUG 
		cout << "mprotect done" << endl;
#endif
		return Status::OK;
	}
};

class GetLatestServiceImpl final : public GetLatest::Service {
	Status getLatest(ServerContext* context, const LatestRequest* request, LatestReply* reply) override {
		getLatestLock.lock();
#if DEBUG
		cout << "In getLatest" << endl;
		cout << "Call from host = " + request->host() << endl;
		cout << "pageAddr = " + to_string((uint64_t) request->pageaddr()) << endl;
#endif
		int outerLoc = -1;
		int innerLoc = -1;
		bool found = false;
		tuple<string, uint64_t> owner;
		dsmDataLock.lock();
		for (int i = 0; i < dsmData.size(); i++) {
			for (int j = 0; j < dsmData[i].size(); j++) {
				string hostName = get<0>(dsmData[i][j]);
				uint64_t pageAddr = get<1>(dsmData[i][j]);
				if (hostName == request->host() && pageAddr == request->pageaddr()) {
					outerLoc = i;
					found = true;
					break;
				}
			}
			if (found) {
				break;
			}
		}
		dsmDataLock.unlock();
		if (found) {
			ownerVecLock.lock();
			innerLoc = ownerVec[outerLoc];
			ownerVecLock.unlock();
			dsmDataLock.lock();
			owner = dsmData[outerLoc][innerLoc];
			dsmDataLock.unlock();
#if DEBUG
			cout << to_string(outerLoc) + ", " + to_string(innerLoc) + ", owner host = " + get<0>(owner) + ", pageAddr = " + to_string((uint64_t) get<1>(owner)) << endl;
#endif
			string dirHost = GetHostName();
			logLock.lock();
			if (!logFile.is_open()) {
				logFile.open(dirHost + "_" + gRPCLogFilename, fstream::out | fstream::app);
			}
			logFile << "RPC call from " + dirHost + " to " + get<0>(owner) + " for fetchLatest with arguments pageAddr = " + to_string((uint64_t) get<1>(owner)) << endl;
			if (logFile.is_open()) {
				logFile.close();
			}
			logLock.unlock();
			FetchLatestClient fetchLatestClient(CreateChannel(get<0>(owner) + ":" + to_string(port), InsecureChannelCredentials()));
			fetchLatestClient.fetchLatest((uint64_t) get<1>(owner), reply);
#if DEBUG
			cout << "0th value in page at directory to be sent to the requester = " + to_string((uint64_t) reply->pagecontent(0)) << endl;
#endif
		}
		getLatestLock.unlock();
		return Status::OK;
	}
};

class GetLatestAllocServiceImpl final : public GetLatestAlloc::Service {
	Status getLatestAlloc(ServerContext* context, const LatestAllocRequest* request, LatestAllocReply* reply) override {
		getLatestLock.lock();
#if DEBUG
		cout << "In getLatestAlloc" << endl;
		cout << "Call from host = " + request->host() << endl;
		cout << "pointer = " + to_string((uint64_t) request->pointer()) << endl;
#endif
		bool found = false;
		uint64_t pointer = 0;
		string owner = "";
		allocateDsmDataLock.lock();
		map<string, map<string, vector<uint64_t>>>::iterator it;
		for (it = allocateDsmData.begin(); it != allocateDsmData.end(); it++) {
			if (it->second.count(request->host())) {
				vector<uint64_t> pointers = it->second[request->host()];
				for (int i = 0; i < pointers.size(); i++) {
					if (pointers[i] == ((uint64_t) request->pointer())) {
						ownerMapLock.lock();
						owner = ownerMap[it->first];
						pointer = allocateDsmData[it->first][ownerMap[it->first]][i];
						ownerMapLock.unlock();
						found = true;
						break;
					}
				}
				if (found) {
					break;
				}
			}
		}
		allocateDsmDataLock.unlock();
		if (found) {
#if DEBUG
			cout << "owner = " + owner + ", pointer = " + to_string(pointer) << endl;
#endif
			string dirHost = GetHostName();

			logLock.lock();
			if (!logFile.is_open()) {
				logFile.open(dirHost + "_" + gRPCLogFilename, fstream::out | fstream::app);
			}
			logFile << "RPC call from " + dirHost + " to " + owner + " for fetchLatestAlloc with arguments pointer = " + to_string(pointer) << endl;
			if (logFile.is_open()) {
				logFile.close();
			}
			logLock.unlock();
			FetchLatestAllocClient fetchLatestAllocClient(CreateChannel(owner + ":" + to_string(port), InsecureChannelCredentials()));
			fetchLatestAllocClient.fetchLatestAlloc(pointer, reply);
#if DEBUG
			cout << "0th value at pointer at directory to be sent to the requester = " + to_string((uint64_t) reply->pagecontent(0)) << endl;
#endif
		}
		getLatestLock.unlock();
		return Status::OK;
	}
};

class FetchLatestServiceImpl final : public FetchLatest::Service {
	Status fetchLatest(ServerContext* context, const FetchRequest* request, FetchReply* reply) override {
		fetchLatestLock.lock();
#if DEBUG
		cout << "In fetchLatest" << endl;
		cout << "pageAddr = " + to_string((uint64_t) request->pageaddr()) << endl;
#endif
		uint64_t *startAddr = (uint64_t *) request->pageaddr();
		for (int i = 0; i < PAGE_SIZE / sizeof(uint64_t); i++) {
			reply->add_pagecontent(startAddr[i]);
		}
#if DEBUG
		cout << "0th value in page at owner to be sent to the directory = " + to_string((uint64_t) reply->pagecontent(0)) << endl;
#endif
		fetchLatestLock.unlock();
		return Status::OK;
	}
};

class FetchLatestAllocServiceImpl final : public FetchLatestAlloc::Service {
	Status fetchLatestAlloc(ServerContext* context, const FetchAllocRequest* request, FetchAllocReply* reply) override {
		fetchLatestLock.lock();
#if DEBUG
		cout << "In fetchLatestAlloc" << endl;
		cout << "pointer = " + to_string((uint64_t) request->pointer()) << endl;
#endif
		uint64_t *initialPointer = (uint64_t *) request->pointer();
		for (int i = 0; i < PAGE_SIZE / sizeof(uint64_t); i++) {
			reply->add_pagecontent(initialPointer[i]);
		}
#if DEBUG
		cout << "0th value at pointer at owner to be sent to the directory = " + to_string((uint64_t) reply->pagecontent(0)) << endl;
#endif
		fetchLatestLock.unlock();
		return Status::OK;
	}
};

class CheckAllocateServiceImpl final : public CheckAllocate::Service {
	Status checkAllocate(ServerContext* context, const CheckRequest* request, CheckReply* reply) override {
#if DEBUG
		cout << "In checkAllocate" << endl;
		cout << "Call from host = " + request->host() << endl;
		cout << "name = " + request->name() << endl;
#endif
		bool present = false;
		allocateDsmDataLock.lock();
		if (allocateDsmData.count(request->name())) {
			if (allocateDsmData[request->name()].count(request->host())) {
				present = true;
			}
		}
		allocateDsmDataLock.unlock();
#if DEBUG
		string flag = present ? "true" : "false";
		cout << "present = " + flag << endl;
#endif
		reply->set_present(present);
		return Status::OK;
	}
};

class CheckAllocatePointerServiceImpl final : public CheckAllocatePointer::Service {
	Status checkAllocatePointer(ServerContext* context, const CheckPointerRequest* request, CheckPointerReply* reply) override {
#if DEBUG
		cout << "In checkAllocatePointer" << endl;
		cout << "Call from host = " + request->host() << endl;
		cout << "pointer = " + to_string((uint64_t) request->pointer()) << endl;
#endif
		bool present = false;
		allocateDsmDataLock.lock();
		map<string, map<string, vector<uint64_t>>>::iterator it;
		for (it = allocateDsmData.begin(); it != allocateDsmData.end(); it++) {
			if (it->second.count(request->host())) {
				vector<uint64_t> pointers = it->second[request->host()];
				for (int i = 0; i < pointers.size(); i++) {
					if (pointers[i] == ((uint64_t) request->pointer())) {
						present = true;
						break;
					}
				}
				if (present) {
					break;
				}
			}
		}
		allocateDsmDataLock.unlock();
#if DEBUG
		string flag = present ? "true" : "false";
		cout << "present = " + flag << endl;
#endif
		reply->set_present(present);
		return Status::OK;
	}
};

class GetPointerServiceImpl final : public GetPointer::Service {
	Status getPointer(ServerContext* context, const PointerRequest* request, PointerReply* reply) override {
#if DEBUG
		cout << "In getPointer" << endl;
		cout << "Call from host = " + request->host() << endl;
		cout << "name = " + request->name() << endl;
#endif
		// uint64_t pointer = 0;
		allocateDsmDataLock.lock();
		/*
		if (allocateDsmData.count(request->name())) {
			if (allocateDsmData[request->name()].count(request->host())) {
				pointer = allocateDsmData[request->name()][request->host()][0];
			}
		}
		*/
		reply->set_pointer(allocateDsmData[request->name()][request->host()][0]);
		allocateDsmDataLock.unlock();
#if DEBUG
		cout << "pointer = " + to_string(allocateDsmData[request->name()][request->host()][0]) << endl;
#endif
		// reply->set_pointer(pointer);
		return Status::OK;
	}
};

class AllocateServiceImpl final : public Allocate::Service {
	Status allocate(ServerContext* context, const AllocRequest* request, AllocReply* reply) override {
#if DEBUG
		cout << "In allocate" << endl;
		cout << "Call from host = " + request->host() << endl;
		cout << "name = " + request->name() << endl;
		cout << "pointer = " + to_string((uint64_t) request->pointer()) << endl;
		cout << "size = " + to_string((uint32_t) request->size()) << endl;
#endif
		int numPointers = ((int) request->size() / PAGE_SIZE) + 1;
#if DEBUG
		cout << "numPointers = " + to_string(numPointers) << endl;
#endif
		vector<uint64_t> pointers;
		for (int i = 0; i < numPointers; i++) {
			pointers.push_back(((uint64_t) request->pointer()) + ((uint64_t) i * PAGE_SIZE));
		}
		allocateDsmDataLock.lock();
		if (allocateDsmData.count(request->name())) {
			allocateDsmData[request->name()][request->host()] = pointers;
		}
		else {
			map<string, vector<uint64_t>> hostPointers;
			hostPointers[request->host()] = pointers;
			allocateDsmData[request->name()] = hostPointers;
			/*
			ownerMapLock.lock();
			ownerMap[request->name()] = "";
			ownerMapLock.unlock();
			*/
		}
#if DEBUG
		printAllocDsmData();
		// printOwnerMap();
#endif
		allocateDsmDataLock.unlock();
		return Status::OK;
	}
};

static void PollDirClient(string hostName, string dirHost) {
	string dirAddress = dirHost + ":" + to_string(port);
	PollClient pollClient(CreateChannel(dirAddress, InsecureChannelCredentials()));

	logFile << "RPC call from " + hostName + " to " + dirHost + " for poll with arguments none" << endl;
	bool response = pollClient.poll();

#if DEBUG
	cout << "Directory status: " << response << endl;
#endif
}

static void RunDirServer(string hostName) {
	string serverAddress = hostName + ":" + to_string(port);
	PollServiceImpl pollService;
	RegDataSegServiceImpl regDataSegService;
	DirUpdServiceImpl dirUpdService;
	DirUpdAllocServiceImpl dirUpdAllocService;
	GetLatestServiceImpl getLatestService;
	GetLatestAllocServiceImpl getLatestAllocService;
	CheckAllocateServiceImpl checkAllocateService;
	CheckAllocatePointerServiceImpl checkAllocatePointerService;
	GetPointerServiceImpl getPointerService;
	AllocateServiceImpl allocateService;

	ServerBuilder builder;
	builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
	builder.RegisterService(&pollService);
	builder.RegisterService(&regDataSegService);
	builder.RegisterService(&dirUpdService);
	builder.RegisterService(&dirUpdAllocService);
	builder.RegisterService(&getLatestService);
	builder.RegisterService(&getLatestAllocService);
	builder.RegisterService(&checkAllocateService);
	builder.RegisterService(&checkAllocatePointerService);
	builder.RegisterService(&getPointerService);
	builder.RegisterService(&allocateService);

	unique_ptr<Server> server(builder.BuildAndStart());
#if DEBUG
	cout << "Directory server listening on port: " << serverAddress << endl;
#endif

	server->Wait();
}

static void RunServer(string hostName) {
	string serverAddress = hostName + ":" + to_string(port);
	InvalidateServiceImpl invalidateService;
	InvalidateAllocServiceImpl invalidateAllocService;
	FetchLatestServiceImpl fetchLatestService;
	FetchLatestAllocServiceImpl fetchLatestAllocService;

	ServerBuilder builder;
	builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
	builder.RegisterService(&invalidateService);
	builder.RegisterService(&invalidateAllocService);
	builder.RegisterService(&fetchLatestService);
	builder.RegisterService(&fetchLatestAllocService);

	unique_ptr<Server> server(builder.BuildAndStart());
#if DEBUG
	cout << "Server listening on port: " << serverAddress << endl;
#endif

	server->Wait();
}

static vector<string> GetNodeList() {
	ifstream nodeListFile(nodeListFilename);
	string line;
	vector<string> lines;
	while (getline(nodeListFile, line)) {
		lines.push_back(line);
	}
	return lines;
}

// NOT USED
static string GetDirHost() {
	return GetNodeList().back();
}

static string CheckDirHost() {
	string dirHostName = GetHostName();
	if (GetNodeList().back() == dirHostName) {
		return dirHostName;
	}
	return "";
}

static string CheckHostName() {
	string hostName = GetHostName();
	vector<string> lines = GetNodeList();
	bool found = false;
	for (int i = 0; i < lines.size() - 1; i++) {
		if (hostName == lines[i]) {
			found = true;
			break;
		}
	}
	dirHost = lines.back();
	if (found) {
		return hostName;
	}
	return "";
}

bool initDir() {
	string dirHost = CheckDirHost();
	if (dirHost == "") {
		cout << "Current node's hostname is not the directory in " + nodeListFilename << endl;
		return false;
	}
	logFile.open(dirHost + "_" + gRPCLogFilename, fstream::out | fstream::trunc);
	logFile.close();
	thread server(RunDirServer, dirHost);
	server.detach();
#if DEBUG
	cout << "Directory server thread launched" << endl;
#endif
	return true;
}

static bool initServer() {
	string hostName = CheckHostName();
	if (hostName == "") {
		cout << "Current node's hostname not found in " + nodeListFilename << endl;
		return false;
	}
	logFile.open(hostName + "_" + gRPCLogFilename, fstream::out | fstream::trunc);
	thread server(RunServer, hostName);
	server.detach();
#if DEBUG
	cout << "Server thread launched" << endl;
#endif
	PollDirClient(hostName, dirHost);
#if DEBUG
	cout << "Client running" << endl;
#endif
	return true;
}

void stopDir() {
	while (1);
}

static void stopServer() {
	logFile.close();
	while (true);
}

static void segv_handler(int signum, siginfo_t *info, void *ucontext) {
	string hostName = GetHostName();
	mprotect((void *) PAGE_DOWN((uint64_t) info->si_addr), PAGE_SIZE, PROT_READ | PROT_WRITE);
	if (((ucontext_t *) ucontext)->uc_mcontext.gregs[REG_ERR] & 0x2) {
#if DEBUG
		cout << "Write fault on " + hostName + " at " + to_string((uint64_t) info->si_addr) << endl;
#endif

		logFile << "RPC call from " + hostName + " to " + dirHost + " for checkAllocatePointer with arguments host = " + hostName + ", pointer = " + to_string((uint64_t) PAGE_DOWN((uint64_t) info->si_addr)) << endl;
		CheckAllocatePointerClient checkAllocatePointerClient(CreateChannel(dirHost + ":" + to_string(port), InsecureChannelCredentials()));
		bool present = checkAllocatePointerClient.checkAllocatePointer(hostName, (uint64_t) PAGE_DOWN((uint64_t) info->si_addr));

		if (present) {
			// call malloc specific dirUpd
			logFile << "RPC call from " + hostName + " to " + dirHost + " for dirUpdAlloc with arguments host = " + hostName + ", pointer = " + to_string(PAGE_DOWN((uint64_t) info->si_addr)) << endl;
			DirUpdAllocClient dirUpdAllocClient(CreateChannel(dirHost + ":" + to_string(port), InsecureChannelCredentials()));
			dirUpdAllocClient.dirUpdAlloc(hostName, PAGE_DOWN((uint64_t) info->si_addr));
		}
		else {
			logFile << "RPC call from " + hostName + " to " + dirHost + " for dirUpd with arguments host = " + hostName + ", pageAddr = " + to_string(PAGE_DOWN((uint64_t) info->si_addr)) << endl;
			DirUpdClient dirUpdClient(CreateChannel(dirHost + ":" + to_string(port), InsecureChannelCredentials()));
			dirUpdClient.dirUpd(hostName, PAGE_DOWN((uint64_t) info->si_addr));
		}
    }
	else {
#if DEBUG
		cout << "Read fault on " + hostName + " at " + to_string((uint64_t) info->si_addr) << endl;
#endif

		logFile << "RPC call from " + hostName + " to " + dirHost + " for checkAllocatePointer with arguments host = " + hostName + ", pointer = " + to_string((uint64_t) PAGE_DOWN((uint64_t) info->si_addr)) << endl;
		CheckAllocatePointerClient checkAllocatePointerClient(CreateChannel(dirHost + ":" + to_string(port), InsecureChannelCredentials()));
		bool present = checkAllocatePointerClient.checkAllocatePointer(hostName, (uint64_t) PAGE_DOWN((uint64_t) info->si_addr));

		if (present) {
			// call malloc specific getLatest
			logFile << "RPC call from " + hostName + " to " + dirHost + " for getLatestAlloc with arguments host = " + hostName + ", pointer = " + to_string(PAGE_DOWN((uint64_t) info->si_addr)) << endl;
			GetLatestAllocClient getLatestAllocClient(CreateChannel(dirHost + ":" + to_string(port), InsecureChannelCredentials()));
			getLatestAllocClient.getLatestAlloc(hostName, PAGE_DOWN((uint64_t) info->si_addr));
		}
		else {
			logFile << "RPC call from " + hostName + " to " + dirHost + " for getLatest with arguments host = " + hostName + ", pageAddr = " + to_string(PAGE_DOWN((uint64_t) info->si_addr)) << endl;
			GetLatestClient getLatestClient(CreateChannel(dirHost + ":" + to_string(port), InsecureChannelCredentials()));
			getLatestClient.getLatest(hostName, PAGE_DOWN((uint64_t) info->si_addr));
		}

		mprotect((void *) PAGE_DOWN((uint64_t) info->si_addr), PAGE_SIZE, PROT_READ);
    }
}

static void register_segv_handler() {
	struct sigaction sa;
 	memset(&sa, 0, sizeof(struct sigaction));
 	sa.sa_sigaction = &segv_handler;
	sa.sa_flags = SA_SIGINFO;
 	sigaction(SIGSEGV, &sa, NULL);
}

static void initialize() {
	bool status = initServer();
	if (!status) {
#if DEBUG
		cout << "Server not initialized" << endl;
#endif
		exit(0);
	}
#if DEBUG
	cout << "Server initialized" << endl;
#endif

	register_segv_handler();
}

void psu_dsm_register_datasegment(void *psu_ds_start, size_t psu_ds_size) {
	if (!initialized) {
		initialize();
	}

	mprotect(psu_ds_start, psu_ds_size, PROT_READ);

	string hostName = GetHostName();

	logFile << "RPC call from " + hostName + " to " + dirHost + " for regDataSeg with arguments host = " + hostName + ", pageNum = " + to_string(pageNum) + ", pageAddr = " + to_string((uint64_t) psu_ds_start) + ", size = " + to_string((uint32_t) psu_ds_size) << endl;
	RegDataSegClient regDataSegClient(CreateChannel(dirHost + ":" + to_string(port), InsecureChannelCredentials()));
	regDataSegClient.regDataSeg(hostName, (uint64_t) psu_ds_start, (uint32_t) psu_ds_size);
}

void *psu_dsm_malloc(char *name, size_t size) {
	if (!initialized) {
		initialize();
	}

	// call dir to check if name is there or not
	// if there, call dir to get start address, return this
	// if not there, call valloc, then call dir to allocate
	// mprotect this region, then return this pointer
	string hostName = GetHostName();
	string allocName(name);
	void *pointer;

	logFile << "RPC call from " + hostName + " to " + dirHost + " for checkAllocate with arguments host = " + hostName + ", name = " + allocName << endl;
	CheckAllocateClient checkAllocateClient(CreateChannel(dirHost + ":" + to_string(port), InsecureChannelCredentials()));
	bool present = checkAllocateClient.checkAllocate(hostName, allocName);
	if (present) {
		logFile << "RPC call from " + hostName + " to " + dirHost + " for getPointer with arguments host = " + hostName + ", name = " + allocName << endl;
		GetPointerClient getPointerClient(CreateChannel(dirHost + ":" + to_string(port), InsecureChannelCredentials()));
		pointer = (void *) getPointerClient.getPointer(hostName, allocName);
#if DEBUG
		cout << "Received pointer for directory = " + to_string((uint64_t) pointer) << endl;
#endif
	}
	else {
		// pointer = valloc(size);
		pointer = pvalloc(size);
#if DEBUG
		cout << "valloced pointer = " + to_string((uint64_t) pointer) << endl;
#endif

		mprotect(pointer, size, PROT_READ);

		logFile << "RPC call from " + hostName + " to " + dirHost + " for allocate with arguments host = " + hostName + ", name = " + allocName + ", pointer = " + to_string((uint64_t) pointer) + ", size = " + to_string((uint32_t) size) << endl;
		AllocateClient allocateClient(CreateChannel(dirHost + ":" + to_string(port), InsecureChannelCredentials()));
		allocateClient.allocate(hostName, allocName, (uint64_t) pointer, (uint32_t) size);
	}
	return pointer;
}

void psu_dsm_free() {
	stopServer();
}
