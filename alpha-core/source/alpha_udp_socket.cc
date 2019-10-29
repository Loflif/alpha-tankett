// alpha_udp_socket.cc

#include "alpha.h"

#include <WinSock2.h>

namespace alpha {
   namespace {
      sockaddr_in to_sockaddr(const ip_address &addr) {
         sockaddr_in result = {};
         result.sin_family = AF_INET;
         result.sin_addr.s_addr = htonl(addr.host_);
         result.sin_port = htons(addr.port_);
         return result;
      }

      ip_address from_sockaddr(const sockaddr_in &addr) {
         return ip_address(ntohl(addr.sin_addr.s_addr), htons(addr.sin_port));
      }
   } // !anon

   udp_socket::udp_socket()
      : handle_(~0u)
   {
   }

   bool udp_socket::is_valid() const {
      return handle_ != ~0u;
   }

   bool udp_socket::open() {
      if (is_valid()) {
         return false;
      }

      uint32_t handle = (int)socket(AF_INET, SOCK_DGRAM, 0);
      if (handle == ~0u) {
         return false;
      }

      // note: enable non-blocking mode
      u_long non_blocking = 1;
      if (ioctlsocket(handle, FIONBIO, &non_blocking) != 0) {
         return false;
      }

      // note: INADDR_ANY == 0x00000000
      ip_address addr;
      sockaddr_in addr_in = to_sockaddr(addr);
      if (bind(handle, (const sockaddr *)& addr_in, sizeof(addr_in)) != 0) {
         return false;
      }

      handle_ = handle;

      return true;
   }

   bool udp_socket::open(ip_address &addr) {
      if (is_valid()) {
         return false;
      }

      uint32_t handle = (int)socket(AF_INET, SOCK_DGRAM, 0);
      if (handle == ~0u) {
         return false;
      }

      // note: enable non-blocking mode
      u_long non_blocking = 1;
      if (ioctlsocket(handle, FIONBIO, &non_blocking) != 0) {
         return false;
      }

      sockaddr_in addr_in = to_sockaddr(addr);
      if (bind(handle, (const sockaddr *)& addr_in, sizeof(addr_in)) != 0) {
         return false;
      }

      handle_ = handle;

      return true;
   }

   void udp_socket::close() {
      if (!is_valid()) {
         return;
      }

      closesocket(handle_);
      handle_ = ~0u;
   }

   bool udp_socket::send_to(const ip_address &address, byte_stream &stream) {
      if (!is_valid()) {
         return false;
      }

      sockaddr_in addr_in = to_sockaddr(address);
      const char *data = (const char *)stream.base_;
      int size = (int)stream.length();
      int result = sendto(handle_, data, size, 0, (const sockaddr *)& addr_in, sizeof(addr_in));
      if (result < 0) {
         return false;
      }

      return true;
   }

   bool udp_socket::recv_from(ip_address &address, byte_stream &stream) {
      if (!is_valid()) {
         return false;
      }

      char *base = (char *)stream.base_;
      int size = (int)stream.capacity();
      sockaddr_in addr_in = {};
      int remote_size = sizeof(addr_in);
      int result = recvfrom(handle_, base, size, 0, (sockaddr *)& addr_in, &remote_size);
      if (result < 0) {
         return false;
      }

      address.host_ = ntohl(addr_in.sin_addr.s_addr);
      address.port_ = ntohs(addr_in.sin_port);
      stream.at_ = const_cast<uint8_t *>(stream.base_) + result;

      return true;
   }

   bool udp_socket::address_of(ip_address &address) {
      if (!is_valid()) {
         return false;
      }

      int size = sizeof(sockaddr_in);
      sockaddr_in addr_in = {};
      addr_in.sin_family = AF_INET;
      if (getsockname(handle_, (sockaddr *)& addr_in, &size) != 0) {
         return false;
      }

      address = from_sockaddr(addr_in);

      return true;
   }
} // !alpha
