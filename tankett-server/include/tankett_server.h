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

struct Client {
	ClientState state_;
	ip_address address_;
	uint64 key_;
	uint32 recieveSequence_ = 0;
	uint8 id_;
};

namespace tankett {
   void debugf(const char *format, ...);

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


	  void challengeClient(Client &client);
	  
	  void sendPayload(Client &client);
	  int connectedClientCount();

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

	  udp_socket sock_;
	  uint8 dst_[1024 * 4];
	  dynamic_array<Client> clients;

   };
} // !tankett

#endif // !TANKETT_SERVER_H_INCLUDED
