// alpha_network_error.cc

#include "alpha.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

namespace alpha {
   // static 
   network_error network_error::get_error() {
      network_error result;
      int error_code = WSAGetLastError();
      switch (error_code) {
         case 0:
            result.code_ = NETWORK_ERROR_NO_ERROR;
            break;
         case WSA_INVALID_HANDLE:
            result.code_ = NETWORK_ERROR_INVALID_HANDLE;
            break;
         case WSA_NOT_ENOUGH_MEMORY:
            result.code_ = NETWORK_ERROR_NOT_ENOUGH_MEMORY;
            break;
         case WSA_INVALID_PARAMETER:
            result.code_ = NETWORK_ERROR_INVALID_PARAMETER;
            break;
         case WSA_OPERATION_ABORTED:
            result.code_ = NETWORK_ERROR_OPERATION_ABORTED;
            break;
         case WSAEINTR:
            result.code_ = NETWORK_ERROR_INTERUPTED_CALL;
            break;
         case WSAEBADF:
            result.code_ = NETWORK_ERROR_BAD_FILE_HANDLE;
            break;
         case WSAEACCES:
            result.code_ = NETWORK_ERROR_SOCKET_ACCESS_DENIED;
            break;
         case WSAEFAULT:
            result.code_ = NETWORK_ERROR_BAD_ADDRESS;
            break;
         case WSAEINVAL:
            result.code_ = NETWORK_ERROR_INVALID_ARGUMENT;
            break;
         case WSAEMFILE:
            result.code_ = NETWORK_ERROR_TOO_MANY_OPEN_FILES;
            break;
         case WSAEWOULDBLOCK:
            result.code_ = NETWORK_ERROR_WOULD_BLOCK;
            break;
         case WSAEINPROGRESS:
            result.code_ = NETWORK_ERROR_IN_PROGRESS;
            break;
         case WSAEALREADY:
            result.code_ = NETWORK_ERROR_ALREADY_IN_PROGRESS;
            break;
         case WSAENOTSOCK:
            result.code_ = NETWORK_ERROR_HANDLE_NON_SOCKET;
            break;
         case WSAEDESTADDRREQ:
            result.code_ = NETWORK_ERROR_DESTINATION_ADDRESS_REQUIRED;
            break;
         case WSAEMSGSIZE:
            result.code_ = NETWORK_ERROR_MESSAGE_TOO_LONG;
            break;
         case WSAEPROTOTYPE:
            result.code_ = NETWORK_ERROR_WRONG_PROTOTYPE;
            break;
         case WSAENOPROTOOPT:
            result.code_ = NETWORK_ERROR_BAD_PROTOCOL_OPTION;
            break;
         case WSAEPROTONOSUPPORT:
            result.code_ = NETWORK_ERROR_PROTOCOL_NOT_SUPPORTED;
            break;
         case WSAESOCKTNOSUPPORT:
            result.code_ = NETWORK_ERROR_SOCKET_TYPE_NOT_SUPPORTED;
            break;
         case WSAEOPNOTSUPP:
            result.code_ = NETWORK_ERROR_OPERATION_NOT_SUPPORTED;
            break;
         case WSAEPFNOSUPPORT:
            result.code_ = NETWORK_ERROR_PROTOCOL_FAMILY_NOT_SUPPORT;
            break;
         case WSAEAFNOSUPPORT:
            result.code_ = NETWORK_ERROR_ADDRESS_FAMILY_NOT_SUPPORT;
            break;
         case WSAEADDRINUSE:
            result.code_ = NETWORK_ERROR_ADDRESS_IN_USE;
            break;
         case WSAEADDRNOTAVAIL:
            result.code_ = NETWORK_ERROR_ADDRESS_NOT_AVAILABLE;
            break;
         case WSAENETDOWN:
            result.code_ = NETWORK_ERROR_NETWORK_DOWN;
            break;
         case WSAENETUNREACH:
            result.code_ = NETWORK_ERROR_NETWORK_UNREACHABLE;
            break;
         case WSAENETRESET:
            result.code_ = NETWORK_ERROR_NETWORK_DROPPED_CONNECTION;
            break;
         case WSAECONNABORTED:
            result.code_ = NETWORK_ERROR_CONNECTION_RESET_BY_SOFTWARE;
            break;
         case WSAECONNRESET:
            result.code_ = NETWORK_ERROR_CONNECTION_RESET_BY_PEER;
            break;
         case WSAENOBUFS:
            result.code_ = NETWORK_ERROR_NO_BUFFER_SPACE_AVAIABLE;
            break;
         case WSAEISCONN:
            result.code_ = NETWORK_ERROR_ALREADY_CONNECTED;
            break;
         case WSAENOTCONN:
            result.code_ = NETWORK_ERROR_NOT_CONNECTED;
            break;
         case WSAESHUTDOWN:
            result.code_ = NETWORK_ERROR_SEND_SHUTDOWN;
            break;
         case WSAETOOMANYREFS:
            result.code_ = NETWORK_ERROR_TOO_MANY_REFS;
            break;
         case WSAETIMEDOUT:
            result.code_ = NETWORK_ERROR_CONNECTION_TIMED_OUT;
            break;
         case WSAECONNREFUSED:
            result.code_ = NETWORK_ERROR_CONNECTION_REFUSED;
            break;
         case WSAELOOP:
            result.code_ = NETWORK_ERROR_TRANSLATE_NAME;
            break;
         case WSAENAMETOOLONG:
            result.code_ = NETWORK_ERROR_NAME_TOO_LONG;
            break;
         case WSAEHOSTDOWN:
            result.code_ = NETWORK_ERROR_HOST_DOWN;
            break;
         case WSAEHOSTUNREACH:
            result.code_ = NETWORK_ERROR_HOST_UNREACHABLE;
            break;
         case WSAEPROCLIM:
            result.code_ = NETWORK_ERROR_PROCESS_LIMIT;
            break;
         case WSASYSNOTREADY:
            result.code_ = NETWORK_ERROR_NETWORK_SYSTEM_NOT_READY;
            break;
         case WSAVERNOTSUPPORTED:
            result.code_ = NETWORK_ERROR_VERSION_NOT_SUPPORTED;
            break;
         case WSANOTINITIALISED:
            result.code_ = NETWORK_ERROR_NETWORK_NOT_INITIALISED;
            break;
         case WSAEDISCON:
            result.code_ = NETWORK_ERROR_DISCONNECT_IN_PROGRESS;
            break;
         case WSAEINVALIDPROCTABLE:
            result.code_ = NETWORK_ERROR_INVALIDPROCTABLE;
            break;
         case WSAEINVALIDPROVIDER:
            result.code_ = NETWORK_ERROR_INVALIDPROVIDER;
            break;
         case WSAEPROVIDERFAILEDINIT:
            result.code_ = NETWORK_ERROR_PROVIDERFAILEDINIT;
            break;
         case WSASYSCALLFAILURE:
            result.code_ = NETWORK_ERROR_SYSCALLFAILURE;
            break;
         case WSAHOST_NOT_FOUND:
            result.code_ = NETWORK_ERROR_HOST_NOT_FOUND;
            break;
         case WSATRY_AGAIN:
            result.code_ = NETWORK_ERROR_TRY_AGAIN;
            break;
         case WSANO_RECOVERY:
            result.code_ = NETWORK_ERROR_NO_RECOVERY;
            break;
         case WSANO_DATA:
            result.code_ = NETWORK_ERROR_NO_DATA;
            break;
         default:
            result.code_ = NETWORK_ERROR_UNKNOWN;
            break;
      }

      return result;
   }

