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

#include "mutex.grpc.pb.h"

using namespace std;
using namespace grpc;
using namespace mutex;

int port = 50000;
string nodeListFilename = "node_list.txt";
string gRPCLogFilename = "gRPC_log.txt";

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

class PollServiceImpl final : public Poll::Service {
	Status poll(ServerContext* context, const PollRequest* request, PollReply* reply) override {
		reply->set_active(true);
		return Status::OK;
	}
};

void RunClient(string hostName) {
	string remoteHostName = "localhost";
	string remoteServerIP = "127.0.1.1";
	int remotePort = 50000;
	string target_address = remoteServerIP + ":" + to_string(remotePort);
	PollClient client(CreateChannel(target_address, InsecureChannelCredentials()));

	fstream logFile;
	logFile.open(hostName + "_" + to_string(port) + "_" + gRPCLogFilename, fstream::in | fstream::out | fstream::trunc);

	logFile << "RPC call from " + hostName + " to " + remoteHostName + " for poll with arguments none" << endl;
	bool response = client.poll();

	logFile.close();

	cout << "Server status: " << response << endl;
}

void RunServer(string serverIP) {
	string serverAddress = serverIP + ":" + to_string(port);
	PollServiceImpl pollService;

	ServerBuilder builder;
	builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
	builder.RegisterService(&pollService);

	unique_ptr<Server> server(builder.BuildAndStart());
	cout << "Server listening on port: " << serverAddress << endl;

	server->Wait();
}

string GetHostName() {
	char host[256];
	gethostname(host, sizeof(host));
	string hostName(host);
	return hostName;
}

string GetIP() {
	char *IP;
	struct hostent *hostEntry;
	string hostName = GetHostName();
	char host[256];
	strcpy(host, hostName.c_str());
	hostEntry = gethostbyname(host);
	IP = inet_ntoa(*((struct in_addr*) hostEntry->h_addr_list[0]));
	string IPAddr(IP);
	return IPAddr;
}

string CheckIP() {
	string IPAddr = GetIP();
	ifstream nodeListFile(nodeListFilename);
	string line;
	vector<string> lines;
	while (getline(nodeListFile, line)) {
		lines.push_back(line);
	}
	bool found = false;
	for (int i = 0; i < lines.size() - 1; i++) {
		if (IPAddr == lines[i]) {
			found = true;
			break;
		}
	}
	if (found) {
		return IPAddr;
	}
	return "";
}

int main(int argc, char* argv[]) {
	string serverIP = CheckIP();
	if (serverIP == "") {
		cout << "Current node's IP not found in " + nodeListFilename << endl;
		return 0;
	}
	thread server(RunServer, serverIP);
	cout << "Server thread launched" << endl;
	RunClient(GetHostName());
	cout << "Client run" << endl;
	while(1);
	return 0;
}
