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

#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <map>
#include <tuple>
#include <mutex>
#include <chrono>

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
mutex dsmDataLock;

bool invalidateDone = false;

#if DEBUG
static void printDsmData() {
	cout << "dsmData ->" << endl;
	for (int i = 0; i < dsmData.size(); i++) {
		for (int j = 0; j < dsmData[i].size(); j++) {
			cout << to_string(i) + ", " + to_string(j) + ", host = " + get<0>(dsmData[i][j]) + ", pageAddr = " + to_string(get<1>(dsmData[i][j])) << endl;
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

class InvalidateClient {
	public:
		InvalidateClient(shared_ptr<Channel> channel) : stub_(Invalidate::NewStub(channel)) {}
		
		void invalidate(uint64_t pageAddr) {
			InvalidateRequest invalidateRequest;
			invalidateRequest.set_pageaddr(pageAddr);

			InvalidateReply invalidateReply;

			ClientContext context;
			chrono::time_point<chrono::system_clock> deadline = chrono::system_clock::now() + chrono::milliseconds(10);
			context.set_deadline(deadline);

			Status status = stub_->invalidate(&context, invalidateRequest, &invalidateReply);

			// REMOVE
			cout << "Invalidate returned" << endl;

			invalidateDone = true;

			// REMOVE
			if (!status.ok()) {
				cout << "Invalidate unsuccessful" << endl;
			} 
		}

	 private:
		std::unique_ptr<Invalidate::Stub> stub_;
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
				// REMOVE
				// cout << to_string(i) + ", " + to_string(latestReply.pagecontent(i)) + ", " + to_string(startAddr[i]) << endl;
				startAddr[i] = latestReply.pagecontent(i);
			}
#if DEBUG
			cout << "0th updated value in page at requester = " + to_string((uint64_t) startAddr[0]) << endl;
#endif
		}

	 private:
		std::unique_ptr<GetLatest::Stub> stub_;
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
				ownerVec.push_back(-1);
			}
			dsmDataLock.unlock();
			iter++;
			i++;
		}
		reply->set_pagenum(iter);
#if DEBUG
		printDsmData();
		printOwnerVec();
		cout << "Updated pageNum = " + to_string(iter) << endl;
#endif
		return Status::OK;
	}
};

class DirUpdServiceImpl final : public DirUpd::Service {
	Status dirUpd(ServerContext* context, const UpdRequest* request, UpdReply* reply) override {
#if DEBUG
		cout << "In dirUpd" << endl;
		cout << "Call from host = " + request->host() << endl;
		cout << "pageAddr = " + to_string((uint64_t) request->pageaddr()) << endl;
#endif
		int outerLoc = -1;
		int innerLoc = -1;
		bool found = false;
		tuple<string, uint64_t> owner;
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
		if (found) {
			dsmDataLock.lock();
			ownerVec[outerLoc] = innerLoc;
			dsmDataLock.unlock();
#if DEBUG
			cout << to_string(outerLoc) + ", " + to_string(innerLoc) + ", owner host = " + get<0>(owner) + ", pageAddr = " + to_string((uint64_t) get<1>(owner)) << endl;
			printOwnerVec();
#endif
			vector<tuple<string, uint64_t>> invalidateVec = dsmData[outerLoc];
			invalidateVec.erase(remove(invalidateVec.begin(), invalidateVec.end(), owner), invalidateVec.end());
#if DEBUG
			printInvalidateVec(invalidateVec);
#endif
			if (invalidateVec.size() > 0) {
				string dirHost = GetHostName();
				logFile.open(dirHost + "_" + gRPCLogFilename, fstream::in | fstream::out | fstream::app);
				for (int i = 0; i < invalidateVec.size(); i++) {
					logFile << "RPC call from " + dirHost + " to " + get<0>(invalidateVec[i]) + " for invalidate with arguments pageAddr = " + to_string((uint64_t) get<1>(invalidateVec[i])) << endl;
					InvalidateClient invalidateClient(CreateChannel(get<0>(invalidateVec[i]) + ":" + to_string(port), InsecureChannelCredentials()));
					invalidateClient.invalidate((uint64_t) get<1>(invalidateVec[i]));
					// REMOVE
					cout << "Invalidate done" << endl;
				}
				logFile.close();
			}
		}
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
		// REMOVE
		cout << "mprotect done" << endl; 
		return Status::OK;
	}
};

class GetLatestServiceImpl final : public GetLatest::Service {
	Status getLatest(ServerContext* context, const LatestRequest* request, LatestReply* reply) override {
#if DEBUG
		cout << "In getLatest" << endl;
		cout << "Call from host = " + request->host() << endl;
		cout << "pageAddr = " + to_string((uint64_t) request->pageaddr()) << endl;
#endif
		while (!invalidateDone) {
			this_thread::sleep_for(chrono::milliseconds(10));
		};
		invalidateDone = false;
		// REMOVE
		cout << "invalidateDone false" << endl;
		int outerLoc = -1;
		int innerLoc = -1;
		bool found = false;
		tuple<string, uint64_t> owner;
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
		if (found) {
			// REMOVE
			printOwnerVec();
			innerLoc = ownerVec[outerLoc];
			// REMOVE
			if (innerLoc != -1) {
				owner = dsmData[outerLoc][innerLoc];
#if DEBUG
				cout << to_string(outerLoc) + ", " + to_string(innerLoc) + ", owner host = " + get<0>(owner) + ", pageAddr = " + to_string((uint64_t) get<1>(owner)) << endl;
#endif
				string dirHost = GetHostName();
				logFile.open(dirHost + "_" + gRPCLogFilename, fstream::in | fstream::out | fstream::app);
				logFile << "RPC call from " + dirHost + " to " + get<0>(owner) + " for fetchLatest with arguments pageAddr = " + to_string((uint64_t) get<1>(owner)) << endl;
				logFile.close();
				FetchLatestClient fetchLatestClient(CreateChannel(get<0>(owner) + ":" + to_string(port), InsecureChannelCredentials()));
				fetchLatestClient.fetchLatest((uint64_t) get<1>(owner), reply);
#if DEBUG
				cout << "0th value in page at directory to be sent to the requester = " + to_string((uint64_t) reply->pagecontent(0)) << endl;
#endif
			}
		}
		// invalidateDone = false;
		return Status::OK;
	}
};

