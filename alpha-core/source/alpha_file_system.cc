// alpha_file_system.cc

#include "alpha.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace alpha {
   // static 
   bool file_system::exists(const string &filename) {
      DWORD attrib = GetFileAttributesA(filename.c_str());
      return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
   }

   bool file_system::load(const string &filename, dynamic_array<uint8> &content) {
      HANDLE handle = CreateFileA(filename.c_str(),
                                  GENERIC_READ,
                                  FILE_SHARE_READ,
                                  NULL,
                                  OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
      if (handle == INVALID_HANDLE_VALUE) {
         return false;
      }

      LARGE_INTEGER fs = { 0 };
      if (!GetFileSizeEx(handle, &fs)) {
         CloseHandle(handle);
         return false;
      }

      content.resize(fs.QuadPart);

      if (!ReadFile(handle, content.data(), fs.LowPart, NULL, NULL)) {
         CloseHandle(handle);
         return false;
      }

      CloseHandle(handle);

      return true;
   }

   file_system::file_system()
   {
   }
} // !alpha
