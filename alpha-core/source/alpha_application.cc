// alpha_application.cc

#include "alpha.h"
#include "alpha_service_locator.h"

namespace alpha {
   application::application()
      : render_system_(service_locator<render_system>::get())
      , audio_system_(service_locator<audio_system>::get())
   {
   }

   bool application::init() {
      if (!enter()) {
         return false;
      }

      return true;
   }

   void application::shut() {
      exit();
   }
   
   bool application::frame() {
      if (!tick()) {
         return false;
      }

      return true;
   }
} // !alpha
