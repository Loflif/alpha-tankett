// alpha_openal.cc

#include "alpha_openal.h"

#include <math.h>
#include <dsound.h>

struct al_buffer_dsound {
   bool generated_{};
   int index_{};
   IDirectSoundBuffer *buffer_{};
};

struct al_source_dsound {
   bool generated_{};
   ALenum state_{};
   int index_{};
   IDirectSoundBuffer *buffer_{};
};

struct ALCdevice {
   ALCdevice()
   {
   }

   bool open() {
      HRESULT hr = S_OK;
      hr = DirectSoundCreate(NULL, &device_, NULL);
      if (FAILED(hr)) {
         return false;
      }
      hr = device_->SetCooperativeLevel(GetActiveWindow(), DSSCL_PRIORITY);
      if (FAILED(hr)) {
         return false;
      }

      WAVEFORMATEX format = {};
      format.cbSize = sizeof(WAVEFORMATEX);
      format.wFormatTag = WAVE_FORMAT_PCM;
      format.nChannels = 2;
      format.nSamplesPerSec = 44100;
      format.wBitsPerSample = 16;
      format.nBlockAlign = (format.nChannels * format.wBitsPerSample) / 8;
      format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

      DSBUFFERDESC desc = {};
      desc.dwSize = sizeof(DSBUFFERDESC);
      desc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
      hr = device_->CreateSoundBuffer(&desc, &primary_, NULL);
      if (FAILED(hr)) {
         device_->Release();
         return false;
      }

      hr = primary_->SetFormat(&format);
      if (FAILED(hr)) {
         device_->Release();
         primary_->Release();
         return false;
      }

      for (int index = 0; index < 256; index++) {
         buffers_[index].index_ = index;
      }

      return true;
   }

   void close() {
      for (auto &source : sources_) {
         if (source.buffer_) {
            source.buffer_->Stop();
            source.buffer_->Release();
            source.buffer_ = nullptr;
         }
      }

      for (auto &buffer : buffers_) {
         if (buffer.buffer_) {
            buffer.buffer_->Release();
            buffer.buffer_ = nullptr;
         }
      }

      if (primary_) {
         primary_->Release();
         primary_ = nullptr;
      }

      if (device_) {
         device_->Release();
         device_ = nullptr;
      }
   }

   ALCcontext *context_{};
   IDirectSound *device_{};
   IDirectSoundBuffer *primary_{};
   al_buffer_dsound buffers_[256]{};
   al_source_dsound sources_[256]{};
};

struct ALCcontext {
   ALCcontext()
   {
   }

   bool is_valid() const { 
      return device_ != nullptr; 
   }

   ALenum error_{};
   ALCdevice *device_{};
};

static ALCcontext *alc_context;

static bool alc_valid_context() {
   return alc_context && alc_context->is_valid();
}

static ALCdevice *alc_device() {
   return alc_context->device_;
}

static void al_set_error(ALenum err) {
   if (!alc_valid_context()) {
      return;
   }

   alc_context->error_ = err;
}

static int linear_to_logarithmic(float volume) {
   // note: clamp
   if (volume <= 0.0f) {
      return (int)DSBVOLUME_MIN;
   }
   else if (volume >= 1.0f) {
      return (int)DSBVOLUME_MAX;
   }

   return (int)(floorf(2000.0f * log10f(volume)));
}

ALCdevice *alcOpenDevice(const ALCchar *devicename) {
   ALCdevice *device = new ALCdevice;
   if (!device->open()) {
      delete device;
      device = nullptr;
   }
   return device;
}

ALCboolean alcCloseDevice(ALCdevice *device) {
   alcDestroyContext(device->context_);
   device->close();
   delete device;
   return true;
}

ALCcontext *alcCreateContext(ALCdevice *device, const ALCint *attrlist) {
   ALCcontext *context = new ALCcontext;
   context->device_ = device;
   device->context_ = context;
   return context;
}

ALCboolean alcMakeContextCurrent(ALCcontext *context) {
   alc_context = context;
   return true;
}

void alcDestroyContext(ALCcontext *context) {
   context->device_->context_ = nullptr;
   delete context;
}

ALenum alGetError(void) {
   if (alc_valid_context()) {
      ALenum result = alc_context->error_;
      alc_context->error_ = AL_NO_ERROR;
      return result;
   }
   return AL_INVALID_OPERATION;
}

