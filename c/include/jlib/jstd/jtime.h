#pragma once
#ifndef JTIME_H
#define JTIME_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "../jdata/jmap.h"
#include "../jdata/jlist.h"

#define TFMT_ISO      "%Y-%m-%d %H:%M:%S"
#define TFMT_FILENAME "%Y%m%d_%H%M%S"
#define TFMT_US_DATE  "%m/%d/%Y"
#define TFMT_EU_DATE  "%d/%m/%Y"
#define TFMT_CLOCK24  "%H:%M:%S"
#define TFMT_CLOCK12  "%I:%M:%S %p"

typedef enum TimeFormatKind {
    TFMT_NONE = 0,
    TFMT_YEAR,      /* %Y */
    TFMT_year,      /* %y */
    TFMT_MONNUM,   /* %m */
    TFMT_DAYNUM,   /* %d */
    TFMT_HR24,     /* %H */
    TFMT_HR12,     /* %I */
    TFMT_MIN,       /* %M */
    TFMT_SEC,       /* %S */
    TFMT_AMPM,      /* %p */
    TFMT_PRCNT      /* %% */
} tfmtkind_t;

typedef enum TimeFormatFlags {
    TFMT_DEFAULT   = 0,
    TFMT_NUMERIC   = 1u << 0, /* renders digits */
    TFMT_TEXT      = 1u << 1, /* renders text ("AM"/"PM") */
    TFMT_PADZERO   = 1u << 2, /* zero padding expected */ 
    TFMT_FIX_WIDTH = 1u << 3, /* width matters and is clamped */
    TFMT_LITERAL   = 1u << 4  /* special literals (ex. '%%' || '\n') */
} tfmtflags_t;

typedef struct TimeFormatSpec {
    const char *token;  /* "%Y" */
    const char *name;   /* "year_full" */
    tfmtkind_t kind;    /* TFMT_YEARFULL */
    unsigned int flags; /* timefmt_flags_t bitmask */
    unsigned int width; /* 4 for %Y, 2 for %m, etc. */
} tfmtspec_t;

typedef enum TimeFormatTokenType {
    TFMTTOK_NONE = 0,
    TFMTTOK_DIRECTIVE,
    TFMTTOK_LITERAL
} tfmttoktype_t;

typedef struct TimeFormatToken {
    tfmttoktype_t type;
    const tfmtspec_t *spec; /* borrowed, used if DIRECTIVE */
    char *text; /* borrowed, used if LITERAL */
    size_t len; /* token/literal length */
} tfmttoken_t;

typedef enum TimeFormatPresetFlags {
    TFMT_PRESET_NONE     = 0,
    TFMT_PRESET_BUILTIN  = 1u << 0,
    TFMT_PRESET_DATEONLY = 1u << 1,
    TFMT_PRESET_TIMEONLY = 1u << 2,
    TFMT_PRESET_DATETIME = 1u << 3,
    TFMT_PRESET_FILESAFE = 1u << 4
} tfmtpsetflags_t;

typedef struct TimeFormatPreset {
    const char  *name;    /* "filename", "iso", "clock24" */
    const char  *pattern; /* "%Y%m%d_%H%M%S" */
    unsigned int flags;   /* timefmt_preset_flags_t bitmask */
} tfmtpset_t;

typedef struct TimeFormat {
    tfmtflags_t flags;
    tfmtkind_t kind;
    tfmtspec_t spec;
    tfmttoken_t token;
    tfmtpset_t preset;
    /*
        timefmt_pattern_t
        timefmt_parser_t
    */
} timefmt_t;

/*
    char isofmt[64];  // 64 bytes; proper iso8601 dev/log/API timestamp
    char dateusa[32]; // USA format: Month/Day/Year   (MM/DD/YYYY)
    char dateeur[32]; // EUR format: Day/Month/Year   (DD/MM/YYYY)
    char tm_zone[8];  // time-zone information(EST, EDT, UTC, LOCAL, etc.)
*/

typedef enum TimeStringFlags {
    TSTR_NONE        = 0,
    TSTR_PARSED      = 1u << 0, /* token list matches current format */
    TSTR_DIRTY       = 1u << 1, /* display should be "cleaned"; regenerated */
    TSTR_OWNSFORMAT  = 1u << 2, /* free format when cleaning up */
    TSTR_OWNSDISPLAY = 1u << 3  /* free display when cleaning up */
} tstrflags_t;