class FetchLatestServiceImpl final : public FetchLatest::Service {
	Status fetchLatest(ServerContext* context, const FetchRequest* request, FetchReply* reply) override {
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
	GetLatestServiceImpl getLatestService;

	ServerBuilder builder;
	builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
	builder.RegisterService(&pollService);
	builder.RegisterService(&regDataSegService);
	builder.RegisterService(&dirUpdService);
	builder.RegisterService(&getLatestService);

	unique_ptr<Server> server(builder.BuildAndStart());
#if DEBUG
	cout << "Directory server listening on port: " << serverAddress << endl;
#endif

	server->Wait();
}

static void RunServer(string hostName) {
	string serverAddress = hostName + ":" + to_string(port);
	InvalidateServiceImpl invalidateService;
	FetchLatestServiceImpl fetchLatestService;

	ServerBuilder builder;
	builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
	builder.RegisterService(&invalidateService);
	builder.RegisterService(&fetchLatestService);

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

// Not used for now
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
	logFile.open(dirHost + "_" + gRPCLogFilename, fstream::in | fstream::out | fstream::trunc);
	logFile.close();
	// REMOVE
	// logFile << "Dummy write" << endl;
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
	logFile.open(hostName + "_" + gRPCLogFilename, fstream::in | fstream::out | fstream::trunc);
	thread server(RunServer, hostName);
	server.detach();
#if DEBUG
	cout << "Server thread launched" << endl;
#endif
	// string dirHost = GetDirHost();
	PollDirClient(hostName, dirHost);
#if DEBUG
	cout << "Client running" << endl;
#endif
	return true;
}

void stopDir() {
	// logFile.close();
	while (1);
}

static void stopServer() {
	logFile.close();
	while (true);
	// while (1) {
	// 	this_thread::sleep_for(chrono::milliseconds(10000));
	// };
}

static void segv_handler(int signum, siginfo_t *info, void *ucontext) {
#if DEBUG
	// printf("Received signal: %d(SIGSEGV)\n", signum);
	// printf("%p\n", info->si_addr);
#endif
	string hostName = GetHostName();
	if (((ucontext_t *) ucontext)->uc_mcontext.gregs[REG_ERR] & 0x2) {
#if DEBUG
		cout << "Write fault on " + hostName + " at " + to_string((uint64_t) info->si_addr) << endl;
#endif

		mprotect(info->si_addr, PAGE_SIZE, PROT_READ | PROT_WRITE);

		logFile << "RPC call from " + hostName + " to " + dirHost + " for dirUpd with arguments host = " + hostName + ", pageAddr = " + to_string((uint64_t) info->si_addr) << endl;
		DirUpdClient dirUpdClient(CreateChannel(dirHost + ":" + to_string(port), InsecureChannelCredentials()));
		dirUpdClient.dirUpd(hostName, (uint64_t) info->si_addr);
    } else {
#if DEBUG
		cout << "Read fault on " + hostName + " at " + to_string((uint64_t) info->si_addr) << endl;
#endif

		// mprotect(info->si_addr, PAGE_SIZE, PROT_READ);
		// if (!(((uint64_t) info->si_addr) % PAGE_SIZE)) {

			// printf("val- %d\n", *((int *) info->si_addr));
			// mprotect(info->si_addr, PAGE_SIZE, PROT_READ | PROT_WRITE);
			mprotect((void *) PAGE_DOWN((uint64_t) info->si_addr), PAGE_SIZE, PROT_READ | PROT_WRITE);

			logFile << "RPC call from " + hostName + " to " + dirHost + " for getLatest with arguments host = " + hostName + ", pageAddr = " + to_string((uint64_t) info->si_addr) << endl;
			GetLatestClient getLatestClient(CreateChannel(dirHost + ":" + to_string(port), InsecureChannelCredentials()));
			getLatestClient.getLatest(hostName, PAGE_DOWN((uint64_t) info->si_addr));
		// }
    }
}

static void register_segv_handler() {
	struct sigaction sa;
 	memset(&sa, 0, sizeof(struct sigaction));
 	sa.sa_sigaction = &segv_handler;
	sa.sa_flags = SA_SIGINFO;
 	sigaction(SIGSEGV, &sa, NULL);
}

void psu_dsm_register_datasegment(void *psu_ds_start, size_t psu_ds_size) {
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

#if DEBUG
	// printf("first- %p\n", psu_ds_start);
#endif

	register_segv_handler();
	mprotect(psu_ds_start, psu_ds_size, PROT_READ);

	string hostName = GetHostName();

	logFile << "RPC call from " + hostName + " to " + dirHost + " for regDataSeg with arguments host = " + hostName + ", pageNum = " + to_string(pageNum) + ", pageAddr = " + to_string((uint64_t) psu_ds_start) + ", size = " + to_string((uint32_t) psu_ds_size) << endl;
	RegDataSegClient regDataSegClient(CreateChannel(dirHost + ":" + to_string(port), InsecureChannelCredentials()));
	regDataSegClient.regDataSeg(hostName, (uint64_t) psu_ds_start, (uint32_t) psu_ds_size);
}

void psu_dsm_free() {
	// stopServer();
	while (1);
}
