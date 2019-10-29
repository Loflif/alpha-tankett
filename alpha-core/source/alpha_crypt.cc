// alpha_crypt.cc

#include "alpha.h"

#include <time.h>

namespace alpha {
   namespace crypt {
      namespace generator {
         uint64 random_key() {
            bool init = false;
            if (!init) {
               init = true;
			   time_t t;
			   ::time(&t);
               srand((unsigned int)t);
            }

            uint64 result = 0;
            result |= ((uint64)rand() << 48) & 0xffff000000000000;
            result |= ((uint64)rand() << 32) & 0x0000ffff00000000;
            result |= ((uint64)rand() << 16) & 0x00000000ffff0000;
            result |= ((uint64)rand() << 0)  & 0x000000000000ffff;
            return result;
         }
      } // !generator

      xorinator::xorinator()
         : state_{}
      {
      }

      xorinator::xorinator(uint64 a, uint64 b)
         : state_{ a, b }
      {
      }

      bool xorinator::is_valid() const {
         return state_.a_ != 0 && state_.b_ != 0;
      }

      void xorinator::encrypt(const uint64 length, uint8 *data) {
         for (uint64 index = 0; index < length; index++) {
            uint64 element = index & (sizeof(key_) - 1);
            data[index] = data[index] ^ key_[element];
         }
      }

      void xorinator::decrypt(const uint64 length, uint8 *data) {
         encrypt(length, data);
      }
   } // !crypt
} // !alpha
