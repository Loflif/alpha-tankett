// alpha_byte_stream_reader.cc

#include "alpha.h"

namespace alpha {
   namespace {
      bool is_past_stream_end(const byte_stream &stream, const uint8 *cursor, const uint64 size) {
         uint64 length = (cursor - stream.base_);
         return length + size > stream.capacity();
      }

      template <typename T>
      bool read_from(byte_stream &stream, uint8 *&cursor, T &value) {
         if (!is_past_stream_end(stream, cursor, sizeof(T))) {
            value = *reinterpret_cast<T *>(cursor);
            cursor += sizeof(T);
            return true;
         }

         return false;
      }
   } // !anon

   byte_stream_reader::byte_stream_reader(byte_stream &stream)
      : stream_(stream)
      , cursor_(stream.base_)
   {
   }

   uint8 byte_stream_reader::peek() const {
      return *cursor_;
   }

   bool byte_stream_reader::eos() const {
      return (uint64)(cursor_ - stream_.base_) >= stream_.capacity();
   }

   bool byte_stream_reader::serialize(float &value) {
      union
      {
         uint32 u;
         float  f;
      } u2f;

      if (!serialize(u2f.u)) {
         return false;
      }

      value = u2f.f;

      return true;
   }

   bool byte_stream_reader::serialize(uint64 &value) {
      return read_from<uint64>(stream_, cursor_, value);
   }

   bool byte_stream_reader::serialize(uint32 &value) {
      return read_from<uint32>(stream_, cursor_, value);
   }

   bool byte_stream_reader::serialize(uint16 &value) {
      return read_from<uint16>(stream_, cursor_, value);
   }

   bool byte_stream_reader::serialize(int16 &value) {
      return read_from<int16>(stream_, cursor_, value);
   }

   bool byte_stream_reader::serialize(uint8 &value) {
      return read_from<uint8>(stream_, cursor_, value);
   }

   bool byte_stream_reader::serialize(const uint32 count, uint8 *values) {
      const uint32 size = count * sizeof(uint8);

      if (!is_past_stream_end(stream_, cursor_, size)) {
         for (uint32 index = 0; index < count; index++) {
            values[index] = cursor_[index];
         }

         cursor_ += size;

         return true;
      }

      return false;
   }
} // !alpha
