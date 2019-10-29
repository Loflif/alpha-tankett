// alpha_audio_system_openal.cc

#include "alpha.h"
#include "alpha_audio_system_openal.h"

namespace alpha {
   audio_system_openal::audio_system_openal()
      : device_(nullptr)
      , context_(nullptr)
      , playing_{}
      , sources_{}
   {
   }

   audio_system_openal::~audio_system_openal()
   {
   }

   bool audio_system_openal::init() {
      device_ = alcOpenDevice(NULL);
      if (!device_) {
         ALenum err = alGetError();
         return false;
      }

      context_ = alcCreateContext(device_, NULL);
      if (!context_) {
         ALenum err = alGetError();
         alcCloseDevice(device_);
         return false;
      }

      if (!alcMakeContextCurrent(context_)) {
         ALenum err = alGetError();
         alcDestroyContext(context_);
         alcCloseDevice(device_);
         return false;
      }

      alGenSources(PLAYING_SOURCE_COUNT, sources_);

      ALenum err = alGetError();
      if (err != AL_NO_ERROR) {
         alcDestroyContext(context_);
         alcCloseDevice(device_);
         return false;
      }

      return true;
   }

   void audio_system_openal::shut() {
      alDeleteSources(PLAYING_SOURCE_COUNT, sources_);

      if (context_) {
         alcMakeContextCurrent(NULL);
         alcDestroyContext(context_);
         context_ = nullptr;
      }
      if (device_) {
         alcCloseDevice(device_);
         device_ = nullptr;
      }
   }

   void audio_system_openal::make_current() {
      alcMakeContextCurrent(context_);
   }

   void audio_system_openal::process() {
      for (int index = 0; index < PLAYING_SOURCE_COUNT; index++) {
         if (playing_[index]) {
            ALint status = 0;
            alGetSourcei(sources_[index], AL_SOURCE_STATE, &status);

            if (status != AL_PLAYING) {
               playing_[index] = false;
            }
         }
      }
   }

   void audio_system_openal::play(const audio_buffer &sound, const float volume) {
      if (!sound.is_valid()) {
         return;
      }

      for (int index = 0; index < PLAYING_SOURCE_COUNT; index++) {
         if (!playing_[index]) {
            playing_[index] = true;
            alSourcei(sources_[index], AL_BUFFER, sound.handle_);
            alSourcef(sources_[index], AL_GAIN, volume);
            alSourcePlay(sources_[index]);
            break;
         }
      }
   }

   void audio_system_openal::play(const audio_source &source, const audio_buffer &sound, const float volume) {
      if (!source.is_valid() && !sound.is_valid()) {
         return;
      }

      ALint status = 0;
      alGetSourcei(source.handle_, AL_SOURCE_STATE, &status);
      if (status == AL_PLAYING) {
         alSourceStop(source.handle_);
      }

      alSourcei(source.handle_, AL_BUFFER, sound.handle_);
      alSourcef(source.handle_, AL_GAIN, volume);
      alSourcePlay(source.handle_);
   }

   void audio_system_openal::pause(const audio_source &source) {
      if (!source.is_valid()) {
         return;
      }

      ALint status = 0;
      alGetSourcei(source.handle_, AL_SOURCE_STATE, &status);
      if (status == AL_PLAYING) {
         alSourceStop(source.handle_);
      }
   }

   void audio_system_openal::resume(const audio_source &source, const float volume) {
      if (!source.is_valid()) {
         return;
      }

      ALint status = 0;
      alGetSourcei(source.handle_, AL_SOURCE_STATE, &status);
      if (status != AL_PLAYING) {
         alSourcef(source.handle_, AL_GAIN, volume);
         alSourcePlay(source.handle_);
      }
   }

   void audio_system_openal::stop(const audio_source &source) {
      if (!source.is_valid()) {
         return;
      }

      ALint status = 0;
      alGetSourcei(source.handle_, AL_SOURCE_STATE, &status);
      if (status == AL_PLAYING) {
         alSourceStop(source.handle_);
      }
   }

   void audio_system_openal::set_volume(const audio_source &source, const float volume) {
      if (!source.is_valid()) {
         return;
      }

      ALint status = 0;
      alGetSourcei(source.handle_, AL_SOURCE_STATE, &status);
      if (status == AL_PLAYING) {
         alSourcef(source.handle_, AL_GAIN, volume);
      }
   }

   void audio_system_openal::stop_all() {

   }
} // !alpha
