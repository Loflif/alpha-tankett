// alpha_byte_stream.cc

#include "alpha.h"

namespace alpha {
   byte_stream::byte_stream(uint64 capacity, uint8 *base)
      : capacity_(capacity)
      , base_(base)
      , at_(base)
   {
   }

   void byte_stream::reset() {
      at_ = base_;
   }

   uint64 byte_stream::capacity() const {
      return capacity_;
   }

   uint64 byte_stream::length() const {
      return (at_ - base_);
   }
} // !alpha
