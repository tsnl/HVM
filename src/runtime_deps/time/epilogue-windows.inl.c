// see: https://stackoverflow.com/questions/10905892/equivalent-of-gettimeday-for-windows
// see: https://web.archive.org/web/20100111030931/http://www.cpp-programming.net/c-tidbits/gettimeofday-function-for-windows/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// MSVC defines this in winsock2.h!?
typedef struct timeval {
  long tv_sec;
  long tv_usec;
} timeval;

// typedef struct timezone {
//   int z_minuteswest;  // minutes W of Greenwich
//   int z_dsttime;      // type of dst correction
// } timezone;

inline static void time_now(timeval* tp);

inline static void time_now(timeval* tp) {
  // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
  // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
  // until 00:00:00 January 1, 1970 
  #if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
    #define EPOCH  11644473600000000Ui64
  #else
    #define EPOCH  11644473600000000ULL
  #endif


  SYSTEMTIME system_time;
  FILETIME file_time;
  u64 time;

  if (tp) {
    GetSystemTime(&system_time);
    if (!SystemTimeToFileTime(&system_time, &file_time)) {
      __time_check(GetLastError(), "time_now: SystemTimeToFileTime failed");
    }
    time =  ((u64)file_time.dwLowDateTime);
    time += ((u64)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
  }

  #undef EPOCH
}