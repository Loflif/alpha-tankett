// alpha_byte_stream_evaluator.cc

#include "alpha.h"

namespace alpha {
   namespace {
	   template <typename T>
	   bool calculate(const uint64 capacity, uint64& size, uint64 count = 1) {
		   if ((size + sizeof(T) * count) > capacity) {
			   return false;
		   }
		   size += sizeof(T) * count;
		   return true;
	   }
   } // !anon

   byte_stream_evaluator::byte_stream_evaluator(byte_stream &stream)
      : capacity_(stream.capacity())
      , size_(0)
   {
   }

   uint64 byte_stream_evaluator::size() const {
      return size_;
   }

   bool byte_stream_evaluator::serialize(float value) {
      return calculate<float>(capacity_, size_);
   }

   bool byte_stream_evaluator::serialize(uint64 value) {
      return calculate<uint64>(capacity_, size_);
   }

   bool byte_stream_evaluator::serialize(uint32 value) {
      return calculate<uint32>(capacity_, size_);
   }

   bool byte_stream_evaluator::serialize(uint16 value) {
      return calculate<uint16>(capacity_, size_);
   }

   bool byte_stream_evaluator::serialize(int16 value) {
      return calculate<int16>(capacity_, size_);
   }

   bool byte_stream_evaluator::serialize(uint8 value) {
      return calculate<uint8>(capacity_, size_);
   }

   bool byte_stream_evaluator::serialize(const uint32 count, const uint8 *values) {
      return calculate<uint8>(capacity_, size_, count);
   }
} // !alpha
