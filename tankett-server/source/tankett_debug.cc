// tankett_debug.cc

#include "tankett_server.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

namespace tankett {
   void debugf(const char *format, ...) {
      char text[4096] = {};
      va_list args;
      va_start(args, format);
      vsprintf_s(text, sizeof(text) - 1, format, args);
      va_end(args);
      puts(text);
   }
} // !tankett
