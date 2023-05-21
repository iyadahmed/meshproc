#pragma once

#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include "macros.h"

STRUCT(Millis_Timer)
{
#if defined(_WIN32) || defined(_WIN64)
    LARGE_INTEGER start;
    LARGE_INTEGER frequency;
#else
    struct timespec start;
#endif
};

static void timer_start(Millis_Timer *timer)
{
#if defined(_WIN32) || defined(_WIN64)
    QueryPerformanceFrequency(&timer->frequency);
    QueryPerformanceCounter(&timer->start);
#else
    clock_gettime(CLOCK_MONOTONIC, &timer->start);
#endif
}

static unsigned long timer_stop(const Millis_Timer *timer)
{
    unsigned long elapsed;
#if defined(_WIN32) || defined(_WIN64)
    LARGE_INTEGER current;
    QueryPerformanceCounter(&current);
    elapsed = (unsigned long)((current.QuadPart - timer->start.QuadPart) * 1000 / timer->frequency.QuadPart);
#else
    struct timespec current;
    clock_gettime(CLOCK_MONOTONIC, &current);
    elapsed = (current.tv_sec - timer->start.tv_sec) * 1000;
    elapsed += (current.tv_nsec - timer->start.tv_nsec) / 1000000;
#endif
    return elapsed;
}
