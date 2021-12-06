// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: mutex.proto

#ifndef PROTOBUF_mutex_2eproto__INCLUDED
#define PROTOBUF_mutex_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace mutex {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_mutex_2eproto();
void protobuf_AssignDesc_mutex_2eproto();
void protobuf_ShutdownFile_mutex_2eproto();

class DefRep;
class DefReply;
class PollReply;
class PollRequest;
class RecvReply;
class RecvReq;

// ===================================================================

class PollRequest : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:mutex.PollRequest) */ {
 public:
  PollRequest();
  virtual ~PollRequest();

  PollRequest(const PollRequest& from);

  inline PollRequest& operator=(const PollRequest& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const PollRequest& default_instance();

  void Swap(PollRequest* other);

  // implements Message ----------------------------------------------

  inline PollRequest* New() const { return New(NULL); }

  PollRequest* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const PollRequest& from);
  void MergeFrom(const PollRequest& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(PollRequest* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:mutex.PollRequest)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_mutex_2eproto();
  friend void protobuf_AssignDesc_mutex_2eproto();
  friend void protobuf_ShutdownFile_mutex_2eproto();

  void InitAsDefaultInstance();
  static PollRequest* default_instance_;
};
// -------------------------------------------------------------------

class PollReply : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:mutex.PollReply) */ {
 public:
  PollReply();
  virtual ~PollReply();

  PollReply(const PollReply& from);

  inline PollReply& operator=(const PollReply& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const PollReply& default_instance();

  void Swap(PollReply* other);

  // implements Message ----------------------------------------------

  inline PollReply* New() const { return New(NULL); }

  PollReply* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const PollReply& from);
  void MergeFrom(const PollReply& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(PollReply* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional bool active = 1;
  void clear_active();
  static const int kActiveFieldNumber = 1;
  bool active() const;
  void set_active(bool value);

  // @@protoc_insertion_point(class_scope:mutex.PollReply)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  bool active_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_mutex_2eproto();
  friend void protobuf_AssignDesc_mutex_2eproto();
  friend void protobuf_ShutdownFile_mutex_2eproto();

  void InitAsDefaultInstance();
  static PollReply* default_instance_;
};
// -------------------------------------------------------------------

class RecvReq : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:mutex.RecvReq) */ {
 public:
  RecvReq();
  virtual ~RecvReq();

  RecvReq(const RecvReq& from);

  inline RecvReq& operator=(const RecvReq& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const RecvReq& default_instance();

  void Swap(RecvReq* other);

  // implements Message ----------------------------------------------

  inline RecvReq* New() const { return New(NULL); }

  RecvReq* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RecvReq& from);
  void MergeFrom(const RecvReq& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(RecvReq* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional uint64 seqNum = 1;
  void clear_seqnum();
  static const int kSeqNumFieldNumber = 1;
  ::google::protobuf::uint64 seqnum() const;
  void set_seqnum(::google::protobuf::uint64 value);

  // @@protoc_insertion_point(class_scope:mutex.RecvReq)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::uint64 seqnum_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_mutex_2eproto();
  friend void protobuf_AssignDesc_mutex_2eproto();
  friend void protobuf_ShutdownFile_mutex_2eproto();

  void InitAsDefaultInstance();
  static RecvReq* default_instance_;
};
// -------------------------------------------------------------------

class RecvReply : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:mutex.RecvReply) */ {
 public:
  RecvReply();
  virtual ~RecvReply();

  RecvReply(const RecvReply& from);

  inline RecvReply& operator=(const RecvReply& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const RecvReply& default_instance();

  void Swap(RecvReply* other);

  // implements Message ----------------------------------------------

  inline RecvReply* New() const { return New(NULL); }

  RecvReply* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RecvReply& from);
  void MergeFrom(const RecvReply& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(RecvReply* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional bool reply = 1;
  void clear_reply();
  static const int kReplyFieldNumber = 1;
  bool reply() const;
  void set_reply(bool value);

  // @@protoc_insertion_point(class_scope:mutex.RecvReply)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  bool reply_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_mutex_2eproto();
  friend void protobuf_AssignDesc_mutex_2eproto();
  friend void protobuf_ShutdownFile_mutex_2eproto();

  void InitAsDefaultInstance();
  static RecvReply* default_instance_;
};
// -------------------------------------------------------------------

class DefRep : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:mutex.DefRep) */ {
 public:
  DefRep();
  virtual ~DefRep();

