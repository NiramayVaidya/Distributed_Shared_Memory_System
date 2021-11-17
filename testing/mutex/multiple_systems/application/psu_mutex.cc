#include <bits/stdc++.h>
#include <grpcpp/grpcpp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <map>

#include "psu_mutex.h"
#include "mutex.grpc.pb.h"

using namespace std;
using namespace grpc;
using namespace mutex;

fstream logFile;

int port = 50000;
vector<string> remoteHosts;
string nodeListFilename = "node_list.txt";
string gRPCLogFilename = "gRPC_log.txt";
int nodeId = -1;

map<unsigned int, uint64_t> seqNums;
map<unsigned int, uint64_t> highestSeqNums;
map<unsigned int, bool> requestingCSs;

map<unsigned int, map<string, bool>> responses;
map<unsigned int, map<string, bool>> defers;

#if DEBUG
static inline string boolToStr(bool val) {
	return val ? "true" : "false";
}

static void printMap(map<string, bool> const &m) {
	for (auto const &pair: m) {
		cout << "{" << pair.first << ": " << pair.second << "}\t";
    }
	cout << endl;
}
#endif

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

class RecvRequestClient {
	public:
		RecvRequestClient(shared_ptr<Channel> channel) : stub_(RecvRequest::NewStub(channel)) {}

		void recvRequest(string clientHost, string remoteHost, unsigned int lockno) {
#if DEBUG
			cout << "In recvRequest client" << endl;
#endif
			RecvReq recvReq;
			recvReq.set_seqnum(seqNums[lockno]);
			recvReq.set_host(clientHost);
			recvReq.set_nodeid(nodeId);
			recvReq.set_lockno(lockno);

			RecvReply recvReply;

			ClientContext context;

#if DEBUG
			cout << "Response map for lockno = " + to_string(lockno) + " -> ";
			printMap(responses[lockno]);
#endif

			Status status = stub_->recvRequest(&context, recvReq, &recvReply);

			if (recvReply.reply()) {
				responses[lockno][remoteHost] = true;
			}

#if DEBUG
			cout << "Response map for lockno = " + to_string(lockno) + " -> ";
			printMap(responses[lockno]);
#endif
		}

	private:
		std::unique_ptr<RecvRequest::Stub> stub_;
};

class SendDefReplyClient {
	public:
		SendDefReplyClient(shared_ptr<Channel> channel) : stub_(SendDefReply::NewStub(channel)) {}

		void sendDefRep(string clientHost, unsigned int lockno) {
#if DEBUG
			cout << "In sendDefRep client" << endl;
#endif
			DefRep defRep;
			defRep.set_host(clientHost);
			defRep.set_lockno(lockno);

			DefReply defReply;

			ClientContext context;

			Status status = stub_->sendDefRep(&context, defRep, &defReply);
		}

	private:
		std::unique_ptr<SendDefReply::Stub> stub_;
};

class PollServiceImpl final : public Poll::Service {
	Status poll(ServerContext* context, const PollRequest* request, PollReply* reply) override {
		reply->set_active(true);
		return Status::OK;
	}
};

class RecvRequestServiceImpl final : public RecvRequest::Service {
	Status recvRequest(ServerContext* context, const RecvReq* request, RecvReply* reply) override {
#if DEBUG
		cout << "In recvRequest" << endl;
		cout << "lockno = " + to_string(request->lockno()) << endl;
		cout << "highestSeqNum = " + to_string(highestSeqNums[request->lockno()]) << endl;
#endif
		highestSeqNums[request->lockno()] = max(highestSeqNums[request->lockno()], (uint64_t) request->seqnum()) + 1;
#if DEBUG
		cout << "highestSeqNum = " + to_string(highestSeqNums[request->lockno()]) + ", request->seqNum = " + to_string(request->seqnum()) + ", seqNum = " + to_string(seqNums[request->lockno()]) + ", requestingCS = " + boolToStr(requestingCSs[request->lockno()]) + ", request->host = " + request->host() + ", host = " + GetHostName() + ", request->nodeId = " + to_string(request->nodeid()) + ", nodeId = " + to_string(nodeId) + ", lockno = " + to_string(request->lockno()) << endl;
#endif
		if (requestingCSs[request->lockno()] && ((request->seqnum() > seqNums[request->lockno()]) || ((request->seqnum() == seqNums[request->lockno()]) && (request->nodeid() > nodeId)))) {
			defers[request->lockno()][request->host()] = true;
			reply->set_reply(false);
#if DEBUG
			cout << "Deferred" << endl;
			cout << "Defers map for lockno = " + to_string(request->lockno()) + " -> ";
			printMap(defers[request->lockno()]);
#endif
		}
		else {
			reply->set_reply(true);
#if DEBUG
			cout << "Responded" << endl;
			cout << "Defers map for lockno = " + to_string(request->lockno()) + " -> ";
			printMap(defers[request->lockno()]);
#endif
		}
		return Status::OK;
	}
};

class SendDefReplyServiceImpl final : public SendDefReply::Service {
	Status sendDefRep(ServerContext* context, const DefRep* request, DefReply* reply) override {
#if DEBUG
		cout << "In sendDefRep" << endl;
		cout << "request->host = " + request->host() << endl;
		cout << "lockno = " + to_string(request->lockno()) << endl;
		cout << "Response map for lockno = " + to_string(request->lockno()) + " -> ";
		printMap(responses[request->lockno()]);
#endif
		responses[request->lockno()][request->host()] = true;
#if DEBUG
		cout << "Response map for lockno = " + to_string(request->lockno()) + " -> ";
		printMap(responses[request->lockno()]);
#endif
		return Status::OK;
	}
};

