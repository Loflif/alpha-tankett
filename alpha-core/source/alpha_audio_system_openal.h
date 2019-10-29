// alpha_audio_system_openal.h

#ifndef ALPHA_AUDIO_SYSTEM_OPENAL_H_INCLUDED
#define ALPHA_AUDIO_SYSTEM_OPENAL_H_INCLUDED

#include "alpha.h"

#include "alpha_openal.h"

namespace alpha {
   constexpr uint32 PLAYING_SOURCE_COUNT = 64;

   struct audio_system_openal : audio_system {
      audio_system_openal();
      ~audio_system_openal();

      bool init();
      void shut();
      void make_current();

      virtual void process() final;
      virtual void play(const audio_buffer &sound, const float volume) final;
      virtual void play(const audio_source &source, const audio_buffer &sound, const float volume) final;
      virtual void pause(const audio_source &source) final;
      virtual void resume(const audio_source &source, const float volume) final;
      virtual void stop(const audio_source &source) final;
      virtual void set_volume(const audio_source &source, const float volume) final;
      virtual void stop_all() final;

      ALCdevice *device_;
      ALCcontext *context_;

      bool playing_[PLAYING_SOURCE_COUNT];
      ALuint sources_[PLAYING_SOURCE_COUNT];
   };
} // !alpha

#endif // !ALPHA_AUDIO_SYSTEM_OPENAL_H_INCLUDED