void alGenSources(ALsizei n, ALuint *sources) {
   if (!alc_valid_context()) {
      return;
   }

   ALsizei count = 0;
   ALCdevice *device = alc_device();
   for (int index = 0; index < 256; index++) {
      auto &source = device->sources_[index];
      if (!source.generated_) {
         source.generated_ = true;
         sources[count++] = index;
         if (count == n) {
            break;
         }
      }
   }

   if (count != n) {
      al_set_error(AL_OUT_OF_MEMORY);
   }
}

void alDeleteSources(ALsizei n, const ALuint *sources) {
   if (!alc_valid_context()) {
      return;
   }

   ALCdevice *device = alc_device();
   for (int index = 0; index < n; index++) {
      auto &source = device->sources_[sources[index]];
      if (source.generated_) {
         source.generated_ = false;
         source.state_ = AL_INITIAL;
         if (source.buffer_) {
            source.buffer_->Release();
            source.buffer_ = nullptr;
         }
      }
   }
}

void alSourcef(ALuint index, ALenum param, ALfloat value) {
   if (!alc_valid_context()) {
      return;
   }

   ALCdevice *device = alc_device();
   auto &source = device->sources_[index];
   if (!source.generated_) {
      al_set_error(AL_INVALID_NAME);
      return;
   }

   if (!source.buffer_) {
      al_set_error(AL_INVALID_ENUM);
      return;
   }

   switch (param) {
      case AL_GAIN: 
      {
         source.buffer_->SetVolume(linear_to_logarithmic(value));
      } break;
      default:
      {
         al_set_error(AL_INVALID_ENUM);
      } break;
   }
}

void alSourcei(ALuint index, ALenum param, ALint value) {
   if (!alc_valid_context()) {
      return;
   }

   ALCdevice *device = alc_device();
   auto &source = device->sources_[index];
   if (!source.generated_) {
      al_set_error(AL_INVALID_NAME);
      return;
   }

   switch (param) {
      case AL_BUFFER:
      {
         auto &buffer = device->buffers_[value];
         if (!buffer.generated_) {
            al_set_error(AL_INVALID_VALUE);
            return;
         }

         if (source.buffer_) {
            source.buffer_->Release();
            source.buffer_ = nullptr;
         }

         IDirectSound *dsound = device->device_;
         IDirectSoundBuffer *duplicate = nullptr;
         HRESULT hr = dsound->DuplicateSoundBuffer(buffer.buffer_, &duplicate);
         if (FAILED(hr)) {
            al_set_error(AL_INVALID_OPERATION);
            return;
         }

         source.state_ = AL_INITIAL;
         source.index_ = buffer.index_;
         source.buffer_ = duplicate;
      } break;
      default:
      {
         al_set_error(AL_INVALID_ENUM);
      } break;
   }
}

void alGetSourcei(ALuint index, ALenum param, ALint *value) {
   if(!alc_valid_context()) {
      return;
   }

   if (!value) {
      al_set_error(AL_INVALID_VALUE);
      return;
   }

   *value = -1;

   ALCdevice *device = alc_device();
   auto &source = device->sources_[index];
   if (!source.generated_) {
      al_set_error(AL_INVALID_NAME);
      return;
   }

   if (!source.buffer_) {
      al_set_error(AL_INVALID_OPERATION);
      return;
   }

   DWORD status = 0;
   HRESULT hr = source.buffer_->GetStatus(&status);
   if (FAILED(hr)) {
      al_set_error(AL_INVALID_OPERATION);
      return;
   }

   if ((status & DSBSTATUS_PLAYING) == 0) {
      source.state_ = AL_STOPPED;
   }

   switch (param) {
      case AL_SOURCE_STATE:
      {
         *value = source.state_;
      } break;
      case AL_BUFFER:
      {
         *value = source.index_;
      } break;
      default:
      {
         al_set_error(AL_INVALID_ENUM);
      } break;
   }
}

void alSourcePlay(ALuint index) {
   if (!alc_valid_context()) {
      return;
   }

   ALCdevice *device = alc_device();
   auto &source = device->sources_[index];
   if (!source.generated_) {
      al_set_error(AL_INVALID_NAME);
      return;
   }

   if (!source.buffer_) {
      al_set_error(AL_INVALID_OPERATION);
      return;
   }

   source.state_ = AL_PLAYING;
   source.buffer_->Play(0, 0, 0);
}

void alSourceStop(ALuint index) {
   if (!alc_valid_context()) {
      return;
   }

   ALCdevice *device = alc_device();
   auto &source = device->sources_[index];
   if (!source.generated_) {
      al_set_error(AL_INVALID_NAME);
      return;
   }

   if (!source.buffer_) {
      al_set_error(AL_INVALID_OPERATION);
      return;
   }

   source.state_ = AL_STOPPED;
   source.buffer_->Stop();
}

