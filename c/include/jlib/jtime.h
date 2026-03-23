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
