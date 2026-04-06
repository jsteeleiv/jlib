#pragma once
#ifndef JTIME_H
#define JTIME_H

#include <stdio.h>
#include <time.h>

// place the highest value into a var of type time_t {(8bytes)-1}
time_t max_v = 0x7FFFFFFFl;

typedef struct TimeStamp {
    time_t rawtime;
    struct tm tm_time;
    char time_str[1024];       // 1k; timestamp in formatted string form
    const char iso_rdy[64];    // 64 bytes; proper iso8601 dev/log/API timestamp
    const char dateusa[16];    // USA format: Month/Day/Year   (MM/DD/YYYY)
    const char dateeur[16];    // EUR format: Day/Month/Year   (DD/MM/YYYY)
    const char tm_zone[4];     // time-zone information(EST, EDT, UTC, etc.)
} jtime_t;

typedef struct TimeString {
    char diag[1024];           // 1k; diagnostic string for Jerror reporting
    long msec, usec, nsec;     // [milli|micro|nano]seconds
    int sec, min, hour, wday, mon, mday, 
        year, yday, yweek, doy, isdst, gmtoff;
    bool isweekend;
    bool isleapyear;
    bool isvalid;
} timestr_t;

typedef struct Timer {
    jtime_t curr;
    jtime_t start;
    jtime_t stop;
    bool reset;
} timer_t;

typedef struct Clock {
    Jansi ansi;
    jtime_t jtime;
    struct tm tm_time;
    jtime_t offset;
    timestr_t tstr;
    timer_t timer;
    size_t ticks;
    char *display;
    bool frozen;
    bool dirty;
    bool utc;
    bool dst;
} jclock_t;

typedef struct  {
    Jerror err;
    Jlog log;
    jclock_t clock;
    timestr_t tstr;
    jtime_t epoch;            // epoch time (raw)
} Jtime;

static inline time_t yup();

Jtime init_jtime(void);

static inline struct tm getnow();


#endif /* JTIME_H */
#ifdef JTIME_IMPL

static inline struct tm getnow(){
    time_t now = time(NULL);
    struct tm out;

#ifdef _WIN32
    localtime_s(&out, &now);
#else
    localtime_r(&now, &out);
#endif
    return out; // return by value (copy)
}

Jtime init_jtime(void){
    Jtime t = {0};
    struct tm now = getnow();
    struct tm *lt = localtime(&now);

    if (!lt) return t;
    t.clock.tm_time = *lt;
    t.tstr.sec = lt->tm_sec;
    t.tstr.min = lt->tm_min;
    t.tstr.hour = lt->tm_hour;
    t.tstr.mday = lt->tm_mday;
    t.tstr.wday = lt->tm_wday;
    t.tstr.yday = lt->tm_yday;
    t.tstr.isdst = lt->tm_isdst;
    t.tstr.gmtoff = lt->tm_gmtoff;
    t.tstr.year = (lt->tm_year + 1900);
    t.tstr.mon = (lt->tm_mon + 1);
    size_t n = strftime(t.tstr.diag, sizeof(t.tstr), "%Y%m%d_%H%M%S",lt);
    if (n == 0) t.tstr.diag[0] = '\0';
    snprintf(t.tstr.diag, sizeof(t.tstr.diag), "strftime() wrote %zu characters at time of initialization", n);
    return t;
}

#endif /* JTIME_IMPL */
