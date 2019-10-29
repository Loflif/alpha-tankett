// alpha_byte_stream_writer.cc

#include "alpha.h"

namespace alpha {
   namespace {
      bool is_past_stream_end(const byte_stream &stream, const uint8 *cursor, const uint64 size) {
         uint64 length = (cursor - stream.base_);
         return length + size > stream.capacity();
      }

      template <typename T>
      bool write_to(byte_stream &stream, uint8 *&cursor, T value) {
         if (!is_past_stream_end(stream, cursor, sizeof(T))) {
            *reinterpret_cast<T *>(cursor) = value;
            cursor += sizeof(T);
            return true;
         }

         return false;
      }
   } // !anon

   byte_stream_writer::byte_stream_writer(byte_stream &stream)
      : stream_(stream)
   {
   }

   bool byte_stream_writer::serialize(float value) {
      union {
         float  f;
         uint32 u;
      } f2u;
      f2u.f = value;
      return serialize(f2u.u);
   }

   bool byte_stream_writer::serialize(uint64 value) {
      return write_to<uint64 >(stream_, stream_.at_, value);
   }

   bool byte_stream_writer::serialize(uint32 value) {
      return write_to<uint32>(stream_, stream_.at_, value);
   }

   bool byte_stream_writer::serialize(uint16 value) {
      return write_to<uint16>(stream_, stream_.at_, value);
   }

   bool byte_stream_writer::serialize(int16 value) {
      return write_to<int16>(stream_, stream_.at_, value);
   }

   bool byte_stream_writer::serialize(uint8 value) {
      return write_to<uint8>(stream_, stream_.at_, value);
   }

   bool byte_stream_writer::serialize(uint32 count, const uint8 *values) {
      const uint32 size = count * sizeof(uint8);

      if (!is_past_stream_end(stream_, stream_.at_, size)) {
         for (uint32 index = 0; index < count; index++) {
            stream_.at_[index] = values[index];
         }

         stream_.at_ += size;

         return true;
      }

      return false;
   }
} // !alpha
