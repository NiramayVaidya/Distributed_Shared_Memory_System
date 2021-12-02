LDFLAGS = -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl

CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++11 -g

GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

all: app1 directory

app1: mutex.pb.o mutex.grpc.pb.o psu_mutex.o dsm.pb.o dsm.grpc.pb.o psu_dsm.o psu_mr.o app1.o
	$(CXX) $^ $(LDFLAGS) -o $@

directory: dsm.pb.o dsm.grpc.pb.o psu_dsm.o directory.o
	$(CXX) $^ $(LDFLAGS) -o $@

%.grpc.pb.cc: %.proto
	protoc --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	protoc --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h app1 directory intermediate1.txt intermediate2.txt