static void RunClient(string hostName, string remoteHost) {
	string targetAddress = remoteHost + ":" + to_string(port);
	PollClient pollClient(CreateChannel(targetAddress, InsecureChannelCredentials()));

	logFile << "RPC call from " + hostName + " to " + remoteHost + " for poll with arguments none" << endl;
	bool response = pollClient.poll();

#if DEBUG
	cout << "Server status: " << response << endl;
#endif
}

static void RunServer(string hostName) {
	string serverAddress = hostName + ":" + to_string(port);
	PollServiceImpl pollService;
	RecvRequestServiceImpl recvRequestService;
	SendDefReplyServiceImpl sendDefReplyService;

	ServerBuilder builder;
	builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
	builder.RegisterService(&pollService);
	builder.RegisterService(&recvRequestService);
	builder.RegisterService(&sendDefReplyService);

	unique_ptr<Server> server(builder.BuildAndStart());
#if DEBUG
	cout << "Server listening on port: " << serverAddress << endl;
#endif

	server->Wait();
}

string GetHostName() {
	char host[256];
	gethostname(host, sizeof(host));
	string hostName(host);
	return hostName;
}

static string CheckHostName() {
	string hostName = GetHostName();
	ifstream nodeListFile(nodeListFilename);
	string line;
	vector<string> lines;
	while (getline(nodeListFile, line)) {
		lines.push_back(line);
	}
	bool found = false;
	for (int i = 0; i < lines.size() - 1; i++) {
		if (hostName == lines[i]) {
			nodeId = i;
			found = true;
			break;
		}
	}
	remoteHosts = lines;
	remoteHosts.pop_back();
	if (found) {
		remoteHosts.erase(remove(remoteHosts.begin(), remoteHosts.end(), hostName), remoteHosts.end());
		return hostName;
	}
	return "";
}

void psu_mutex_lock(unsigned int lockno) {
	string hostName = GetHostName();
#if DEBUG
	cout << "In psu_mutex_lock" << endl;
	cout << "lockno = " + to_string(lockno) << endl;
#endif
	requestingCSs[lockno] = true;
	seqNums[lockno] = highestSeqNums[lockno] + 1;
	for (int i = 0; i < remoteHosts.size(); i++) {
		logFile << "RPC call from " + hostName + " to " + remoteHosts[i] + " for recvRequest with arguments seqNum = " + to_string(seqNums[lockno]) + ", host = " + remoteHosts[i] + ", nodeId = " + to_string(nodeId) + ", lockno = " + to_string(lockno) << endl;
		RecvRequestClient recvRequestClient(CreateChannel(remoteHosts[i] + ":" + to_string(port), InsecureChannelCredentials()));
		recvRequestClient.recvRequest(hostName, remoteHosts[i], lockno);
	}
#if DEBUG
	cout << "Response map for lockno = " + to_string(lockno) + " -> ";
	printMap(responses[lockno]);
#endif
	while (true) {
		int i;
		for (i = 0; i < remoteHosts.size(); i++) {
			if (!responses[lockno][remoteHosts[i]]) {
				break;
			}
		}
		if (i == remoteHosts.size()) {
			break;
		}
	}
#if DEBUG
	cout << "Response map for lockno = " + to_string(lockno) + " -> ";
	printMap(responses[lockno]);
#endif
}

void psu_mutex_unlock(unsigned int lockno) {
	string hostName = GetHostName();
#if DEBUG
	cout << "In psu_mutex_unlock" << endl;
	cout << "lockno = " + to_string(lockno) << endl;
#endif
	requestingCSs[lockno] = false;
	for (int i = 0; i < remoteHosts.size(); i++) {
		if (defers[lockno][remoteHosts[i]]) {
			logFile << "RPC call from " + hostName + " to " + hostName + " for sendDefRep with arguments host = " + remoteHosts[i] + ", lockno = " + to_string(lockno) << endl;
			SendDefReplyClient sendDefReplyClient(CreateChannel(remoteHosts[i] + ":" + to_string(port), InsecureChannelCredentials()));
			sendDefReplyClient.sendDefRep(hostName, lockno);
		}
	}
	for (int i = 0; i < remoteHosts.size(); i++) {
		if (defers[lockno][remoteHosts[i]]) {
			defers[lockno][remoteHosts[i]] = false;
		}
		if (responses[lockno][remoteHosts[i]]) {
			responses[lockno][remoteHosts[i]] = false;
		}
	}
}

bool psu_start_lock() {
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
	for (int i = 0; i < remoteHosts.size(); i++) {
		RunClient(hostName, remoteHosts[i]);
	}
#if DEBUG
	cout << "Client running" << endl;
#endif
	return true;
}

void psu_init_lock(unsigned int lockno) {
	seqNums[lockno] = 0;
	highestSeqNums[lockno] = 0;
	requestingCSs[lockno] = false;

	map<string, bool> response;
	map<string, bool> defer;

	for (int i = 0; i < remoteHosts.size(); i++) {
		response[remoteHosts[i]] = false;
		defer[remoteHosts[i]] = false;
	}

	responses[lockno] = response;
	defers[lockno] = defer;
}

void psu_destroy_lock(unsigned int lockno) {
	seqNums.erase(lockno);
	highestSeqNums.erase(lockno);
	responses.erase(lockno);
	defers.erase(lockno);
}

void psu_stop_lock() {
	logFile.close();
	while (1);
}