typedef struct TimeString {
    const char *preset; /* selected preset name */
    const char *format; /* active raw format string */
    char *display;      /* rendered output */
    map_t directives;   /* "%Y" -> timefmt_spec_t * */
    map_t presets;      /* "filename" -> timefmt_preset_t * */
    list_t tokens;      /* timefmt_token_t * or token records */
    unsigned int flags;
} timestr_t;

static inline timestr_t timestr_init(void);

typedef enum TimeZone {
    JTIME_LCL = 0,
    JTIME_UTC
} timezone_t;

typedef struct TimeStamp {
    time_t rawtime;
    struct tm jtm;
    timezone_t tzone;
    long msec, usec, nsec;     // [milli|micro|nano]seconds
    int sec, min, hour, wday, mon, mday, year, yday, yweek, doy, isdst, gmtoff;
    bool weekend, leapyear, valid, utc;
} tmstamp_t;

static inline time_t getnow();
static inline struct tm gettime(time_t t);
static inline bool getlcl(time_t raw, struct tm *dest);
static inline bool getutc(time_t raw, struct tm *dest);
static inline void fillstrings(tmstamp_t *ts);

static inline bool isleapyear(int year);
static inline int weekofyear(const struct tm *tmt);
static inline long gmtoffset(const struct tm *tmt, time_t raw);

static inline bool tmstamp_filltm(time_t raw, bool utc, struct tm *dest);
static inline void tmstamp_print(const tmstamp_t *ts, FILE *stream);
static inline int tmstamp_from_time(tmstamp_t *ts, time_t raw, bool utc);
static inline void tmstamp_fillfields(tmstamp_t *ts);
static inline int tmstamp_refresh(tmstamp_t *ts);
static inline int tmstamp_now(tmstamp_t *ts);
static inline tmstamp_t tmstamp_init();

static inline size_t tmstamp_strfmt(
    const tmstamp_t *ts, char *buf, size_t bufsz, const char*fmt, bool utc
);



typedef struct Chronometer {
    tmstamp_t timenow;
    tmstamp_t offset;
    tmstamp_t *timesbuf;
    char *display;       // display string for viewing timestr
    size_t ticks;
    bool frozen;
    bool dirty;
    bool utc;
    bool dst;
} chrono_t;

static inline int chrono_init(chrono_t *ch, bool utc);
static inline int chrono_tick(chrono_t *ch);
static inline void chrono_freeze(chrono_t *ch);

typedef struct StopWatch {
    chrono_t curr;
    chrono_t start;
    chrono_t stop;
    time_t epoch;
    bool reset;
} stopwatch_t;

//#pragma pack(1). look into
// typedef struct {
//     Jerror error;
//     Jansi ansi;
//     chrono_t clock;
//     tmstamp_t epoch;
//     stopwatch_t watch;
//     tmstamp_t *tstamps;
// } Jtime;

#endif /* JTIME_H */
#define JTIME_IMPL // #debug-mode
#ifdef JTIME_IMPL

// [compact]  = "%Y%m%d_%H%M%S"
// [iso-comp] = "%Y-%m-%dT%H:%M:%S"
// [filename] = "%Y%m%d_%H%M%S"
// [us_date]  = "%m/%d/%Y"
// [eu_date]  = "%d/%m/%Y"
// [clock24]  = "%H:%M:%S"
// [clock12]  = "%I:%M:%S %p"  

// place the highest value into a var of type time_t {(8bytes)-1}
time_t max_v = 0x7FFFFFFFl;

