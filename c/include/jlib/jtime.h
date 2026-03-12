#ifndef JTIME_H
#define JTIME_H

// getnow() + timestamp types
#include <time.h>

typedef struct Timestamp {
    // char buf[20];
    // strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &now_struct);
    short year;
    signed char mon;
    signed char week;
    signed char day;
    signed char hour;
    signed char min;
    signed char sec;
} time_st;


struct tm getnow(void);

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
