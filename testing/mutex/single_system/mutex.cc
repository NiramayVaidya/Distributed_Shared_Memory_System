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

#include "mutex.grpc.pb.h"

#define DEBUG 0

using namespace std;
using namespace grpc;
using namespace mutex;

fstream logFile;

int port = 50000;
vector<int> remotePorts = {50001, 50002};
string nodeListFilename = "node_list.txt";
string gRPCLogFilename = "gRPC_log.txt";

uint64_t seqNum = 0;
uint64_t highestSeqNum = 0;
bool requestingCS = false;

map<int, bool> responses;
map<int, bool> defers;

#if DEBUG
inline string boolToStr(bool val) {
	return val ? "true" : "false";
}

void printMap(map<int, bool> const &m) {
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

		void recvRequest(int clientPort, int remotePort) {
#if DEBUG
			cout << "In recvRequest client" << endl;
#endif
			RecvReq recvReq;
			recvReq.set_seqnum(seqNum);
			recvReq.set_port(clientPort);

			RecvReply recvReply;

			ClientContext context;

#if DEBUG
			cout << "Responses map -> ";
			printMap(responses);
#endif

			Status status = stub_->recvRequest(&context, recvReq, &recvReply);

			if (recvReply.reply()) {
				responses[remotePort] = true;
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

		void sendDefRep(int clientPort) {
#if DEBUG
			cout << "In sendDefRep client" << endl;
#endif
			DefRep defRep;
			defRep.set_port(clientPort);

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
		cout << "highestSeqNum = " + to_string(highestSeqNum) + ", request->seqNum = " + to_string(request->seqnum()) + ", seqNum = " + to_string(seqNum) + ", requestingCS = " + boolToStr(requestingCS) + ", request->port = " + to_string(request->port()) + ", port = " + to_string(port) << endl;
#endif
		if (requestingCS && ((request->seqnum() > seqNum) || ((request->seqnum() == seqNum) && (request->port() > port)))) {
			defers[request->port()] = true;
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
		cout << "request->port = " + to_string(request->port()) << endl;
		cout << "Responses map -> ";
		printMap(responses);
#endif
		responses[request->port()] = true;
#if DEBUG
		cout << "Responses map -> ";
		printMap(responses);
#endif
		return Status::OK;
	}
};

void RunClient(string hostName, int remotePort) {
	string target_address = hostName + ":" + to_string(remotePort);
	PollClient pollClient(CreateChannel(target_address, InsecureChannelCredentials()));

	logFile << "RPC call from " + hostName + ":" + to_string(port) + " to " + hostName + ":" + to_string(remotePort) + " for poll with arguments none" << endl;
	bool response = pollClient.poll();

#if DEBUG
	cout << "Server status: " << response << endl;
#endif
}

void RunServer(string hostName) {
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

string CheckHostName() {
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
			found = true;
			break;
		}
	}
	if (found) {
		return hostName;
	}
	return "";
}

void enterMutex(string hostName) {
#if DEBUG
	cout << "In enterMutex" << endl;
#endif
	requestingCS = true;
	seqNum = highestSeqNum + 1;
	for (int i = 0; i < remotePorts.size(); i++) {
		logFile << "RPC call from " + hostName + ":" + to_string(port) + " to " + hostName + ":" + to_string(remotePorts[i]) + " for recvRequest with arguments seqNum = " + to_string(seqNum) + ", port = " + to_string(remotePorts[i]) << endl;
		RecvRequestClient recvRequestClient(CreateChannel(hostName + ":" + to_string(remotePorts[i]), InsecureChannelCredentials()));
		recvRequestClient.recvRequest(port, remotePorts[i]);
	}
#if DEBUG
	cout << "Responses map -> ";
	printMap(responses);
#endif
	while (true) {
		int i;
		for (i = 0; i < remotePorts.size(); i++) {
			if (!responses[remotePorts[i]]) {
				break;
			}
		}
		if (i == remotePorts.size()) {
			break;
		}
	}
#if DEBUG
	cout << "Responses map -> ";
	printMap(responses);
#endif
}

void exitMutex(string hostName) {
#if DEBUG
	cout << "In exitMutex" << endl;
#endif
	requestingCS = false;
	for (int i = 0; i < remotePorts.size(); i++) {
		if (defers[remotePorts[i]]) {
			logFile << "RPC call from " + hostName + ":" + to_string(port) + " to " + hostName + ":" + to_string(remotePorts[i]) + " for sendDefRep with arguments port = " + to_string(remotePorts[i]) << endl;
			SendDefReplyClient sendDefReplyClient(CreateChannel(hostName + ":" + to_string(remotePorts[i]), InsecureChannelCredentials()));
			sendDefReplyClient.sendDefRep(port);
		}
	}
	for (int i = 0; i < remotePorts.size(); i++) {
		if (defers[remotePorts[i]]) {
			defers[remotePorts[i]] = false;
		}
		if (responses[remotePorts[i]]) {
			responses[remotePorts[i]] = false;
		}
	}
}

int main(int argc, char* argv[]) {
	string hostName = CheckHostName();
	if (hostName == "") {
		cout << "Current node's hostname not found in " + nodeListFilename << endl;
		return 0;
	}
	logFile.open(hostName + "_" + to_string(port) + "_" + gRPCLogFilename, fstream::in | fstream::out | fstream::trunc);
	thread server(RunServer, hostName);
#if DEBUG
	cout << "Server thread launched" << endl;
#endif
	for (int i = 0; i < remotePorts.size(); i++) {
		responses[remotePorts[i]] = false;
		defers[remotePorts[i]] = false;
	}
	for (int i = 0; i < remotePorts.size(); i++) {
		RunClient(hostName, remotePorts[i]);
	}
#if DEBUG
	cout << "Client run" << endl;
#endif
	enterMutex(hostName);
	cout << "CS begin" << endl;
	cout << time(0) << endl;
	cout << hostName << endl;
	this_thread::sleep_for(chrono::milliseconds(5000));
	cout << time(0) << endl;
	cout << "CS end" << endl;
	exitMutex(hostName);
	logFile.close();
	while(1);
	return 0;
}
