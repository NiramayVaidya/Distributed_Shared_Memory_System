LDFLAGS = -L/usr/local/lib `pkg-config --libs protobuf grpc++`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl

CXX = g++
CPPFLAGS += `pkg-config --cflags protobuf grpc`
CXXFLAGS += -std=c++11 -g

GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

all: directory p1 p2 p3

directory: dsm.pb.o dsm.grpc.pb.o psu_dsm.o directory.o
	$(CXX) $^ $(LDFLAGS) -o $@

p1: dsm.pb.o dsm.grpc.pb.o psu_dsm.o p1.o
	$(CXX) $^ $(LDFLAGS) -o $@

p2: dsm.pb.o dsm.grpc.pb.o psu_dsm.o p2.o
	$(CXX) $^ $(LDFLAGS) -o $@

p3: dsm.pb.o dsm.grpc.pb.o psu_dsm.o p3.o
	$(CXX) $^ $(LDFLAGS) -o $@

%.grpc.pb.cc: %.proto
	protoc --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

%.pb.cc: %.proto
	protoc --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h directory p1 p2 p3