   network_error::network_error()
      : code_(NETWORK_ERROR_NO_ERROR)
   {
   }

   bool network_error::is_critical() const {
      return false;
   }

   bool network_error::is_non_critical() const {
      return !is_critical();
   }

   const char *network_error::as_string() const {
      switch (code_) {
         case NETWORK_ERROR_NO_ERROR:
            return "NETWORK_ERROR_NO_ERROR";
         case NETWORK_ERROR_INVALID_HANDLE:
            return "NETWORK_ERROR_INVALID_HANDLE";
         case NETWORK_ERROR_NOT_ENOUGH_MEMORY:
            return "NETWORK_ERROR_NOT_ENOUGH_MEMORY";
         case NETWORK_ERROR_INVALID_PARAMETER:
            return "NETWORK_ERROR_INVALID_PARAMETER";
         case NETWORK_ERROR_OPERATION_ABORTED:
            return "NETWORK_ERROR_OPERATION_ABORTED";
         case NETWORK_ERROR_INTERUPTED_CALL:
            return "NETWORK_ERROR_INTERUPTED_CALL";
         case NETWORK_ERROR_BAD_FILE_HANDLE:
            return "NETWORK_ERROR_BAD_FILE_HANDLE,";
         case NETWORK_ERROR_SOCKET_ACCESS_DENIED:
            return "NETWORK_ERROR_SOCKET_ACCESS_DENIED";
         case NETWORK_ERROR_BAD_ADDRESS:
            return "NETWORK_ERROR_BAD_ADDRESS";
         case NETWORK_ERROR_INVALID_ARGUMENT:
            return "NETWORK_ERROR_INVALID_ARGUMENT";
         case NETWORK_ERROR_TOO_MANY_OPEN_FILES:
            return "NETWORK_ERROR_TOO_MANY_OPEN_FILES";
         case NETWORK_ERROR_WOULD_BLOCK:
            return "NETWORK_ERROR_WOULD_BLOCK";
         case NETWORK_ERROR_IN_PROGRESS:
            return "NETWORK_ERROR_IN_PROGRESS";
         case NETWORK_ERROR_ALREADY_IN_PROGRESS:
            return "NETWORK_ERROR_ALREADY_IN_PROGRESS";
         case NETWORK_ERROR_HANDLE_NON_SOCKET:
            return "NETWORK_ERROR_HANDLE_NON_SOCKET";
         case NETWORK_ERROR_DESTINATION_ADDRESS_REQUIRED:
            return "NETWORK_ERROR_DESTINATION_ADDRESS_REQUIRED";
         case NETWORK_ERROR_MESSAGE_TOO_LONG:
            return "NETWORK_ERROR_MESSAGE_TOO_LONG";
         case NETWORK_ERROR_WRONG_PROTOTYPE:
            return "NETWORK_ERROR_WRONG_PROTOTYPE";
         case NETWORK_ERROR_BAD_PROTOCOL_OPTION:
            return "NETWORK_ERROR_BAD_PROTOCOL_OPTION";
         case NETWORK_ERROR_PROTOCOL_NOT_SUPPORTED:
            return "NETWORK_ERROR_PROTOCOL_NOT_SUPPORTED";
         case NETWORK_ERROR_SOCKET_TYPE_NOT_SUPPORTED:
            return "NETWORK_ERROR_SOCKET_TYPE_NOT_SUPPORTED";
         case NETWORK_ERROR_OPERATION_NOT_SUPPORTED:
            return "NETWORK_ERROR_OPERATION_NOT_SUPPORTED";
         case NETWORK_ERROR_PROTOCOL_FAMILY_NOT_SUPPORT:
            return "NETWORK_ERROR_PROTOCOL_FAMILY_NOT_SUPPORT";
         case NETWORK_ERROR_ADDRESS_FAMILY_NOT_SUPPORT:
            return "NETWORK_ERROR_ADDRESS_FAMILY_NOT_SUPPORT";
         case NETWORK_ERROR_ADDRESS_IN_USE:
            return "NETWORK_ERROR_ADDRESS_IN_USE";
         case NETWORK_ERROR_ADDRESS_NOT_AVAILABLE:
            return "NETWORK_ERROR_ADDRESS_NOT_AVAILABLE";
         case NETWORK_ERROR_NETWORK_DOWN:
            return "NETWORK_ERROR_NETWORK_DOWN";
         case NETWORK_ERROR_NETWORK_UNREACHABLE:
            return "NETWORK_ERROR_NETWORK_UNREACHABLE";
         case NETWORK_ERROR_NETWORK_DROPPED_CONNECTION:
            return "NETWORK_ERROR_NETWORK_DROPPED_CONNECTION";
         case NETWORK_ERROR_CONNECTION_RESET_BY_SOFTWARE:
            return "NETWORK_ERROR_CONNECTION_RESET_BY_SOFTWARE";
         case NETWORK_ERROR_CONNECTION_RESET_BY_PEER:
            return "NETWORK_ERROR_CONNECTION_RESET_BY_PEER";
         case NETWORK_ERROR_NO_BUFFER_SPACE_AVAIABLE:
            return "NETWORK_ERROR_NO_BUFFER_SPACE_AVAIABLE";
         case NETWORK_ERROR_ALREADY_CONNECTED:
            return "NETWORK_ERROR_ALREADY_CONNECTED";
         case NETWORK_ERROR_NOT_CONNECTED:
            return "NETWORK_ERROR_NOT_CONNECTED";
         case NETWORK_ERROR_SEND_SHUTDOWN:
            return "NETWORK_ERROR_SEND_SHUTDOWN";
         case NETWORK_ERROR_TOO_MANY_REFS:
            return "NETWORK_ERROR_TOO_MANY_REFS";
         case NETWORK_ERROR_CONNECTION_TIMED_OUT:
            return "NETWORK_ERROR_CONNECTION_TIMED_OUT";
         case NETWORK_ERROR_CONNECTION_REFUSED:
            return "NETWORK_ERROR_CONNECTION_REFUSED";
         case NETWORK_ERROR_TRANSLATE_NAME:
            return "NETWORK_ERROR_TRANSLATE_NAME";
         case NETWORK_ERROR_NAME_TOO_LONG:
            return "NETWORK_ERROR_NAME_TOO_LONG";
         case NETWORK_ERROR_HOST_DOWN:
            return "NETWORK_ERROR_HOST_DOWN";
         case NETWORK_ERROR_HOST_UNREACHABLE:
            return "NETWORK_ERROR_HOST_UNREACHABLE";
         case NETWORK_ERROR_PROCESS_LIMIT:
            return "NETWORK_ERROR_PROCESS_LIMIT";
         case NETWORK_ERROR_NETWORK_SYSTEM_NOT_READY:
            return "NETWORK_ERROR_NETWORK_SYSTEM_NOT_READY";
         case NETWORK_ERROR_VERSION_NOT_SUPPORTED:
            return "NETWORK_ERROR_VERSION_NOT_SUPPORTED";
         case NETWORK_ERROR_NETWORK_NOT_INITIALISED:
            return "NETWORK_ERROR_NETWORK_NOT_INITIALISED";
         case NETWORK_ERROR_DISCONNECT_IN_PROGRESS:
            return "NETWORK_ERROR_DISCONNECT_IN_PROGRESS";
         case NETWORK_ERROR_INVALIDPROCTABLE:
            return "NETWORK_ERROR_INVALIDPROCTABLE";
         case NETWORK_ERROR_INVALIDPROVIDER:
            return "NETWORK_ERROR_INVALIDPROVIDER";
         case NETWORK_ERROR_PROVIDERFAILEDINIT:
            return "NETWORK_ERROR_PROVIDERFAILEDINIT";
         case NETWORK_ERROR_SYSCALLFAILURE:
            return "NETWORK_ERROR_SYSCALLFAILURE";
         case NETWORK_ERROR_HOST_NOT_FOUND:
            return "NETWORK_ERROR_HOST_NOT_FOUND";
         case NETWORK_ERROR_TRY_AGAIN:
            return "NETWORK_ERROR_TRY_AGAIN";
         case NETWORK_ERROR_NO_RECOVERY:
            return "NETWORK_ERROR_NO_RECOVERY";
         case NETWORK_ERROR_NO_DATA:
            return "NETWORK_ERROR_NO_DATA";
         case NETWORK_ERROR_REMOTE_DISCONNECT:
            return "NETWORK_ERROR_REMOTE_DISCONNECT";
         case NETWORK_ERROR_SELECT_TIMEOUT:
            return "NETWORK_ERROR_SELECT_TIMEOUT";
      }

      return "NETWORK_ERROR_UNKNOWN";
   }
} // !alpha