void alSourceRewind(ALuint index) {
   if (!alc_valid_context()) {
      return;
   }

   ALCdevice *device = alc_device();
   auto &source = device->sources_[index];
   if (!source.generated_) {
      al_set_error(AL_INVALID_NAME);
      return;
   }

   if (!source.buffer_) {
      al_set_error(AL_INVALID_OPERATION);
      return;
   }

   source.state_ = AL_INITIAL;
   source.buffer_->Stop();
   source.buffer_->SetCurrentPosition(0);
}

void alSourcePause(ALuint index) {
   if (!alc_valid_context()) {
      return;
   }

   ALCdevice *device = alc_device();
   auto &source = device->sources_[index];
   if (!source.generated_) {
      al_set_error(AL_INVALID_NAME);
      return;
   }

   if (!source.buffer_) {
      al_set_error(AL_INVALID_OPERATION);
      return;
   }

   source.state_ = AL_PAUSED;
   source.buffer_->Stop();
}

void alGenBuffers(ALsizei n, ALuint *buffers) {
   if (!alc_valid_context()) {
      return;
   }

   ALsizei count = 0;
   ALCdevice *device = alc_device();
   for (int index = 0; index < 256; index++) {
      auto &buffer = device->buffers_[index];
      if (!buffer.generated_) {
         buffer.generated_ = true;
         buffers[count++] = index;
         if (count == n) {
            break;
         }
      }
   }

   if (count != n) {
      al_set_error(AL_OUT_OF_MEMORY);
   }
}

void alDeleteBuffers(ALsizei n, const ALuint *buffers) {
   if (!alc_valid_context()) {
      return;
   }

   ALCdevice *device = alc_device();
   for (int index = 0; index < n; index++) {
      auto &buffer = device->buffers_[buffers[index]];
      if (buffer.generated_) {
         buffer.generated_ = false;
         if (buffer.buffer_) {
            buffer.buffer_->Release();
            buffer.buffer_ = nullptr;
         }
      }
   }
}

void alBufferData(ALuint index, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq) {
   if (!alc_valid_context()) {
      return;
   }

   ALCdevice *device = alc_device();
   auto &buffer = device->buffers_[index];
   if (!buffer.generated_) {
      al_set_error(AL_INVALID_NAME);
      return;
   }

   WORD channels = 2;
   switch (format) {
      case AL_FORMAT_MONO8:
      case AL_FORMAT_MONO16:
         channels = 1;
         break;
   }

   WORD bit_count = 16;
   switch (format) {
      case AL_FORMAT_MONO8:
      case AL_FORMAT_STEREO8:
         bit_count = 8;
         break;
   }

   WAVEFORMATEX wave = {};
   wave.cbSize = sizeof(WAVEFORMATEX);
   wave.wFormatTag = WAVE_FORMAT_PCM;
   wave.nSamplesPerSec = freq;
   wave.wBitsPerSample = bit_count;
   wave.nChannels = channels;
   wave.nBlockAlign = (wave.wBitsPerSample / 8) * wave.nChannels;
   wave.nAvgBytesPerSec = wave.nSamplesPerSec * wave.nBlockAlign;

   DSBUFFERDESC desc = {};
   desc.dwSize = sizeof(DSBUFFERDESC);
   desc.dwFlags = DSBCAPS_CTRLVOLUME;
   desc.dwBufferBytes = size;
   desc.dwReserved = 0;
   desc.lpwfxFormat = &wave;
   desc.guid3DAlgorithm = GUID_NULL;

   IDirectSound *dsound = device->device_;
   IDirectSoundBuffer *temp = nullptr;
   HRESULT hr = dsound->CreateSoundBuffer(&desc, &temp, NULL);
   if (FAILED(hr)) {
      al_set_error(AL_INVALID_OPERATION);
      return;
   }

   hr = temp->QueryInterface(IID_IDirectSoundBuffer, (void **)&buffer.buffer_);
   temp->Release();
   temp = nullptr;
   if (FAILED(hr)) {
      al_set_error(AL_INVALID_VALUE);
      return;
   }

   void *dst = nullptr;
   DWORD len = 0;
   hr = buffer.buffer_->Lock(0, size, (void **)& dst, (DWORD *)&len, NULL, 0, 0);
   if (FAILED(hr)) {
      buffer.buffer_->Unlock(dst, len, NULL, 0);
      buffer.buffer_->Release();
      al_set_error(AL_INVALID_OPERATION);
      return;
   }

   memcpy(dst, data, size);
   buffer.buffer_->Unlock(dst, len, NULL, 0);
}
