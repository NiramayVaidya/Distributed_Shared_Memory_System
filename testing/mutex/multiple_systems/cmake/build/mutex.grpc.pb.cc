// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: mutex.proto

#include "mutex.pb.h"
#include "mutex.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace mutex {

static const char* Poll_method_names[] = {
  "/mutex.Poll/poll",
};

std::unique_ptr< Poll::Stub> Poll::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< Poll::Stub> stub(new Poll::Stub(channel, options));
  return stub;
}

Poll::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_poll_(Poll_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status Poll::Stub::poll(::grpc::ClientContext* context, const ::mutex::PollRequest& request, ::mutex::PollReply* response) {
  return ::grpc::internal::BlockingUnaryCall< ::mutex::PollRequest, ::mutex::PollReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_poll_, context, request, response);
}

void Poll::Stub::async::poll(::grpc::ClientContext* context, const ::mutex::PollRequest* request, ::mutex::PollReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::mutex::PollRequest, ::mutex::PollReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_poll_, context, request, response, std::move(f));
}

void Poll::Stub::async::poll(::grpc::ClientContext* context, const ::mutex::PollRequest* request, ::mutex::PollReply* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_poll_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::mutex::PollReply>* Poll::Stub::PrepareAsyncpollRaw(::grpc::ClientContext* context, const ::mutex::PollRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::mutex::PollReply, ::mutex::PollRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_poll_, context, request);
}

::grpc::ClientAsyncResponseReader< ::mutex::PollReply>* Poll::Stub::AsyncpollRaw(::grpc::ClientContext* context, const ::mutex::PollRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncpollRaw(context, request, cq);
  result->StartCall();
  return result;
}

Poll::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      Poll_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< Poll::Service, ::mutex::PollRequest, ::mutex::PollReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](Poll::Service* service,
             ::grpc::ServerContext* ctx,
             const ::mutex::PollRequest* req,
             ::mutex::PollReply* resp) {
               return service->poll(ctx, req, resp);
             }, this)));
}

Poll::Service::~Service() {
}

::grpc::Status Poll::Service::poll(::grpc::ServerContext* context, const ::mutex::PollRequest* request, ::mutex::PollReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


static const char* RecvRequest_method_names[] = {
  "/mutex.RecvRequest/recvRequest",
};

std::unique_ptr< RecvRequest::Stub> RecvRequest::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< RecvRequest::Stub> stub(new RecvRequest::Stub(channel, options));
  return stub;
}

RecvRequest::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_recvRequest_(RecvRequest_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status RecvRequest::Stub::recvRequest(::grpc::ClientContext* context, const ::mutex::RecvReq& request, ::mutex::RecvReply* response) {
  return ::grpc::internal::BlockingUnaryCall< ::mutex::RecvReq, ::mutex::RecvReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_recvRequest_, context, request, response);
}

void RecvRequest::Stub::async::recvRequest(::grpc::ClientContext* context, const ::mutex::RecvReq* request, ::mutex::RecvReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::mutex::RecvReq, ::mutex::RecvReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_recvRequest_, context, request, response, std::move(f));
}

void RecvRequest::Stub::async::recvRequest(::grpc::ClientContext* context, const ::mutex::RecvReq* request, ::mutex::RecvReply* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_recvRequest_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::mutex::RecvReply>* RecvRequest::Stub::PrepareAsyncrecvRequestRaw(::grpc::ClientContext* context, const ::mutex::RecvReq& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::mutex::RecvReply, ::mutex::RecvReq, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_recvRequest_, context, request);
}

::grpc::ClientAsyncResponseReader< ::mutex::RecvReply>* RecvRequest::Stub::AsyncrecvRequestRaw(::grpc::ClientContext* context, const ::mutex::RecvReq& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncrecvRequestRaw(context, request, cq);
  result->StartCall();
  return result;
}

RecvRequest::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      RecvRequest_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< RecvRequest::Service, ::mutex::RecvReq, ::mutex::RecvReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](RecvRequest::Service* service,
             ::grpc::ServerContext* ctx,
             const ::mutex::RecvReq* req,
             ::mutex::RecvReply* resp) {
               return service->recvRequest(ctx, req, resp);
             }, this)));
}

RecvRequest::Service::~Service() {
}

::grpc::Status RecvRequest::Service::recvRequest(::grpc::ServerContext* context, const ::mutex::RecvReq* request, ::mutex::RecvReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


static const char* SendDefReply_method_names[] = {
  "/mutex.SendDefReply/sendDefRep",
};

std::unique_ptr< SendDefReply::Stub> SendDefReply::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< SendDefReply::Stub> stub(new SendDefReply::Stub(channel, options));
  return stub;
}

SendDefReply::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_sendDefRep_(SendDefReply_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status SendDefReply::Stub::sendDefRep(::grpc::ClientContext* context, const ::mutex::DefRep& request, ::mutex::DefReply* response) {
  return ::grpc::internal::BlockingUnaryCall< ::mutex::DefRep, ::mutex::DefReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_sendDefRep_, context, request, response);
}

void SendDefReply::Stub::async::sendDefRep(::grpc::ClientContext* context, const ::mutex::DefRep* request, ::mutex::DefReply* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::mutex::DefRep, ::mutex::DefReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_sendDefRep_, context, request, response, std::move(f));
}

void SendDefReply::Stub::async::sendDefRep(::grpc::ClientContext* context, const ::mutex::DefRep* request, ::mutex::DefReply* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_sendDefRep_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::mutex::DefReply>* SendDefReply::Stub::PrepareAsyncsendDefRepRaw(::grpc::ClientContext* context, const ::mutex::DefRep& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::mutex::DefReply, ::mutex::DefRep, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_sendDefRep_, context, request);
}

::grpc::ClientAsyncResponseReader< ::mutex::DefReply>* SendDefReply::Stub::AsyncsendDefRepRaw(::grpc::ClientContext* context, const ::mutex::DefRep& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncsendDefRepRaw(context, request, cq);
  result->StartCall();
  return result;
}

SendDefReply::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      SendDefReply_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< SendDefReply::Service, ::mutex::DefRep, ::mutex::DefReply, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](SendDefReply::Service* service,
             ::grpc::ServerContext* ctx,
             const ::mutex::DefRep* req,
             ::mutex::DefReply* resp) {
               return service->sendDefRep(ctx, req, resp);
             }, this)));
}

SendDefReply::Service::~Service() {
}

::grpc::Status SendDefReply::Service::sendDefRep(::grpc::ServerContext* context, const ::mutex::DefRep* request, ::mutex::DefReply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace mutex

