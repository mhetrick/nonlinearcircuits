/*
    TimeInSeconds.hpp  -  Don Cross <cosinekitty@gmail.com>  -  2023-09-25

    A handy helper function for measuring time to at least microsecond resolution
    on Linux, Windows, and Mac OS.
*/
#pragma once

#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
#include <sys/time.h>
#elif defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

inline double TimeInSeconds()
{
    double sec;         // Seconds since midnight January 1, 1970.

#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    sec = (double)tv.tv_sec + tv.tv_usec/1.0e+6;
#elif defined(_WIN32)
    FILETIME ft;
    ULARGE_INTEGER large;
    // Get time in 100-nanosecond units from January 1, 1601.
    GetSystemTimePreciseAsFileTime(&ft);
    large.u.LowPart  = ft.dwLowDateTime;
    large.u.HighPart = ft.dwHighDateTime;
    sec = (large.QuadPart - 116444736000000000ULL) / 1.0e+7;
#else
    #error Microsecond time resolution is not supported on this platform.
#endif

    return sec;
}