static const tfmtspec_t timefmt_specs[] = {
    { "%Y", "year_full", TFMT_YEAR,
      TFMT_NUMERIC | TFMT_PADZERO | TFMT_FIX_WIDTH, 4 },
    { "%m", "month_num", TFMT_MONNUM,
      TFMT_NUMERIC | TFMT_PADZERO | TFMT_FIX_WIDTH, 2 },
    { "%d", "day_num", TFMT_DAYNUM,
      TFMT_NUMERIC | TFMT_PADZERO | TFMT_FIX_WIDTH, 2 },
    { "%H", "hour_24", TFMT_HR24,
      TFMT_NUMERIC | TFMT_PADZERO | TFMT_FIX_WIDTH, 2 },
    { "%I", "hour_12", TFMT_HR12,
      TFMT_NUMERIC | TFMT_PADZERO | TFMT_FIX_WIDTH, 2 },
    { "%M", "minute", TFMT_MIN,
      TFMT_NUMERIC | TFMT_PADZERO | TFMT_FIX_WIDTH, 2 },
    { "%S", "second", TFMT_SEC,
      TFMT_NUMERIC | TFMT_PADZERO | TFMT_FIX_WIDTH, 2 },
    { "%p", "ampm", TFMT_AMPM,
      TFMT_TEXT, 2 }, { "%%", "percent", TFMT_PRCNT, TFMT_LITERAL, 1 }
};

static const tfmtpset_t timefmt_presets[] = {
    { "filename", "%Y%m%d_%H%M%S",
      TFMT_PRESET_BUILTIN | TFMT_PRESET_DATETIME | TFMT_PRESET_FILESAFE },
    { "iso", "%Y-%m-%d %H:%M:%S", TFMT_PRESET_BUILTIN | TFMT_PRESET_DATETIME },
    { "us_date", "%m/%d/%Y", TFMT_PRESET_BUILTIN | TFMT_PRESET_DATEONLY },
    { "eu_date", "%d/%m/%Y", TFMT_PRESET_BUILTIN | TFMT_PRESET_DATEONLY },
    { "clock24", "%H:%M:%S", TFMT_PRESET_BUILTIN | TFMT_PRESET_TIMEONLY },
    { "clock12", "%I:%M:%S %p", TFMT_PRESET_BUILTIN | TFMT_PRESET_TIMEONLY }
};

static inline time_t getnow(){
    return time(NULL);
}

static inline bool getlcl(time_t raw, struct tm *dest){
    if (!dest) return false;

#if defined(_WIN32)
    return localtime_s(out, &raw) == 0;
#else
    return localtime_r(&raw, dest) != NULL;
#endif
}

static inline bool getutc(time_t raw, struct tm *dest){
    if (!dest) return false;

#if defined(_WIN32)
    return gmtime_s(out, &raw) == 0;
#else
    return gmtime_r(&raw, dest) != NULL;
#endif
}

static inline void tmstamp_fillfields(tmstamp_t *ts);


static inline timestr_t timestr_init(void){
    timestr_t ts = {0};

    ts.preset  = NULL;
    ts.format  = NULL;
    ts.display = NULL;

    // ts.directives = map_init(sizeof(char *), sizeof(tfmtspec_t *));
    // ts.presets    = map_init(sizeof(char *), sizeof(tfmtpset_t *));
    // ts.tokens     = jlist_init(sizeof(tfmttoken_t));

    ts.flags = TSTR_NONE;

    return ts;
}

static inline bool isleapyear(int year){
    if (year % 400 == 0) return true;
    if (year % 100 == 0) return false;
    return (year % 4 == 0);
}

static inline int weekofyear(const struct tm *tmt){
    if (!tmt || tmt->tm_yday < 0) return -1;
    return (tmt->tm_yday / 7) + 1;
}

static inline long gmtoffset(const struct tm *tmt, time_t raw){
#if defined(__APPLE__)
    (void)raw;
    return (long)tmt->tm_gmtoff;
#else
    struct tm gmtm = {0};
    struct tm loctm = {0};
#if defined(_WIN32)
    gmtime_s(&gmtm, &raw);
    localtime_s(&loctm, &raw);
#else
    gmtime_r(&raw, &gmtm);
    localtime_r(&raw, &loctm);
#endif
    time_t g = mktime(&gmtm);
    time_t l = mktime(&loctm);

    return (long)difftime(l, g);
#endif

    // if (!getutc(raw, &gmtm)) return 0;
    // if (!getlcl(raw, &loctm)) return 0;
}

