// tankett_server.h

#ifndef TANKETT_SERVER_H_INCLUDED
#define TANKETT_SERVER_H_INCLUDED

#include <new>
#include <memory>

#include <alpha.h>
#include <tankett_shared.h>
#include "serverEntityManager.h"

using namespace alpha;

enum ClientState {
	CONNECTED,
	CHALLENGE,
	DENIED,
	DISCONNECTED,
};

namespace tankett {
	void debugf(const char* format, ...);

	struct client {
		ClientState state_;
		ip_address address_;
		time ping_;
		uint64 client_key_{};
		uint64 server_key_{};
		uint8 id_ = 0;
		crypt::xorinator xorinator_;
		uint32 latest_received_sequence_{};
		time latest_receive_time_;
		dynamic_array<std::pair<uint8, time>> pings_;
		uint8 pingSequence_ = 0;
		dynamic_array<network_message_header*> messages_;
	};

	struct server {
		server();

		bool init();
		void shut();
		void run();

		void update(const time& dt);
		void receive(const time& dt);
		void send(const time& dt);

		bool hasAddress(ip_address addr);

		void updateClientData();
		void StartRound();
		void EndRound();

		void processConnectionRequest(ip_address remote, protocol_connection_request& msg);
		void processChallengeResponse(ip_address remote, protocol_challenge_response& msg);
		void processPayload(ip_address remote, protocol_payload& msg);
		void processDisconnect(ip_address remote);
		void parsePingMessage(network_message_ping message, uint8 clientID);
		void queueMessage(client& pClient);
		void queuePing(client& pClient);
		uint8 GetUnusedClientID();
		void DisconnectAfterTimeout();


		void challengeClient(client& client);

		uint8 connectedClientCount();

		void SpawnTank(int id);

		server_to_client_data clientData[4];

		serverEntityManager* entityManager_;
		bool running_;
		time send_accumulator_;
		ip_address local_;

		uint64 key;
		uint32 sendSequence_ = 0;

		udp_socket socket_;
		uint8 dst_[2048] = { NULL };

		void process_client_queues();
		bool send_payload(const ip_address& remote, protocol_payload& packet);

		uint32 server_sequence_{};

		dynamic_array<client> clients_;

		float currentRoundTime_ = ROUND_TIME;
		GAME_STATE state_ = WAITING_FOR_PLAYER;
	};
} // !tankett

#endif // !TANKETT_SERVER_H_INCLUDED
