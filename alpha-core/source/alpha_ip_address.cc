// alpha_ip_address.cc

#define _CRT_SECURE_NO_WARNINGS 1
#include "alpha.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>


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
   
   // static
   bool ip_address::local_addresses(dynamic_array<ip_address> &addresses) {
      DWORD size = 0;
      GetAdaptersAddresses(AF_INET,
                           GAA_FLAG_INCLUDE_PREFIX,
                           NULL,
                           NULL,
                           &size);

      IP_ADAPTER_ADDRESSES *adapter_addresses = (IP_ADAPTER_ADDRESSES *)calloc(1, size);
      GetAdaptersAddresses(AF_INET,
                           GAA_FLAG_INCLUDE_PREFIX,
                           NULL,
                           adapter_addresses,
                           &size);

      for (IP_ADAPTER_ADDRESSES *iter = adapter_addresses; iter != NULL; iter = iter->Next) {
         if (iter->OperStatus == IfOperStatusUp && (iter->IfType == IF_TYPE_ETHERNET_CSMACD || iter->IfType == IF_TYPE_IEEE80211)) {
            for (IP_ADAPTER_UNICAST_ADDRESS *ua = iter->FirstUnicastAddress; ua != NULL; ua = ua->Next) {
               char addrstr[1024] = {};
               getnameinfo(ua->Address.lpSockaddr, ua->Address.iSockaddrLength, addrstr, sizeof(addrstr), NULL, 0, NI_NUMERICHOST);
               if (ua->Address.lpSockaddr->sa_family == AF_INET) {
                  sockaddr_in ai = *(sockaddr_in *)ua->Address.lpSockaddr;
                  ip_address address;
                  address.host_ = ntohl(ai.sin_addr.s_addr);
                  address.port_ = ntohs(ai.sin_port);
                  addresses.push_back(address);
               }
            }
         }
      }

      free(adapter_addresses);

      return !addresses.empty();
   }

   bool ip_address::dns_lookup(const string &dns, dynamic_array<ip_address> &addresses) {
      ADDRINFO *query_result = NULL;
      ADDRINFO hint = {};
      hint.ai_family = AF_INET;
      hint.ai_socktype = SOCK_DGRAM;
      bool result = getaddrinfo(dns.c_str(), NULL, &hint, &query_result) == 0;
      if (result) {
         ADDRINFO *iter = query_result;
         while (iter) {
            sockaddr_in addrin = *(sockaddr_in *)iter->ai_addr;
            ip_address address;
            address.host_ = ntohl(addrin.sin_addr.s_addr);
            address.port_ = ntohs(addrin.sin_port);
            addresses.push_back(address);

            iter = iter->ai_next;
         }
      }

      freeaddrinfo(query_result);

      return !addresses.empty();
   }

   ip_address::ip_address()
      : host_(0)
      , port_(0)
   {
   }

   ip_address::ip_address(uint8 a, uint8 b, uint8 c, uint8 d, uint16 port)
      : host_(0)
      , port_(port)
   {
      host_ |= ((uint32)a << 24);
      host_ |= ((uint32)b << 16);
      host_ |= ((uint32)c << 8);
      host_ |= ((uint32)d << 0);
   }

   ip_address::ip_address(uint32 host, uint16 port)
      : host_(host)
      , port_(port)
   {
   }

   bool ip_address::operator==(const ip_address &rhs) const {
      return host_ == rhs.host_ && port_ == rhs.port_;
   }

   bool ip_address::operator!=(const ip_address &rhs) const {
      return host_ != rhs.host_ || port_ != rhs.port_;
   }

   void ip_address::set_host(uint8 a, uint8 b, uint8 c, uint8 d) {
      host_ = 0;
      host_ |= ((uint32)a << 24);
      host_ |= ((uint32)b << 16);
      host_ |= ((uint32)c << 8);
      host_ |= ((uint32)d << 0);
   }

   void ip_address::set_port(uint16 port) {
      port_ = port;
   }

   const char *ip_address::as_string() const {
      static char buf[128] = {};
      sockaddr_in inaddr = to_sockaddr(*this);
      if (inet_ntop(AF_INET, &inaddr.sin_addr, buf, sizeof(buf))) {
         size_t len = strlen(buf);
         sprintf(buf + len, ":%d", port_);
      }
      
      return buf;
   }
} // !alpha