  DefRep(const DefRep& from);

  inline DefRep& operator=(const DefRep& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const DefRep& default_instance();

  void Swap(DefRep* other);

  // implements Message ----------------------------------------------

  inline DefRep* New() const { return New(NULL); }

  DefRep* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const DefRep& from);
  void MergeFrom(const DefRep& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(DefRep* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:mutex.DefRep)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_mutex_2eproto();
  friend void protobuf_AssignDesc_mutex_2eproto();
  friend void protobuf_ShutdownFile_mutex_2eproto();

  void InitAsDefaultInstance();
  static DefRep* default_instance_;
};
// -------------------------------------------------------------------

class DefReply : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:mutex.DefReply) */ {
 public:
  DefReply();
  virtual ~DefReply();

  DefReply(const DefReply& from);

  inline DefReply& operator=(const DefReply& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const DefReply& default_instance();

  void Swap(DefReply* other);

  // implements Message ----------------------------------------------

  inline DefReply* New() const { return New(NULL); }

  DefReply* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const DefReply& from);
  void MergeFrom(const DefReply& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(DefReply* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional bool defer = 1;
  void clear_defer();
  static const int kDeferFieldNumber = 1;
  bool defer() const;
  void set_defer(bool value);

  // @@protoc_insertion_point(class_scope:mutex.DefReply)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  bool defer_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_mutex_2eproto();
  friend void protobuf_AssignDesc_mutex_2eproto();
  friend void protobuf_ShutdownFile_mutex_2eproto();

  void InitAsDefaultInstance();
  static DefReply* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// PollRequest

// -------------------------------------------------------------------

// PollReply

// optional bool active = 1;
inline void PollReply::clear_active() {
  active_ = false;
}
inline bool PollReply::active() const {
  // @@protoc_insertion_point(field_get:mutex.PollReply.active)
  return active_;
}
inline void PollReply::set_active(bool value) {
  
  active_ = value;
  // @@protoc_insertion_point(field_set:mutex.PollReply.active)
}

// -------------------------------------------------------------------

// RecvReq

// optional uint64 seqNum = 1;
inline void RecvReq::clear_seqnum() {
  seqnum_ = GOOGLE_ULONGLONG(0);
}
inline ::google::protobuf::uint64 RecvReq::seqnum() const {
  // @@protoc_insertion_point(field_get:mutex.RecvReq.seqNum)
  return seqnum_;
}
inline void RecvReq::set_seqnum(::google::protobuf::uint64 value) {
  
  seqnum_ = value;
  // @@protoc_insertion_point(field_set:mutex.RecvReq.seqNum)
}

// -------------------------------------------------------------------

// RecvReply

// optional bool reply = 1;
inline void RecvReply::clear_reply() {
  reply_ = false;
}
inline bool RecvReply::reply() const {
  // @@protoc_insertion_point(field_get:mutex.RecvReply.reply)
  return reply_;
}
inline void RecvReply::set_reply(bool value) {
  
  reply_ = value;
  // @@protoc_insertion_point(field_set:mutex.RecvReply.reply)
}

// -------------------------------------------------------------------

// DefRep

// -------------------------------------------------------------------

// DefReply

// optional bool defer = 1;
inline void DefReply::clear_defer() {
  defer_ = false;
}
inline bool DefReply::defer() const {
  // @@protoc_insertion_point(field_get:mutex.DefReply.defer)
  return defer_;
}
inline void DefReply::set_defer(bool value) {
  
  defer_ = value;
  // @@protoc_insertion_point(field_set:mutex.DefReply.defer)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace mutex

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_mutex_2eproto__INCLUDED