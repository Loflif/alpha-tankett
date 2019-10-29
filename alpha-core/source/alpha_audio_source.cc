// alpha_audio_source.cc

#include "alpha.h"

#include "alpha_openal.h"

constexpr alpha::uint32 INVALID_SOURCE_HANDLE = ~0u;

static ALuint
openal_create_source()
{
   ALuint id = INVALID_SOURCE_HANDLE;
   alGenSources(1, &id);

   if (alGetError() != AL_NO_ERROR) {
      alDeleteSources(1, &id);
      id = INVALID_SOURCE_HANDLE;
   }

   return id;
}

static void
openal_destroy_source(alpha::uint32 id) {
   alDeleteSources(1, &id);
}

namespace alpha {
   audio_source::audio_source()
      : handle_(INVALID_SOURCE_HANDLE)
   {
   }

   bool audio_source::is_valid() const {
      return handle_ != INVALID_SOURCE_HANDLE;
   }

   bool audio_source::is_playing() const {
      if (!is_valid()) {
         return false;
      }

      ALint status = 0;
      alGetSourcei(handle_, AL_SOURCE_STATE, &status);
      if (status == AL_PLAYING) {
         return true;
      }

      return false;
   }

   bool audio_source::has_buffer() const {
      if (!is_valid()) {
         return false;
      }

      ALint buffer = 0;
      alGetSourcei(handle_, AL_BUFFER, &buffer);

      return buffer != -1;
   }

   bool audio_source::create() {
      if (is_valid()) {
         return true;
      }

      handle_ = openal_create_source();

      return is_valid();
   }

   void audio_source::destroy() {
      if (is_valid()) {
         openal_destroy_source(handle_);
         handle_ = INVALID_SOURCE_HANDLE;
      }
   }
} // !alpha
