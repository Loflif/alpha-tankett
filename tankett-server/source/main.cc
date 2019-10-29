// main.cc

#include "tankett_server.h"

int main(int argc, char **argv) {
   tankett::server server;
   if (server.init()) {
      server.run();
   }
   server.shut();
   return 0;
}
