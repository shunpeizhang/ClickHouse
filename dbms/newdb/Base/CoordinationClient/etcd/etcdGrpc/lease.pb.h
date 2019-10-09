// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: lease.proto

#ifndef PROTOBUF_INCLUDED_lease_2eproto
#define PROTOBUF_INCLUDED_lease_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3007000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3007001 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "gogo.pb.h"
#include "rpc.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_lease_2eproto

// Internal implementation detail -- do not use these members.
struct TableStruct_lease_2eproto {
  static const ::google::protobuf::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::ParseTable schema[3]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors_lease_2eproto();
namespace leasepb {
class Lease;
class LeaseDefaultTypeInternal;
extern LeaseDefaultTypeInternal _Lease_default_instance_;
class LeaseInternalRequest;
class LeaseInternalRequestDefaultTypeInternal;
extern LeaseInternalRequestDefaultTypeInternal _LeaseInternalRequest_default_instance_;
class LeaseInternalResponse;
class LeaseInternalResponseDefaultTypeInternal;
extern LeaseInternalResponseDefaultTypeInternal _LeaseInternalResponse_default_instance_;
}  // namespace leasepb
namespace google {
namespace protobuf {
template<> ::leasepb::Lease* Arena::CreateMaybeMessage<::leasepb::Lease>(Arena*);
template<> ::leasepb::LeaseInternalRequest* Arena::CreateMaybeMessage<::leasepb::LeaseInternalRequest>(Arena*);
template<> ::leasepb::LeaseInternalResponse* Arena::CreateMaybeMessage<::leasepb::LeaseInternalResponse>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace leasepb {

// ===================================================================

class Lease :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:leasepb.Lease) */ {
 public:
  Lease();
  virtual ~Lease();

  Lease(const Lease& from);

