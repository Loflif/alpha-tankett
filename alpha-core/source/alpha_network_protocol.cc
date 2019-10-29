// alpha_network_protocol.cc

#include "alpha.h"

namespace alpha {
   protocol_packet_header::protocol_packet_header() 
      : type_(0xff)
   {
   }

   protocol_packet_header::protocol_packet_header(packet_type type)
      : type_((uint8)type)
   {
   }

   protocol_connection_request::protocol_connection_request()
      : protocol_packet_header(PACKET_TYPE_CONNECTION_REQUEST)
      , protocol_(PROTOCOL_ID)
      , version_(PROTOCOL_VERSION)
      , client_key_(0)
      , padding_{}
   {
   }

   protocol_connection_request::protocol_connection_request(uint64 client_key)
      : protocol_packet_header(PACKET_TYPE_CONNECTION_REQUEST)
      , protocol_(PROTOCOL_ID)
      , version_(PROTOCOL_VERSION)
      , client_key_(client_key)
      , padding_{}
   {
   }

   protocol_connection_denied::protocol_connection_denied()
      : protocol_packet_header(PACKET_TYPE_CONNECTION_DENIED)
      , reason_(0)
   {
   }

   protocol_connection_denied::protocol_connection_denied(denied_reason_type reason)
      : protocol_packet_header(PACKET_TYPE_CONNECTION_DENIED)
      , reason_((uint8)reason)
   {
   }

   protocol_connection_challenge::protocol_connection_challenge()
      : protocol_packet_header(PACKET_TYPE_CONNECTION_CHALLENGE)
      , server_key_(0)
   {
   }

   protocol_connection_challenge::protocol_connection_challenge(uint64 server_key)
      : protocol_packet_header(PACKET_TYPE_CONNECTION_CHALLENGE)
      , server_key_(server_key)
   {
   }

   protocol_challenge_response::protocol_challenge_response()
      : protocol_packet_header(PACKET_TYPE_CHALLENGE_RESPONSE)
      , combined_key_(0)
      , padding_{}
   {
   }

   protocol_challenge_response::protocol_challenge_response(uint64 combined_key)
      : protocol_packet_header(PACKET_TYPE_CHALLENGE_RESPONSE)
      , combined_key_(combined_key)
      , padding_{}
   {
   }

   protocol_payload::protocol_payload() 
      : protocol_packet_header(PACKET_TYPE_PAYLOAD)
      , sequence_(0)
      , length_(0)
      , payload_{}
   {
   }

   protocol_payload::protocol_payload(uint32 sequence)
      : protocol_packet_header(PACKET_TYPE_PAYLOAD)
      , sequence_(sequence)
      , length_(0)
      , payload_{}
   {
   }

   bool protocol_payload::is_newer(const uint32 sequence) {
      // note: evening reading matter
      //       https://www.ietf.org/rfc/rfc1323.txt
      return ((sequence_ > sequence) && (sequence_ - sequence <= 2147483647)) ||
             ((sequence_ < sequence) && (sequence  - sequence_ > 2147483647));
   }

   protocol_disconnect::protocol_disconnect()
      : protocol_packet_header(PACKET_TYPE_DISCONNECT)
      , client_key_(0)
   {
   }

   protocol_disconnect::protocol_disconnect(uint64 client_key)
      : protocol_packet_header(PACKET_TYPE_DISCONNECT)
      , client_key_(client_key)
   {
   }

   network_message_header::network_message_header()
      : type_(0xff)
   {
   }

   network_message_header::network_message_header(uint8 type)
      : type_(type)
   {
   }
} // !alpha
