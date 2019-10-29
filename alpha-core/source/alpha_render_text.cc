// alpha_render_text.cc

#include "alpha.h"

#include <cstdarg>

namespace alpha {
   text::text()
      : color_(0xffffffff)
      , scale_(1)
   {
   }

   text::text(const string &text, uint32 color)
      : color_(color)
      , scale_(1)
      , text_(text)
   {
   }

   void text::set_color(const uint32 color) {
      color_ = color;
   }

   void text::set_scale(const int32 scale) {
      scale_ = scale;
   }

   void text::set_text(const char *format, ...) {
      char text[2048] = { 0 };
      va_list args;
      va_start(args, format);
      size_t len = vsprintf_s(text, sizeof(text) - 1, format, args);
      va_end(args);
      text_ = text;
   }
} // !alpha