  inline Lease& operator=(const Lease& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  Lease(Lease&& from) noexcept
    : Lease() {
    *this = ::std::move(from);
  }

  inline Lease& operator=(Lease&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor() {
    return default_instance().GetDescriptor();
  }
  static const Lease& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Lease* internal_default_instance() {
    return reinterpret_cast<const Lease*>(
               &_Lease_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(Lease* other);
  friend void swap(Lease& a, Lease& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline Lease* New() const final {
    return CreateMaybeMessage<Lease>(nullptr);
  }

  Lease* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<Lease>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const Lease& from);
  void MergeFrom(const Lease& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  static const char* _InternalParse(const char* begin, const char* end, void* object, ::google::protobuf::internal::ParseContext* ctx);
  ::google::protobuf::internal::ParseFunc _ParseFunc() const final { return _InternalParse; }
  #else
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Lease* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // int64 ID = 1;
  void clear_id();
  static const int kIDFieldNumber = 1;
  ::google::protobuf::int64 id() const;
  void set_id(::google::protobuf::int64 value);

  // int64 TTL = 2;
  void clear_ttl();
  static const int kTTLFieldNumber = 2;
  ::google::protobuf::int64 ttl() const;
  void set_ttl(::google::protobuf::int64 value);

  // int64 RemainingTTL = 3;
  void clear_remainingttl();
  static const int kRemainingTTLFieldNumber = 3;
  ::google::protobuf::int64 remainingttl() const;
  void set_remainingttl(::google::protobuf::int64 value);

  // @@protoc_insertion_point(class_scope:leasepb.Lease)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::int64 id_;
  ::google::protobuf::int64 ttl_;
  ::google::protobuf::int64 remainingttl_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_lease_2eproto;
};
// -------------------------------------------------------------------

class LeaseInternalRequest :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:leasepb.LeaseInternalRequest) */ {
 public:
  LeaseInternalRequest();
  virtual ~LeaseInternalRequest();

  LeaseInternalRequest(const LeaseInternalRequest& from);

  inline LeaseInternalRequest& operator=(const LeaseInternalRequest& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  LeaseInternalRequest(LeaseInternalRequest&& from) noexcept
    : LeaseInternalRequest() {
    *this = ::std::move(from);
  }

  inline LeaseInternalRequest& operator=(LeaseInternalRequest&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor() {
    return default_instance().GetDescriptor();
  }
  static const LeaseInternalRequest& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const LeaseInternalRequest* internal_default_instance() {
    return reinterpret_cast<const LeaseInternalRequest*>(
               &_LeaseInternalRequest_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  void Swap(LeaseInternalRequest* other);
  friend void swap(LeaseInternalRequest& a, LeaseInternalRequest& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline LeaseInternalRequest* New() const final {
    return CreateMaybeMessage<LeaseInternalRequest>(nullptr);
  }

  LeaseInternalRequest* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<LeaseInternalRequest>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const LeaseInternalRequest& from);
  void MergeFrom(const LeaseInternalRequest& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  static const char* _InternalParse(const char* begin, const char* end, void* object, ::google::protobuf::internal::ParseContext* ctx);
  ::google::protobuf::internal::ParseFunc _ParseFunc() const final { return _InternalParse; }
  #else
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(LeaseInternalRequest* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // .etcdserverpb.LeaseTimeToLiveRequest LeaseTimeToLiveRequest = 1;
  bool has_leasetimetoliverequest() const;
  void clear_leasetimetoliverequest();
  static const int kLeaseTimeToLiveRequestFieldNumber = 1;
  const ::etcdserverpb::LeaseTimeToLiveRequest& leasetimetoliverequest() const;
  ::etcdserverpb::LeaseTimeToLiveRequest* release_leasetimetoliverequest();
  ::etcdserverpb::LeaseTimeToLiveRequest* mutable_leasetimetoliverequest();
  void set_allocated_leasetimetoliverequest(::etcdserverpb::LeaseTimeToLiveRequest* leasetimetoliverequest);

  // @@protoc_insertion_point(class_scope:leasepb.LeaseInternalRequest)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::etcdserverpb::LeaseTimeToLiveRequest* leasetimetoliverequest_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_lease_2eproto;
};
// -------------------------------------------------------------------

class LeaseInternalResponse :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:leasepb.LeaseInternalResponse) */ {
 public:
  LeaseInternalResponse();
  virtual ~LeaseInternalResponse();

  LeaseInternalResponse(const LeaseInternalResponse& from);

  inline LeaseInternalResponse& operator=(const LeaseInternalResponse& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  LeaseInternalResponse(LeaseInternalResponse&& from) noexcept
    : LeaseInternalResponse() {
    *this = ::std::move(from);
  }

  inline LeaseInternalResponse& operator=(LeaseInternalResponse&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor() {
    return default_instance().GetDescriptor();
  }
  static const LeaseInternalResponse& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const LeaseInternalResponse* internal_default_instance() {
    return reinterpret_cast<const LeaseInternalResponse*>(
               &_LeaseInternalResponse_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  void Swap(LeaseInternalResponse* other);
  friend void swap(LeaseInternalResponse& a, LeaseInternalResponse& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline LeaseInternalResponse* New() const final {
    return CreateMaybeMessage<LeaseInternalResponse>(nullptr);
  }

  LeaseInternalResponse* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<LeaseInternalResponse>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const LeaseInternalResponse& from);
  void MergeFrom(const LeaseInternalResponse& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  static const char* _InternalParse(const char* begin, const char* end, void* object, ::google::protobuf::internal::ParseContext* ctx);
  ::google::protobuf::internal::ParseFunc _ParseFunc() const final { return _InternalParse; }
  #else
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(LeaseInternalResponse* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // .etcdserverpb.LeaseTimeToLiveResponse LeaseTimeToLiveResponse = 1;
  bool has_leasetimetoliveresponse() const;
  void clear_leasetimetoliveresponse();
  static const int kLeaseTimeToLiveResponseFieldNumber = 1;
  const ::etcdserverpb::LeaseTimeToLiveResponse& leasetimetoliveresponse() const;
  ::etcdserverpb::LeaseTimeToLiveResponse* release_leasetimetoliveresponse();
  ::etcdserverpb::LeaseTimeToLiveResponse* mutable_leasetimetoliveresponse();
  void set_allocated_leasetimetoliveresponse(::etcdserverpb::LeaseTimeToLiveResponse* leasetimetoliveresponse);

  // @@protoc_insertion_point(class_scope:leasepb.LeaseInternalResponse)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::etcdserverpb::LeaseTimeToLiveResponse* leasetimetoliveresponse_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_lease_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Lease

// int64 ID = 1;
inline void Lease::clear_id() {
  id_ = PROTOBUF_LONGLONG(0);
}
inline ::google::protobuf::int64 Lease::id() const {
  // @@protoc_insertion_point(field_get:leasepb.Lease.ID)
  return id_;
}
inline void Lease::set_id(::google::protobuf::int64 value) {
  
  id_ = value;
  // @@protoc_insertion_point(field_set:leasepb.Lease.ID)
}

// int64 TTL = 2;
inline void Lease::clear_ttl() {
  ttl_ = PROTOBUF_LONGLONG(0);
}
inline ::google::protobuf::int64 Lease::ttl() const {
  // @@protoc_insertion_point(field_get:leasepb.Lease.TTL)
  return ttl_;
}
inline void Lease::set_ttl(::google::protobuf::int64 value) {
  
  ttl_ = value;
  // @@protoc_insertion_point(field_set:leasepb.Lease.TTL)
}

// int64 RemainingTTL = 3;
inline void Lease::clear_remainingttl() {
  remainingttl_ = PROTOBUF_LONGLONG(0);
}
inline ::google::protobuf::int64 Lease::remainingttl() const {
  // @@protoc_insertion_point(field_get:leasepb.Lease.RemainingTTL)
  return remainingttl_;
}
inline void Lease::set_remainingttl(::google::protobuf::int64 value) {
  
  remainingttl_ = value;
  // @@protoc_insertion_point(field_set:leasepb.Lease.RemainingTTL)
}

// -------------------------------------------------------------------

// LeaseInternalRequest

// .etcdserverpb.LeaseTimeToLiveRequest LeaseTimeToLiveRequest = 1;
inline bool LeaseInternalRequest::has_leasetimetoliverequest() const {
  return this != internal_default_instance() && leasetimetoliverequest_ != nullptr;
}
inline const ::etcdserverpb::LeaseTimeToLiveRequest& LeaseInternalRequest::leasetimetoliverequest() const {
  const ::etcdserverpb::LeaseTimeToLiveRequest* p = leasetimetoliverequest_;
  // @@protoc_insertion_point(field_get:leasepb.LeaseInternalRequest.LeaseTimeToLiveRequest)
  return p != nullptr ? *p : *reinterpret_cast<const ::etcdserverpb::LeaseTimeToLiveRequest*>(
      &::etcdserverpb::_LeaseTimeToLiveRequest_default_instance_);
}
inline ::etcdserverpb::LeaseTimeToLiveRequest* LeaseInternalRequest::release_leasetimetoliverequest() {
  // @@protoc_insertion_point(field_release:leasepb.LeaseInternalRequest.LeaseTimeToLiveRequest)
  
  ::etcdserverpb::LeaseTimeToLiveRequest* temp = leasetimetoliverequest_;
  leasetimetoliverequest_ = nullptr;
  return temp;
}
inline ::etcdserverpb::LeaseTimeToLiveRequest* LeaseInternalRequest::mutable_leasetimetoliverequest() {
  
  if (leasetimetoliverequest_ == nullptr) {
    auto* p = CreateMaybeMessage<::etcdserverpb::LeaseTimeToLiveRequest>(GetArenaNoVirtual());
    leasetimetoliverequest_ = p;
  }
  // @@protoc_insertion_point(field_mutable:leasepb.LeaseInternalRequest.LeaseTimeToLiveRequest)
  return leasetimetoliverequest_;
}
inline void LeaseInternalRequest::set_allocated_leasetimetoliverequest(::etcdserverpb::LeaseTimeToLiveRequest* leasetimetoliverequest) {
  ::google::protobuf::Arena* message_arena = GetArenaNoVirtual();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::google::protobuf::MessageLite*>(leasetimetoliverequest_);
  }
  if (leasetimetoliverequest) {
    ::google::protobuf::Arena* submessage_arena = nullptr;
    if (message_arena != submessage_arena) {
      leasetimetoliverequest = ::google::protobuf::internal::GetOwnedMessage(
          message_arena, leasetimetoliverequest, submessage_arena);
    }
    
  } else {
    
  }
  leasetimetoliverequest_ = leasetimetoliverequest;
  // @@protoc_insertion_point(field_set_allocated:leasepb.LeaseInternalRequest.LeaseTimeToLiveRequest)
}

// -------------------------------------------------------------------

// LeaseInternalResponse

// .etcdserverpb.LeaseTimeToLiveResponse LeaseTimeToLiveResponse = 1;
inline bool LeaseInternalResponse::has_leasetimetoliveresponse() const {
  return this != internal_default_instance() && leasetimetoliveresponse_ != nullptr;
}
inline const ::etcdserverpb::LeaseTimeToLiveResponse& LeaseInternalResponse::leasetimetoliveresponse() const {
  const ::etcdserverpb::LeaseTimeToLiveResponse* p = leasetimetoliveresponse_;
  // @@protoc_insertion_point(field_get:leasepb.LeaseInternalResponse.LeaseTimeToLiveResponse)
  return p != nullptr ? *p : *reinterpret_cast<const ::etcdserverpb::LeaseTimeToLiveResponse*>(
      &::etcdserverpb::_LeaseTimeToLiveResponse_default_instance_);
}
inline ::etcdserverpb::LeaseTimeToLiveResponse* LeaseInternalResponse::release_leasetimetoliveresponse() {
  // @@protoc_insertion_point(field_release:leasepb.LeaseInternalResponse.LeaseTimeToLiveResponse)
  
  ::etcdserverpb::LeaseTimeToLiveResponse* temp = leasetimetoliveresponse_;
  leasetimetoliveresponse_ = nullptr;
  return temp;
}
inline ::etcdserverpb::LeaseTimeToLiveResponse* LeaseInternalResponse::mutable_leasetimetoliveresponse() {
  
  if (leasetimetoliveresponse_ == nullptr) {
    auto* p = CreateMaybeMessage<::etcdserverpb::LeaseTimeToLiveResponse>(GetArenaNoVirtual());
    leasetimetoliveresponse_ = p;
  }
  // @@protoc_insertion_point(field_mutable:leasepb.LeaseInternalResponse.LeaseTimeToLiveResponse)
  return leasetimetoliveresponse_;
}
inline void LeaseInternalResponse::set_allocated_leasetimetoliveresponse(::etcdserverpb::LeaseTimeToLiveResponse* leasetimetoliveresponse) {
  ::google::protobuf::Arena* message_arena = GetArenaNoVirtual();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::google::protobuf::MessageLite*>(leasetimetoliveresponse_);
  }
  if (leasetimetoliveresponse) {
    ::google::protobuf::Arena* submessage_arena = nullptr;
    if (message_arena != submessage_arena) {
      leasetimetoliveresponse = ::google::protobuf::internal::GetOwnedMessage(
          message_arena, leasetimetoliveresponse, submessage_arena);
    }
    
  } else {
    
  }
  leasetimetoliveresponse_ = leasetimetoliveresponse;
  // @@protoc_insertion_point(field_set_allocated:leasepb.LeaseInternalResponse.LeaseTimeToLiveResponse)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace leasepb

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // PROTOBUF_INCLUDED_lease_2eproto
