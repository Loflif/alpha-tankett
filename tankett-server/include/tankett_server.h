// tankett_server.h

#ifndef TANKETT_SERVER_H_INCLUDED
#define TANKETT_SERVER_H_INCLUDED

#include <new>
#include <memory>

#include <alpha.h>
#include <tankett_shared.h>

using namespace alpha;

enum ClientState {
	CONNECTED,
	CHALLENGE,
	DENIED,
	DISCONNECTED,
};

namespace tankett {
   void debugf(const char *format, ...);

   struct client {
	   ClientState state_;
	   ip_address address_;
	   time connection_time_;
	   uint64 client_key_{};
	   uint64 server_key_{};
	   uint8 id_;
	   crypt::xorinator xorinator_;
	   uint32 latest_received_sequence_{};
	   time latest_receive_time_;
	   dynamic_array<network_message_header*> messages_;
   };

   struct server {
      server();

      bool init();
      void shut();
      void run();

      void update(const time &dt);
      void receive(const time &dt);
      void send(const time &dt);

	  bool hasAddress(ip_address addr);

	  void processConnectionRequest(ip_address remote, protocol_connection_request &msg);
	  void processChallengeResponse(ip_address remote, protocol_challenge_response  &msg);
	  void processPayload(ip_address remote, protocol_payload &msg);
	  void processDisconnect(ip_address remote, protocol_payload& msg);
	  void queueMessage(client &pClient);


	  void challengeClient(client &client);
	  
	  //void sendPayload(Client &client);
	  uint8 connectedClientCount();

	  void SpawnTank();

	  vector2 spawnPoints[4] {
		  vector2(4,4),
		  vector2(40,4),
		  vector2(4,29),
		  vector2(40,29),
	  };

	  server_to_client_data clientData[4];

      bool running_;
      time send_accumulator_;
      ip_address local_;

	  uint64 key;
	  uint32 sendSequence_ = 0;

	  udp_socket socket_;
	  uint8 dst_[1024 * 4];

	  // note: example below
	  void process_client_queues();
	  bool send_payload(const ip_address& remote, protocol_payload& packet);

	  uint32 server_sequence_{};
	  
	  dynamic_array<client> clients_;

   };
} // !tankett

#endif // !TANKETT_SERVER_H_INCLUDED
