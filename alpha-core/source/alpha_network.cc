// alpha_network.cc

#include "alpha.h"

#include <WinSock2.h>

namespace alpha {
   bool network_init() {
      WSADATA data = {};
      int result = WSAStartup(MAKEWORD(2, 2), &data);
      return (result == 0);
   }

   void network_shut() {
      WSACleanup();
   }
} // !alpha
