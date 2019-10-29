// tankett_shared.h

#ifndef TANKETT_SHARED_H_INCLUDED
#define TANKETT_SHARED_H_INCLUDED

#include <alpha.h>

using namespace alpha;

namespace tankett {
   struct allocator {
      virtual ~allocator() = default;
      virtual void *allocate(uint64 size, uint64 align) = 0;
      virtual void deallocate(void *pointer) = 0;
   };

   constexpr uint32 PROTOCOL_ID        = 0x11223344u;
   constexpr uint32 PROTOCOL_VERSION   = 0x01000001u;
   constexpr uint16 PROTOCOL_PORT      = 32100ui16;

   enum packet_type {
      PACKET_TYPE_CONNECTION_REQUEST,
      PACKET_TYPE_CONNECTION_DENIED,
      PACKET_TYPE_CONNECTION_CHALLENGE,
      PACKET_TYPE_CHALLENGE_RESPONSE,
      PACKET_TYPE_KEEP_ALIVE,
      PACKET_TYPE_PAYLOAD,
      PACKET_TYPE_DISCONNECT,
   };

   enum denied_reason_type {
      DENIED_REASON_TYPE_UNKNOWN,
      DENIED_REASON_TYPE_INVALID_CHALLENGE,
      DENIED_REASON_TYPE_WRONG_PROTOCOL,
      DENIED_REASON_TYPE_WRONG_VERSION,
      DENIED_REASON_TYPE_SERVER_FULL,
   };

   struct protocol_packet_header {
      protocol_packet_header();
      explicit protocol_packet_header(packet_type type);
     
      uint8 type_;
   };

   struct protocol_connection_request : protocol_packet_header {
      protocol_connection_request();
      explicit protocol_connection_request(uint64 client_key);

      template <typename T>
      bool serialize(T &stream) {
         bool result = true;
         result &= stream.serialize(type_);
         result &= stream.serialize(protocol_);
         result &= stream.serialize(version_);
         result &= stream.serialize(client_key_);
         result &= stream.serialize(sizeof(padding_), padding_);
         return result;
      }

      uint32 protocol_;
      uint32 version_;
      uint64 client_key_;
      uint8 padding_[1024];
   };

   struct protocol_connection_denied : protocol_packet_header {
      protocol_connection_denied();
      explicit protocol_connection_denied(denied_reason_type reason);

      template <typename T>
      bool serialize(T &stream) {
         bool result = true;
         result &= stream.serialize(type_);
         result &= stream.serialize(reason_);
         return result;
      }

      uint8 reason_;
   };

   struct protocol_connection_challenge : protocol_packet_header {
      protocol_connection_challenge();
      explicit protocol_connection_challenge(uint64 server_key);

      template <typename T>
      bool serialize(T &stream) {
         bool result = true;
         result &= stream.serialize(type_);
         result &= stream.serialize(server_key_);
         return result;
      }

      uint64 server_key_;
   };

   struct protocol_challenge_response : protocol_packet_header {
      protocol_challenge_response();
      explicit protocol_challenge_response(uint64 combined_key);

      template <typename T>
      bool serialize(T &stream) {
         bool result = true;
         result &= stream.serialize(type_);
         result &= stream.serialize(combined_key_);
         result &= stream.serialize(sizeof(padding_), padding_);
         return result;
      }

      // note: combined_key client_key ^ server_key
      // note: combined_key client_key ^ server_key
      uint64 combined_key_;
      uint8 padding_[1024];
   };

   struct protocol_keep_alive : protocol_packet_header {
      protocol_keep_alive();

      template <typename S>
      bool serialize(S &stream) {
         bool result = true;
         result &= stream.serialze(type_);
         return result;
      }
   };

   struct protocol_payload : protocol_packet_header {
      protocol_payload();
      explicit protocol_payload(uint32 sequence);

      bool is_newer(const uint32 sequence);

      template <typename T>
      bool serialize(T &stream) {
         bool result = true;
         result &= stream.serialize(type_);
         result &= stream.serialize(sequence_);
         result &= stream.serialize(length_);
         assert(length_ < sizeof(payload_));
         result &= stream.serialize(length_, payload_);
         return result;
      }

      uint32 sequence_;
      uint16 length_;
      uint8 payload_[1200];
   };

   struct protocol_disconnect : protocol_packet_header {
      protocol_disconnect();
      explicit protocol_disconnect(uint64 combined_key);

      template <typename T>
      bool serialize(T &stream) {
         bool result = true;
         result &= stream.serialize(type_);
         result &= stream.serialize(client_key_);
         return result;
      }

      uint64 client_key_;
   };

   enum network_message_type {
      NETWORK_MESSAGE_PING,
      NETWORK_MESSAGE_COUNT,
   };

   struct network_message_header {
      network_message_header();
      explicit network_message_header(network_message_type type);

      uint8 type_;
   };

   struct network_message_ping : network_message_header {
      network_message_ping();

      template <typename T>
      bool serialize(T &stream) {
         bool result = true;
         result &= stream.serialize(type_);
         return result;
      }
   };
} // !tankett

#endif // !TANKETT_SHARED_H_INCLUDED
