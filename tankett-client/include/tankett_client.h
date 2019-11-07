// tankett_client.h

#ifndef TANKETT_CLIENT_H_INCLUDED
#define TANKETT_CLIENT_H_INCLUDED

#include <alpha.h>
#include <tankett_shared.h>
#include <iostream>
#include <string>
#include "tile.h"
#include "tank.h"
#include "entityManager.h"

using namespace alpha;

namespace tankett {
	enum CLIENT_STATE {
		INIT,
		CONNECTION_REQUEST,
		CHALLENGE_RESPONSE,
		CONNECTED,
		DISCONNECTED
	};

	struct client_app : application {

		struct RemoteClientData {
			bool connected_ = false;
			uint32 ping_ = 0;
			uint8 eliminations_ = 0;
		};

		RemoteClientData remoteClientData_[4];

		client_app();

		virtual bool enter() final;
		virtual void exit() final;
		virtual bool tick() final;
		void render();
		void send(time dt);
		bool pack_payload(protocol_payload& pPayload);
		bool send_payload(protocol_payload& pPayload);

		void receive();

		void initializeUI();
		void SetUIElement(UIElement &element, const char* pText, int32 pSize, vector2 pPos, uint32 color = 0xffffffff);

		void parsePayload(protocol_payload pPayload);
		void parseServerMessage(message_server_to_client pMessage);
		bool HandleQuitButton();
		void HandleConnectButton();
		void Disconnect();
		void Reconnect();
		bool DetectMouseHover(UIElement ui);
		bool DetectMouseClick(UIElement ui);
		void SetUI();
		void SetEliminationUI(int pID, UIElement &ui);
		void SetPingUI(int pID, UIElement& ui);
		void SetTimer(float pTime);
		void SetCoolDownDisplay();
		void renderUI(UIElement pUI);
		time current_;
		text debugText_;
		transform debugTextTransform_;

		udp_socket socket_;
		ip_address server_ip_;
		ip_address send_all_ip_;

		uint64 client_key_;
		uint64 server_key_;

		CLIENT_STATE state_;

		time send_accumulator;

		uint32 send_sequence_ = 0;
		uint32 latest_receive_sequence_ = 0;

		crypt::xorinator xorinator_;
		entityManager* entityManager_;
		
		dynamic_array<network_message_header*> messages_;
		message_client_to_server currentMessage_;
		//dynamic_array<network_message_header*>messages_;

		UIElement timer_;
		UIElement coolDown_;
		UIElement p1Eliminations;
		UIElement p2Eliminations;
		UIElement p3Eliminations;
		UIElement p4Eliminations;
		UIElement p1Ping;
		UIElement p2Ping;
		UIElement p3Ping;
		UIElement p4Ping;
		UIElement quitButton;
		UIElement connectButton;

		bool isDisconnected = false;

	};
} // !tankett

#endif // !TANKETT_CLIENT_H_INCLUDED