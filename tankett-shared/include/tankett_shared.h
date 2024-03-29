// tankett_shared.h
#pragma once
#ifndef TANKETT_SHARED_H_INCLUDED
#define TANKETT_SHARED_H_INCLUDED

#include <alpha.h>

using namespace alpha;

namespace tankett {
	static const float TILE_SIZE = 28;
	static const float TANK_SIZE = 1.5f;
	static const float BULLET_SIZE = 0.2f;
	static const float TANK_SPEED = 4.0f;
	static const float BULLET_SPEED = 8.0f;
	static const float ROUND_TIME = 90.0f;
	static const float FIRE_RATE = 1.0f;
	static const float ROUNDENDTIME = 20.0f;
	static const float TIME_OUT_THRESHOLD_SECONDS = 5;
	static const float REMOTE_TANK_UPDATE_DELAY = 100;
	static const float REMOTE_TANK_HISTORY_SPAN = 1000;

	static const int BULLET_MAX_COUNT = 10;
	static const int COLLISION_PAIR_COUNT = 4;
	static const int RESPAWN_MILLISECONDS = 3000;

	static const vector2 SPAWN_POINTS[4]{
			vector2(4,4),
			vector2(40,4),
			vector2(4,29),
			vector2(40,29),
	};

	enum ENTITY_TYPE {
		TANK,
		WALL,
		BULLET
	};

	enum TILE_TYPE {
		W,
		E
	};

	struct UIElement {
		text text_;
		transform transform_;
	};

