// alpha_texture.cc

#include "alpha.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/GL.h>

#define STBI_NO_STDIO
#define STBI_NO_LINEAR
#define STBI_NO_HDR
#define STBI_ONLY_PNG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static GLuint 
opengl_create_texture(int width, int height, const void *bitmap) {
   GLuint id = 0;
   glGenTextures(1, &id);
   glBindTexture(GL_TEXTURE_2D, id);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmap);
   glBindTexture(GL_TEXTURE_2D, 0);
   GLenum err = glGetError();
   if (err != GL_NO_ERROR) {
      assert(!"opengl texture create error");
   }
   return id;
}

namespace alpha {
   texture::texture()
      : handle_(0)
      , width_(0)
      , height_(0)
   {
   }

   bool texture::is_valid() const {
      return handle_ != 0;
   }

   bool texture::create_from_file(const char *filename) {
      if (is_valid()) {
         destroy();
      }

      dynamic_array<uint8> file_content;
      if (!file_system::load(filename, file_content)) {
         return false;
      }

      int width = 0, height = 0, comp = 0;
      auto bitmap = stbi_load_from_memory(file_content.data(), (int)file_content.size(),
                                          &width, &height, &comp, STBI_rgb_alpha);
      if (!bitmap) {
         return false;
      }

      handle_ = opengl_create_texture(width, height, bitmap);
      width_ = width;
      height_ = height;

      stbi_image_free(bitmap);

      return is_valid();
   }

   bool texture::create_from_memory(const int32 width, const int32 height, const void *data) {
      if (is_valid()) {
         destroy();
      }

      handle_ = opengl_create_texture(width, height, data);
      width_ = width;
      height_ = height;

      return is_valid();
   }

   void texture::destroy() {
      if (is_valid()) {
        glDeleteTextures(1, &handle_);
      }

      handle_ = 0;
      width_ = 0;
      height_ = 0;
   }
} // !alpha
