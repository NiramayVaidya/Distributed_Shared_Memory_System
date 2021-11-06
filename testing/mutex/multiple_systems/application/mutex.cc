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
#include <chrono>
#include <ctime>

#include "mutex.h"
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

uint64_t seqNum = 0;
uint64_t highestSeqNum = 0;
bool requestingCS = false;

map<string, bool> responses;
map<string, bool> defers;

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

		void recvRequest(string clientHost, string remoteHost) {
#if DEBUG
			cout << "In recvRequest client" << endl;
#endif
			RecvReq recvReq;
			recvReq.set_seqnum(seqNum);
			recvReq.set_host(clientHost);
			recvReq.set_nodeid(nodeId);

			RecvReply recvReply;

			ClientContext context;

#if DEBUG
			cout << "Responses map -> ";
			printMap(responses);
#endif

			Status status = stub_->recvRequest(&context, recvReq, &recvReply);

			if (recvReply.reply()) {
				responses[remoteHost] = true;
			}

#if DEBUG
			cout << "Responses map -> ";
			printMap(responses);
#endif
		}

	private:
		std::unique_ptr<RecvRequest::Stub> stub_;
};

class SendDefReplyClient {
	public:
		SendDefReplyClient(shared_ptr<Channel> channel) : stub_(SendDefReply::NewStub(channel)) {}

		void sendDefRep(string clientHost) {
#if DEBUG
			cout << "In sendDefRep client" << endl;
#endif
			DefRep defRep;
			defRep.set_host(clientHost);

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
		cout << "highestSeqNum = " + to_string(highestSeqNum) << endl;
#endif
		highestSeqNum = max(highestSeqNum, (uint64_t) request->seqnum()) + 1;
#if DEBUG
		cout << "highestSeqNum = " + to_string(highestSeqNum) + ", request->seqNum = " + to_string(request->seqnum()) + ", seqNum = " + to_string(seqNum) + ", requestingCS = " + boolToStr(requestingCS) + ", request->host = " + request->host() + ", host = " + GetHostName() + ", request->nodeId = " + to_string(request->nodeid()) + ", nodeId = " + to_string(nodeId) << endl;
#endif
		if (requestingCS && ((request->seqnum() > seqNum) || (request->seqnum() == seqNum) && (request->nodeid() > nodeId))) {
			defers[request->host()] = true;
			reply->set_reply(false);
#if DEBUG
			cout << "Deferred" << endl;
			cout << "Defers map -> ";
			printMap(defers);
#endif
		}
		else {
			reply->set_reply(true);
#if DEBUG
			cout << "Responded" << endl;
			cout << "Defers map -> ";
			printMap(defers);
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
		cout << "Responses map -> ";
		printMap(responses);
#endif
		responses[request->host()] = true;
#if DEBUG
		cout << "Responses map -> ";
		printMap(responses);
#endif
		return Status::OK;
	}
};

static void RunClient(string hostName, string remoteHost) {
	string target_address = remoteHost + ":" + to_string(port);
	PollClient pollClient(CreateChannel(target_address, InsecureChannelCredentials()));

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

void enterMutex() {
	string hostName = GetHostName();
#if DEBUG
	cout << "In enterMutex" << endl;
#endif
	requestingCS = true;
	seqNum = highestSeqNum + 1;
	for (int i = 0; i < remoteHosts.size(); i++) {
		logFile << "RPC call from " + hostName + " to " + remoteHosts[i] + " for recvRequest with arguments seqNum = " + to_string(seqNum) + ", host = " + remoteHosts[i] + ", nodeId = " + to_string(nodeId) << endl;
		RecvRequestClient recvRequestClient(CreateChannel(remoteHosts[i] + ":" + to_string(port), InsecureChannelCredentials()));
		recvRequestClient.recvRequest(hostName, remoteHosts[i]);
	}
#if DEBUG
	cout << "Responses map -> ";
	printMap(responses);
#endif
	while (true) {
		int i;
		for (i = 0; i < remoteHosts.size(); i++) {
			if (!responses[remoteHosts[i]]) {
				break;
			}
		}
		if (i == remoteHosts.size()) {
			break;
		}
	}
#if DEBUG
	cout << "Responses map -> ";
	printMap(responses);
#endif
}

void exitMutex() {
	string hostName = GetHostName();
#if DEBUG
	cout << "In exitMutex" << endl;
#endif
	requestingCS = false;
	for (int i = 0; i < remoteHosts.size(); i++) {
		if (defers[remoteHosts[i]]) {
			logFile << "RPC call from " + hostName + " to " + hostName + " for sendDefRep with arguments host = " + remoteHosts[i] << endl;
			SendDefReplyClient sendDefReplyClient(CreateChannel(remoteHosts[i] + ":" + to_string(port), InsecureChannelCredentials()));
			sendDefReplyClient.sendDefRep(hostName);
		}
	}
	for (int i = 0; i < remoteHosts.size(); i++) {
		if (defers[remoteHosts[i]]) {
			defers[remoteHosts[i]] = false;
		}
		if (responses[remoteHosts[i]]) {
			responses[remoteHosts[i]] = false;
		}
	}
}

bool initMutex() {
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
		responses[remoteHosts[i]] = false;
		defers[remoteHosts[i]] = false;
	}
	for (int i = 0; i < remoteHosts.size(); i++) {
		RunClient(hostName, remoteHosts[i]);
	}
#if DEBUG
	cout << "Client running" << endl;
#endif
	return true;
}

void destroyMutex() {
	logFile.close();
	while(1);
}