	static const TILE_TYPE LEVEL[33][44]{
	   {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}, //1
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //2
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //3
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //4
	   {W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //5
	   {W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //6
	   {W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //7
	   {W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //8
	   {W,E,E,E,E,E,E,E,W,E,E,E,E,E,W,W,W,W,W,W,W,E,E,E,W,W,W,W,W,W,W,E,E,E,E,W,E,E,E,E,E,E,E,W}, //9
	   {W,E,E,E,E,E,E,E,W,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,W,E,E,E,E,E,E,E,W}, //10
	   {W,E,E,E,E,E,W,W,W,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,W,W,W,E,E,E,E,E,W}, //11
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //12
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //13
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //14
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //15
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //16
	   {W,W,W,W,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,W,W,W,W}, //17
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //18
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //19
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //20
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //21
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W}, //22
	   {W,E,E,E,E,E,W,W,W,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,W,W,W,E,E,E,E,E,W}, //23
	   {W,E,E,E,E,E,E,E,W,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,W,E,E,E,E,E,E,E,W}, //24
	   {W,E,E,E,E,E,E,E,W,E,E,E,E,E,W,W,W,W,W,W,W,E,E,E,W,W,W,W,W,W,W,E,E,E,E,W,E,E,E,E,E,E,E,W}, //25
	   {W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //26
	   {W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //27
	   {W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //28
	   {W,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,W}, //29
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //30
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //31
	   {W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,E,W}, //32
	   {W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W,W}, //33
	};
	
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
         assert(length_ <= sizeof(payload_));
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
	   NETWORK_MESSAGE_CLIENT_TO_SERVER,
	   NETWORK_MESSAGE_SERVER_TO_CLIENT,
	   NETWORK_MESSAGE_COUNT,
   };

   struct network_message_header {
	  network_message_header();
	  explicit network_message_header(network_message_type type);

	  virtual bool write(byte_stream_writer& writer) = 0;
	  virtual bool write(byte_stream_evaluator& evaluator) = 0;
	  virtual bool read(byte_stream_reader& reader) = 0;

	  uint8 type_;
   };

   struct network_message_ping : network_message_header {
      network_message_ping();
	  uint8 sequence_;

	  virtual bool write(byte_stream_writer& writer) final {
		  return serialize(writer);
	  }
	  virtual bool write(byte_stream_evaluator& evaluator) final {
		  return serialize(evaluator);
	  }
	  virtual bool read(byte_stream_reader& reader) final {
		  return serialize(reader);
	  }

      template <typename T>
      bool serialize(T &stream) {
         bool result = true;
         result &= stream.serialize(type_);
		 result &= stream.serialize(sequence_);
         return result;
      }
   };
   constexpr uint32 PROTOCOL_PAYLOAD_SIZE = 1200;

   struct message_client_to_server : network_message_header
   {
   public:

	   message_client_to_server() : network_message_header(NETWORK_MESSAGE_CLIENT_TO_SERVER)
	   {}

	   uint8 input_field = 0;
	   float turret_angle = .0f;
	   uint32 input_number = 0;

	   enum INPUT
	   {
		   SHOOT = 0,
		   RIGHT = 1,
		   LEFT = 2,
		   DOWN = 3,
		   UP = 4
	   };

	   bool get_input(INPUT input)
	   {
		   return (input_field >> input) & 1;
	   }

	   void set_input(bool shoot, bool right, bool left, bool down, bool up)
	   {
		   input_field = 0;
		   input_field |= (shoot << SHOOT);
		   input_field |= (right << RIGHT);
		   input_field |= (left << LEFT);
		   input_field |= (down << DOWN);
		   input_field |= (up << UP);
	   }

	   template<typename S>
	   bool serialize(S& stream)
	   {
		   bool result = true;
		   result &= stream.serialize(type_);
		   result &= stream.serialize(input_field);
		   result &= stream.serialize(turret_angle);
		   result &= stream.serialize(input_number);
		   return result;
	   }

	   virtual bool write(byte_stream_writer& writer) final
	   {
		   return serialize(writer);
	   }
	   virtual bool write(byte_stream_evaluator& evaluator) final
	   {
		   return serialize(evaluator);
	   }

	   virtual bool read(byte_stream_reader& reader) final
	   {
		   return serialize(reader);
	   }
   };

   struct bullet_data
   {
	   vector2 position;
	   uint8 id = 0;
   };

   struct server_to_client_data
   {
	   bool alive = true;
	   bool connected = false;
	   vector2 position = { 0,0 };
	   float angle = 0.0f;
	   uint8 eliminations = 0;
	   uint8 client_id = 255;
	   uint32 ping = ~0U;
	   uint8 bullet_count = 0;
	   bullet_data bullets[10];
   };

   enum GAME_STATE : uint8 {
	   ROUND_RUNNING,
	   WAITING_FOR_PLAYER,
	   ROUND_END,
   };

   struct message_server_to_client : network_message_header
   {
	   message_server_to_client() : network_message_header(NETWORK_MESSAGE_SERVER_TO_CLIENT)
	   {}

	   uint8 receiver_id = 0;
	   uint32 input_number = 0;
	   uint8 client_count = 0;
	   server_to_client_data client_data[4];
	   float timestamp = .0f; //time::now().as_milliseconds();
	   float round_time = .0f; //time remaining
	   GAME_STATE game_state = WAITING_FOR_PLAYER;

	   template<typename S>
	   bool serialize(S& stream)
	   {
		   bool result = true;

		   result &= stream.serialize(type_);

		   result &= stream.serialize(receiver_id);

		   result &= stream.serialize(input_number);

		   result &= stream.serialize(client_count);

		   assert(client_count > 0 && client_count <= 4);

		   //serialize client array
		   for (int i = 0; i < client_count; i++)
		   {
			   server_to_client_data& currentData = client_data[i];

			   uint8 bit_field = currentData.alive & 1;
			   bit_field |= (currentData.connected << 1);
			   result &= stream.serialize(bit_field);
			   currentData.alive = bit_field & 1;
			   currentData.connected = (bit_field >> 1) & 1;

			   result &= stream.serialize(currentData.position.x_);
			   result &= stream.serialize(currentData.position.y_);

			   result &= stream.serialize(currentData.angle);

			   result &= stream.serialize(currentData.eliminations);

			   result &= stream.serialize(currentData.client_id);

			   result &= stream.serialize(currentData.ping);

			   result &= stream.serialize(currentData.bullet_count);

			   assert(currentData.bullet_count <= 10);

			   for (int j = 0; j < currentData.bullet_count; j++)
			   {
				   result &= stream.serialize(currentData.bullets[j].position.x_);
				   result &= stream.serialize(currentData.bullets[j].position.y_);
				   result &= stream.serialize(currentData.bullets[j].id);
			   }
		   }

		   result &= stream.serialize(timestamp);
		   result &= stream.serialize(round_time);

		   uint8 s = (uint8)game_state;
		   result &= stream.serialize(s);
		   game_state = (GAME_STATE)s;

		   return result;
	   }

	   virtual bool write(byte_stream_writer& writer) final
	   {
		   return serialize(writer);
	   }
	   virtual bool write(byte_stream_evaluator& evaluator) final
	   {
		   return serialize(evaluator);
	   }

	   virtual bool read(byte_stream_reader& reader) final
	   {
		   return serialize(reader);
	   }
	   
   };
} // !tankett

#endif // !TANKETT_SHARED_H_INCLUDED
