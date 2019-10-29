// alpha_render_system_opengl.h

#ifndef ALPHA_RENDER_SYSTEM_OPENGL_H_INCLUDED
#define ALPHA_RENDER_SYSTEM_OPENGL_H_INCLUDED

#include "alpha.h"

namespace alpha {
   struct render_system_opengl : render_system {
      render_system_opengl();
      ~render_system_opengl();

      void make_current();

      // note: render system interface
      virtual void clear(const uint32 color = 0xff000000) final;
      virtual void projection(const int32 width, const int32 height) final;
      virtual void render(const sprite &sprite, const transform &transform) final;
      virtual void render(const text &text, const transform &transform) final;
      //virtual void debug_text(int32 x, int32 y, const uint32 color, const int32 scale, const char *format, ...) final;

      texture debug_font_texture_;
   };
} // !alpha

#endif // !ALPHA_RENDER_SYSTEM_OPENGL_H_INCLUDED
