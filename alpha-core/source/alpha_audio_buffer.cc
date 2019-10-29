// alpha_audio_buffer.cc

#include "alpha.h"

#pragma warning(push)
#pragma warning(disable: 4244) // conversion from X to Y, possible loss of data
#pragma warning(disable: 4456) // declaration of X hides previous local declaration
#pragma warning(disable: 4457) // declaration of X hides function parameter
#pragma warning(disable: 4701) // potentially uninitialized local variable X used
#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_PUSHDATA_API
#include "stb_vorbis.c"
#pragma warning(pop)

#include "alpha_openal.h"

constexpr alpha::uint32 INVALID_BUFFER_HANDLE = ~0u;

static ALuint
openal_create_buffer(const alpha::uint32 rate, 
                     const alpha::uint32 channels, 
                     const alpha::uint64 size, 
                     const void *data) 
{
   ALenum format = AL_FORMAT_STEREO16;
   if (channels == 1) {
      format = AL_FORMAT_MONO16;
   }

   ALuint id = INVALID_BUFFER_HANDLE;
   alGenBuffers(1, &id);
   alBufferData(id, format, data, (ALsizei)size, rate);

   if (alGetError() != AL_NO_ERROR) {
      alDeleteBuffers(1, &id);
      id = INVALID_BUFFER_HANDLE;
   }

   return id;
}

static void 
openal_destroy_buffer(alpha::uint32 id) {
   alDeleteBuffers(1, &id);
}

namespace alpha {
   audio_buffer::audio_buffer()
      : handle_(INVALID_BUFFER_HANDLE)
      , length_(0)
      , rate_(44100)
      , channels_(2)
   {
   }

   bool audio_buffer::is_valid() const {
      return handle_ != INVALID_BUFFER_HANDLE;
   }

   bool audio_buffer::create_from_file(const string &filename) {
      dynamic_array<uint8> file_content;
      if (!file_system::load(filename, file_content)) {
         return false;
      }

      int channels = 0;
      int rate = 0;
      short *samples = nullptr;
      int count = stb_vorbis_decode_memory(file_content.data(), (int)file_content.size(), &channels, &rate, &samples);
      if (count == -1) {
         return false;
      }

      bool result = create_from_memory(rate, channels, count * sizeof(short) * channels, samples);
      free(samples);

      return result;
   }

   bool audio_buffer::create_from_memory(const uint32 rate, const uint32 channels, const uint64 size, const void *data) {
      if (is_valid()) {
         destroy();
      }

      handle_ = openal_create_buffer(rate, channels, size, data);

      return is_valid();
   }

   void audio_buffer::destroy() {
      if (is_valid()) {
         openal_destroy_buffer(handle_);
      }

      handle_ = INVALID_BUFFER_HANDLE;
   }
} // !alpha