static inline void tmstamp_fillfields(tmstamp_t *ts){
    if (!ts) return;

    ts->sec   = ts->jtm.tm_sec;
    ts->min   = ts->jtm.tm_min;
    ts->hour  = ts->jtm.tm_hour;
    ts->mday  = ts->jtm.tm_mday;
    ts->mon   = ts->jtm.tm_mon + 1;
    ts->year  = ts->jtm.tm_year + 1900;
    ts->wday  = ts->jtm.tm_wday;
    ts->yday  = ts->jtm.tm_yday;
    ts->doy   = ts->jtm.tm_yday + 1;
    ts->yweek = weekofyear(&ts->jtm);
    ts->isdst = ts->jtm.tm_isdst;

    ts->weekend  = (ts->wday == 0 || ts->wday == 6);
    ts->leapyear = isleapyear(ts->year);
    ts->gmtoff   = ts->utc ? 0 : gmtoffset(&ts->jtm, ts->rawtime);
}

static inline void fillstrings(tmstamp_t *ts){
    if (!ts) return;
//     strftime(ts->timestr.isofmt, sizeof(ts->timestr.isofmt),
//              "%Y-%m-%dT%H:%M:%S", &ts->time);

//     strftime(ts->timestr.dateeur, sizeof(ts->timestr.dateeur),
//              "%d/%m/%YT%H:%M:%S", &ts->time);

//     strftime(ts->timestr.dateusa, sizeof(ts->timestr.dateusa),
//              "%m/%d/%YT%H:%M:%S", &ts->time);

//     strftime(ts->timestr.tm_zone, sizeof(ts->timestr.tm_zone),
//              "%Z",  &ts->time);

//     snprintf(ts->display,
//              sizeof(ts->display), "%s %02d:%02d:%02d %s%s", "LOCAL",
//              ts->hour, ts->min, ts->sec, ts->timestr.dateusa,
//              ts->timestr.tm_zone[0] ? ts->timestr.tm_zone : "");
}


int tmstamp_from_time(tmstamp_t *ts, time_t raw, bool utc){
    if (!ts) return -1;

    memset(ts, 0, sizeof(*ts));
    ts->rawtime = raw;

#if defined(_WIN32)
    if (utc) {
        if (gmtime_s(&ts->time, &raw) != 0) {
            ts->valid = false;
            return -1;
        }
    } else {
        if (localtime_s(&ts->time, &raw) != 0) {
            ts->valid = false;
            return -1;
        }
    }
#else
    if (ts->utc) {
        if (!gmtime_r(&raw, &ts->jtm)) {
            ts->valid = false;
            return -1;
        }
    } else {
        if (!localtime_r(&raw, &ts->jtm)) {
            ts->valid = false;
            return -1;
        }
    }
#endif

    ts->sec   = ts->jtm.tm_sec;
    ts->min   = ts->jtm.tm_min;
    ts->hour  = ts->jtm.tm_hour;
    ts->wday  = ts->jtm.tm_wday;
    ts->mon   = ts->jtm.tm_mon + 1;
    ts->mday  = ts->jtm.tm_mday;
    ts->year  = ts->jtm.tm_year + 1900;
    ts->yday  = ts->jtm.tm_yday;
    ts->doy   = ts->jtm.tm_yday + 1;
    ts->yweek = weekofyear(&ts->jtm);
    ts->isdst = ts->jtm.tm_isdst;

    ts->weekend = (ts->wday == 0 || ts->wday == 6);
    ts->leapyear = isleapyear(ts->year);
    ts->gmtoff = ts->utc ? 0 : gmtoffset(&ts->jtm, raw);

    ts->msec = 0;
    ts->usec = 0;
    ts->nsec = 0;

    fillstrings(ts);

    ts->valid = true;
    return 0;
}

static inline tmstamp_t timestamp_init(){
    tmstamp_t ts = {0};
    ts.rawtime = getnow();
    ts.jtm = gettime(ts.rawtime);
    ts.mon = ts.jtm.tm_mon + 1;
    ts.year = ts.jtm.tm_year + 1900;
    ts.leapyear = isleapyear(ts.year);
    ts.yweek = weekofyear(&ts.jtm);
    /* will eventually make condition based on locale */
    //ts.display = ts.timestr.preset;
    //ts.gmt = gmtime(ts.time);
    //ts.est = localtime(ts.rawtime);
    fillstrings(&ts);
    ts.valid = true;

    return ts;
}

int timestamp_refresh(tmstamp_t *ts){
    if (!ts) return -1;
    return tmstamp_from_time(ts, ts->rawtime, true);
}



#endif /* JTIME_IMPL */
