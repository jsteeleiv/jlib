#pragma once
#ifndef JTIME_H
#define JTIME_H

#include <stdio.h>
#include <time.h>

typedef struct TimeStamp {
    struct tm raw;
    char tstr[1024];         // 1k; timestamp in formatted string form
    char diag[1024];         // 1k; diagnostic string for Jerror reporting
    char isordy[64];         // 64 bytes; proper iso8601 dev/log/API timestamp
    char dateusa[16];        // USA format: Month/Day/Year   (MM/DD/YYYY)
    char dateeur[16];        // EUR format: Day/Month/Year   (DD/MM/YYYY)
    const char *tzname;      // time-zone information(EST, EDT, UTC, etc.)
    time_t epoch;            // epoch time (raw)
    long msec, usec, nsec;   // [milli|micro|nano]seconds
    int sec, min, hour, mday, wday, yday, isdst, year, mon,
        gmtoff, yweek, doy;
    bool isweekend;
    bool isleapyear;
    bool isvalid;
} Jtime;

Jtime init_t(void) {
    Jtime t = {0};
    time_t now = time(NULL);
    struct tm *lt = localtime(&now);

    if (!lt) return t;
    t.raw = *lt;
    t.sec = lt->tm_sec;
    t.min = lt->tm_min;
    t.hour = lt->tm_hour;
    t.mday = lt->tm_mday;
    t.wday = lt->tm_wday;
    t.yday = lt->tm_yday;
    t.isdst = lt->tm_isdst;
    t.gmtoff = lt->tm_gmtoff;
    t.year = (lt->tm_year + 1900);
    t.mon = (lt->tm_mon + 1);
    size_t n = strftime(t.tstr, sizeof(t.tstr), "%Y%m%d_%H%M%S",lt);
    if (n == 0) t.tstr[0] = '\0';
    snprintf(t.diag, sizeof(t.diag), "strftime() wrote %zu characters at time of initialization", n);
    return t;
}

// place the highest value into a var of type time_t {(8bytes)-1}
time_t max_v = 0x7FFFFFFFl;
// pass it to ctime(){convert time} to convert it into an ascii str
//char *str = ctime(&max_v);

// print the str, 
//printf("max date = %s\n", str);
//look at the definition of time_t, (/usr/include/time.h)
// time_t == __int64_t == ll == long long int == 64 bits == 8 bytes
    /* code */

//? For how many years into the future does time_t last?

	/* amend */

// obtain current time by calling time()
//	char *now = asctime(time(*(&max_v)));
// call difftime() to get # of sec { now .. max } value of time_t
// format value into years, months, weeks, days, hours, mins
// print it
//	printf("now = %s\n", now);



#ifdef JTIME_IMPL

tm getnow(){
    time_t now = time(NULL);
    struct tm out;

#ifdef _WIN32
    localtime_s(&out, &now);
#else
    localtime_r(&now, &out);
#endif
    return out; // return by value (copy)
}

#endif /* JTIME_IMPL */


#endif /* JTIME_H */
