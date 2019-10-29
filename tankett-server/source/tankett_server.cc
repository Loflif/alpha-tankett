// tankett_server.cc

#include "tankett_server.h"

namespace tankett {
   server::server()
      : running_(false) {
	   
	   key = crypt::generator::random_key();
   }

   bool server::init() {
      if (!network_init()) {
         debugf("[err] could not initialize network!");
         return false;
      }

      dynamic_array<ip_address> addressess;
      if (!ip_address::local_addresses(addressess)) {
         debugf("[err] could not retreive local ip addressess!");
         return false;
      }

      debugf("[Info] found %d ip addressess", (int)addressess.size());

      for (int index = 0; index < (int)addressess.size(); index++) {
         debugf("[Info]   #%d - %s", index, addressess[index].as_string());
      }

      local_ = addressess[0];
      local_.set_port(PROTOCOL_PORT);
	  
	  sock_.open(local_);

      running_ = true;

      return running_;
   }

   void server::shut() {
      network_shut();
   }

   void server::run() {
      debugf("[Info] server running at %s", local_.as_string());
      const time start = time::now();
      time current = time::now();
      while (running_) {
         time now = time::now();
         time dt = now - current;
         current = now;

         receive(dt);
         update(dt);
         send(dt);
      }
   }

   void server::update(const time &dt) {

   }

   void server::receive(const time& dt) {
	   ip_address remote;
	   byte_stream stream(1024 * 4, dst_);
	   byte_stream_reader reader(stream);
	   if (!sock_.recv_from(remote, stream)) {
		   return;
	   }
	   while (!reader.eos()) {
		   const uint8 type = reader.peek();

		   switch (type) {
		   case PACKET_TYPE_CONNECTION_REQUEST: {
			   protocol_connection_request msg;
			   if (!msg.serialize(reader)) {
				   debugf("[Warning] Tried to serialize connection request but it didnt work for some reason.");
				   break;
			   }
			   processConnectionRequest(remote, msg);
		   }
			   break;
		   case PACKET_TYPE_CHALLENGE_RESPONSE: {
			   protocol_challenge_response msg;
			   if (!msg.serialize(reader)) {
				   debugf("[Warning] Tried to serialize challenge response but it didnt work for some reason.");
				   break;
			   }
			   processChallengeResponse(remote, msg);
		   }
			break;
		   case PACKET_TYPE_PAYLOAD: {
			   protocol_payload msg;
			   if (!msg.serialize(reader)) {
				   debugf("[Warning] Tried to serialize challenge response but it didnt work for some reason.");
				   break;
			   }			   
			   processPayload(remote, msg);
		   }
			   break;
		   default:
			   return;
			   break;
		   }
	   }  
   }

   


   void server::send(const time &dt) {
      send_accumulator_ += dt;
      if (send_accumulator_ > time(100)) {
         send_accumulator_ -= time(100);

		 for (Client client : clients) {
			 switch (client.state_) {
			 case CHALLENGE:
				 challengeClient(client);
				 break;
			 case CONNECTED:
				 sendEmptyPayload(client);
				 break;
			 default:
				 break;
			 }
		 }

      }
   }

   bool server::hasAddress(ip_address addr) {
	   for (Client client : clients) {
		   if (client.address_ == addr) {
			   return true;
		   }
	   }
	   return false;
   }

   void server::processConnectionRequest(ip_address remote, protocol_connection_request &msg) {
	   if (!hasAddress(remote)) {
		   Client client;
		   client.address_ = remote;
		   client.state_ = CHALLENGE;
		   client.key_ = msg.client_key_;
		   clients.push_back(client);
		   debugf("[Info] New client added: %s", remote.as_string());
	   }
   }

   void server::processChallengeResponse(ip_address remote, protocol_challenge_response &msg) {
	   for (Client &client : clients) {
		   if (client.address_ == remote) {
			   crypt::xorinator xorinator(client.key_, key);
			   uint64 encryptedKeys = 0;
			   xorinator.encrypt(sizeof(uint64), (uint8*)&encryptedKeys);
			   if (encryptedKeys == msg.combined_key_) {
				   if (client.state_ != CONNECTED) {
					   client.state_ = CONNECTED;
					   debugf("[Info] Client connected: %s", client.address_.as_string());
				   }
			   }
			   else {
				   client.state_ = DENIED;
				   debugf("[Info] Client denied: %s", client.address_.as_string());
			   }
		   }
	  }
   }

   void server::processPayload(ip_address remote, protocol_payload& msg) {
	   for (Client& client : clients) {
		   if (client.address_ == remote) {
			   if (msg.is_newer(client.recieveSequence_)) {
				   client.recieveSequence_ = msg.sequence_;
				   //debugf("[Info] Payload received from: %s", client.address_.as_string());
			   }
		   }
	   }
   }

   void server::processDisconnect(ip_address remote, protocol_payload& msg) { //TODO: TEST!
	   int iterator = -1;
	   for (int i = 0; i < clients.size(); i++) {
		   if (clients[i].address_ == remote) {
			   iterator = i;
		   }
	   }
	   if (iterator >= 0) {
		   clients.erase(clients.begin() + iterator);
	   }
   }



   void server::challengeClient(Client &client) {
	   protocol_connection_challenge challenge(key);
	   
	   byte_stream stream(1024 * 4, dst_);
	   byte_stream_writer writer(stream);

	   challenge.serialize(writer);
	   if (!sock_.send_to(client.address_, stream)) {
		   debugf("[Warning] Tried to send but something went wrong");
	   }
	   //debugf("[Info] Client challenged: %s", client.address_.as_string());
	   
   }
   void server::sendEmptyPayload(Client &client) {
	   sendSequence_++;
	   protocol_payload payload(sendSequence_);

	   byte_stream stream(1024 * 4, dst_);
	   byte_stream_writer writer(stream);

	   payload.serialize(writer);
	   if (!sock_.send_to(client.address_, stream)) {
		   debugf("[Warning] Tried to send but something went wrong");
	   }
	   //debugf("[Info] Payload sent %s", client.address_.as_string());
   }
} // !tankett

