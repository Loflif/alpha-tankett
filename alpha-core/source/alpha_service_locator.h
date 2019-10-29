// alpha_service_locator.h

#ifndef ALPHA_SERVICE_LOCATOR_H_INCLUDED
#define ALPHA_SERVICE_LOCATOR_H_INCLUDED

#include <cassert>

namespace alpha {
   template <typename T>
   struct service_locator {
      static void set(T *service) {
         service_ = service;
      }

      static T &get() {
         assert(service_);
         return *service_;
      }

   private:
      static T *service_;
   };

   template <typename T>
   T *service_locator<T>::service_ = nullptr;
} // !alpha

#endif // !ALPHA_SERVICE_LOCATOR_H_INCLUDED
