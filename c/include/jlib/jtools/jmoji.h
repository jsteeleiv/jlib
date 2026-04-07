#pragma once
#ifndef JMOJI_H
#define JMOJI_H

#include <stddef.h>
#include <string.h>
#include <jstd/jerr.h>

/*
 * terminal/font must supportUTF-8 string literals for proper rendering.
 */
typedef enum EmojiFlags {
    EMOJI_OK = 0,
    EMOJI_WARN_WIDTH,
    EMOJI_MULTI_UCODE,
    EMOJI_ALIAS,
    EMOJI_UNSTABLE
} emojiflags_t;


typedef struct EmojiContext {
    const char *key;      /* lookup key: "vhs", "warning", "skull" */
    const char *glyph;    /* UTF-8 glyph, actual rendered emoji */
    const char *ucode;    /* codepoint text: "U+1F4FC" */
    const char *label;    /* human-readable label */
    const char *group;    /* grouping: "status", "tech", "fun", etc. */
    uint32_t hexcode;     /* codepoint hex: 0x1F4FC */
    /* small int {width}: accomodates for wider glyphs ("U+1F468 U+200D U+1F4BB") */
    uint8_t weight;
    uint8_t width;
    error_t error;
    uint8_t bias;
    short fuse;
} emoji_t;

static inline const char *emoji_defer(const char *key, const char *defer);
static inline const emoji_t *emoji_findby_glyph(const char *glyph);
static inline int emoji_group_isequal(const char *a, const char *b);
static inline const emoji_t *emoji_firstin(const char *group);
static inline size_t emoji_groupcount(const char *group);
static inline const emoji_t *emoji_get(const char *key);
static inline const char *emoji_glyph(const char *key);
static inline const char *emoji_ucode(const char *key);
static inline const char *emoji_label(const char *key);
static inline const char *emoji_group(const char *key);
static inline const emoji_t *emoji_index(size_t index);
const emoji_t *emoji_rand_glyph(const char *group);
const char *emoji_randby_group(const char *group);
static inline size_t emoji_count(void);

#define EMOJI_RANDG(group) emoji_randby_group(group)
#define EMOJI_RANDX(code) emoji_randby_hexcode(code)

#endif /* JMOJI_H */
#define JMOJI_IMPL // #debug-mode
#ifdef JMOJI_IMPL

#include <stdlib.h>

static const emoji_t EMOJI_MAP[] = {
    /* status / feedback */
    { "ok",           "✅", "U+2705",  "Check Mark Button",         "status" },
    { "check",        "✔️", "U+2714",  "Check Mark",                "status" },
    { "cross",        "❌", "U+274C",  "Cross Mark",                "status" },
    { "x",            "✖️", "U+2716",  "Multiplication X",          "status" },
    { "warning",      "⚠️", "U+26A0",  "Warning",                   "status" },
    { "error",        "🚫", "U+1F6AB", "Prohibited",                "status" },
    { "stop",         "🛑", "U+1F6D1", "Stop Sign",                 "status" },
    { "info",         "ℹ️", "U+2139",  "Information",               "status" },
    { "question",     "❓", "U+2753",  "Question Mark",             "status" },
    { "exclaim",      "❗", "U+2757",  "Exclamation Mark",          "status" },
    { "boom",         "💥", "U+1F4A5", "Collision",                 "status" },
    { "sparkles",     "✨", "U+2728",  "Sparkles",                  "status" },
    { "star",         "⭐", "U+2B50",  "Star",                      "status" },
    { "fire",         "🔥", "U+1F525", "Fire",                      "status" },
    { "cool",         "🆒", "U+1F192", "Cool Button",               "status" },
    { "new",          "🆕", "U+1F195", "New Button",                "status" },
    { "free",         "🆓", "U+1F193", "Free Button",               "status" },
    { "up",           "🆙", "U+1F199", "Up! Button",                "status" },
    { "soon",         "🔜", "U+1F51C", "Soon Arrow",                "status" },
    { "top",          "🔝", "U+1F51D", "Top Arrow",                 "status" },

    /* mood / reaction */
    { "lol",          "💀", "U+1F480", "Skull",                       "reaction" },
    { "laugh",        "😂", "U+1F602", "Face with Tears of Joy",      "reaction" },
    { "rofl",         "🤣", "U+1F923", "Rolling on Floor Laughing",   "reaction" },
    { "cry",          "😭", "U+1F62D", "Loudly Crying Face",          "reaction" },
    { "smile",        "😄", "U+1F604", "Grinning Face, Smiling Eyes", "reaction" },
    { "grin",         "😁", "U+1F601", "Beaming Face, Smiling Eyes",  "reaction" },
    { "wink",         "😉", "U+1F609", "Winking Face",                "reaction" },
    { "think",        "🤔", "U+1F914", "Thinking Face",               "reaction" },
    { "sus",          "🧐", "U+1F9D0", "Face with Monocle",           "reaction" },
    { "shock",        "😱", "U+1F631", "Face Screaming in Fear",      "reaction" },
    { "angry",        "😠", "U+1F620", "Angry Face",                  "reaction" },
    { "rage",         "🤬", "U+1F92C", "Face with Symbols on Mouth",  "reaction" },
    { "sad",          "☹️", "U+2639",  "Frowning Face",               "reaction" },
    { "neutral",      "😐", "U+1F610", "Neutral Face",                "reaction" },
    { "sleep",        "😴", "U+1F634", "Sleeping Face",               "reaction" },
    { "party",        "🥳", "U+1F973", "Partying Face",               "reaction" },
    { "clown",        "🤡", "U+1F921", "Clown Face",                  "reaction" },
    { "nerd",         "🤓", "U+1F913", "Nerd Face",                   "reaction" },
    { "robot",        "🤖", "U+1F916", "Robot",                       "reaction" },
    { "ghost",        "👻", "U+1F47B", "Ghost",                       "reaction" },
    
    /* hands / interaction */    
    { "thumbs_up",    "👍", "U+1F44D", "Thumbs Up",                     "hands" },
    { "thumbs_down",  "👎", "U+1F44E", "Thumbs Down",                   "hands" },
    { "clap",         "👏", "U+1F44F", "Clapping Hands",                "hands" },
    { "wave",         "👋", "U+1F44B", "Waving Hand",                   "hands" },
    { "fist",         "✊", "U+270A",  "Raised Fist",                   "hands" },
    { "rock",         "🤘", "U+1F918", "Sign of the Horns",             "hands" },
    { "metal",        "🤘", "U+1F918", "Sign of the Horns",             "hands" },
    { "peace",        "✌️", "U+270C",  "Victory Hand",                  "hands" },
    { "point_up",     "☝️", "U+261D",  "Index Pointing Up",             "hands" },
    { "pray",         "🙏", "U+1F64F", "Folded Hands",                  "hands" },
    { "muscle",       "💪", "U+1F4AA", "Flexed Biceps",                 "hands" },
    { "writing",      "✍️", "U+270D",  "Writing Hand",                  "hands" },
    
    /* hearts / symbols */    
    { "heart",        "❤️", "U+2764",  "Red Heart",                     "symbol" },
    { "orange_heart", "🧡", "U+1F9E1", "Orange Heart",                  "symbol" },
    { "yellow_heart", "💛", "U+1F49B", "Yellow Heart",                  "symbol" },
    { "green_heart",  "💚", "U+1F49A", "Green Heart",                   "symbol" },
    { "blue_heart",   "💙", "U+1F499", "Blue Heart",                    "symbol" },
    { "purple_heart", "💜", "U+1F49C", "Purple Heart",                  "symbol" },
    { "black_heart",  "🖤", "U+1F5A4", "Black Heart",                   "symbol" },
    { "broken_heart", "💔", "U+1F494", "Broken Heart",                  "symbol" },
    { "hundred",      "💯", "U+1F4AF", "Hundred Points",                "symbol" },
    { "target",       "🎯", "U+1F3AF", "Direct Hit",                    "symbol" },
    { "gem",          "💎", "U+1F48E", "Gem Stone",                     "symbol" },
    { "key",          "🔑", "U+1F511", "Key",                           "symbol" },
    { "lock",         "🔒", "U+1F512", "Locked",                        "symbol" },
    { "unlock",       "🔓", "U+1F513", "Unlocked",                      "symbol" },
    { "link",         "🔗", "U+1F517", "Link",                          "symbol" },
    { "pin",          "📌", "U+1F4CC", "Pushpin",                       "symbol" },
    { "paperclip",    "📎", "U+1F4CE", "Paperclip",                     "symbol" },
    { "mag",          "🔍", "U+1F50D", "Magnifying Glass Tilted Left",  "symbol" },
    { "mag_right",    "🔎", "U+1F50E", "Magnifying Glass Tilted Right", "symbol" },
    { "bulb",         "💡", "U+1F4A1", "Light Bulb",                    "symbol" },

    /* tech / computing */
    { "computer",     "💻", "U+1F4BB", "Laptop",                    "tech" },
    { "desktop",      "🖥️", "U+1F5A5", "Desktop Computer",          "tech" },
    { "keyboard",     "⌨️", "U+2328",  "Keyboard",                  "tech" },
    { "mouse",        "🖱️", "U+1F5B1", "Computer Mouse",            "tech" },
    { "printer",      "🖨️", "U+1F5A8", "Printer",                   "tech" },
    { "floppy",       "💾", "U+1F4BE", "Floppy Disk",               "tech" },
    { "disk",         "💿", "U+1F4BF", "Optical Disk",              "tech" },
    { "cd",           "💿", "U+1F4BF", "Optical Disk",              "tech" },
    { "dvd",          "📀", "U+1F4C0", "DVD",                       "tech" },
    { "folder",       "📁", "U+1F4C1", "File Folder",               "tech" },
    { "open_folder",  "📂", "U+1F4C2", "Open File Folder",          "tech" },
    { "file",         "📄", "U+1F4C4", "Page Facing Up",            "tech" },
    { "files",        "🗂️", "U+1F5C2", "Card Index Dividers",       "tech" },
    { "calendar",     "📅", "U+1F4C5", "Calendar",                  "tech" },
    { "clipboard",    "📋", "U+1F4CB", "Clipboard",                 "tech" },
    { "mail",         "📧", "U+1F4E7", "E-mail",                    "tech" },
    { "email",        "📧", "U+1F4E7", "E-mail",                    "tech" },
    { "inbox",        "📥", "U+1F4E5", "Inbox Tray",                "tech" },
    { "outbox",       "📤", "U+1F4E4", "Outbox Tray",               "tech" },
    { "package",      "📦", "U+1F4E6", "Package",                   "tech" },

    /* media / audio / video */
    { "vhs",          "📼", "U+1F4FC", "Videocassette",             "media" },
    { "cassette",     "📼", "U+1F4FC", "Videocassette",             "media" },
    { "movie",        "🎬", "U+1F3AC", "Clapper Board",             "media" },
    { "camera",       "📷", "U+1F4F7", "Camera",                    "media" },
    { "video",        "📹", "U+1F4F9", "Video Camera",              "media" },
    { "tv",           "📺", "U+1F4FA", "Television",                "media" },
    { "radio",        "📻", "U+1F4FB", "Radio",                     "media" },
    { "microphone",   "🎤", "U+1F3A4", "Microphone",                "media" },
    { "headphones",   "🎧", "U+1F3A7", "Headphone",                 "media" },
    { "speaker",      "🔊", "U+1F50A", "Speaker High Volume",       "media" },
    { "mute",         "🔇", "U+1F507", "Muted Speaker",             "media" },
    { "music",        "🎵", "U+1F3B5", "Musical Note",              "media" },
    { "notes",        "🎶", "U+1F3B6", "Musical Notes",             "media" },
    { "play",         "▶️", "U+25B6",  "Play Button",               "media" },
    { "pause",        "⏸️", "U+23F8",  "Pause Button",              "media" },
    { "stop_button",  "⏹️", "U+23F9",  "Stop Button",               "media" },
    { "record",       "⏺️", "U+23FA",  "Record Button",             "media" },
    { "fast_forward", "⏩", "U+23E9",  "Fast-Forward Button",       "media" },
    { "rewind",       "⏪", "U+23EA",  "Fast Reverse Button",       "media" },

    /* tools / dev / build */
    { "toolbox",      "🧰", "U+1F9F0", "Toolbox",                   "tools" },
    { "hammer",       "🔨", "U+1F528", "Hammer",                    "tools" },
    { "wrench",       "🔧", "U+1F527", "Wrench",                    "tools" },
    { "hammer_wrench","🛠️", "U+1F6E0", "Hammer and Wrench",         "tools" },
    { "gear",         "⚙️", "U+2699",  "Gear",                      "tools" },
    { "nut_bolt",     "🔩", "U+1F529", "Nut and Bolt",              "tools" },
    { "chain",        "⛓️", "U+26D3",  "Chains",                    "tools" },
    { "magnet",       "🧲", "U+1F9F2", "Magnet",                    "tools" },
    { "test_tube",    "🧪", "U+1F9EA", "Test Tube",                 "tools" },
    { "beaker",       "⚗️", "U+2697",  "Alembic",                   "tools" },
    { "dna",          "🧬", "U+1F9EC", "DNA",                       "tools" },
    { "microscope",   "🔬", "U+1F52C", "Microscope",                "tools" },
    { "telescope",    "🔭", "U+1F52D", "Telescope",                 "tools" },
    { "satellite",    "🛰️", "U+1F6F0", "Satellite",                 "tools" },

    /* coding / command-line vibes */
    { "terminal",     "🖳", "U+1F5B3", "Old Personal Computer",      "dev" },
    { "code",         "👨‍💻", "U+1F468 U+200D U+1F4BB", "Man Technologist", "dev" },
    { "bug",          "🐛", "U+1F41B", "Bug",                       "dev" },
    { "rocket",       "🚀", "U+1F680", "Rocket",                    "dev" },
    { "zap",          "⚡", "U+26A1",  "High Voltage",              "dev" },
    { "brain",        "🧠", "U+1F9E0", "Brain",                     "dev" },
    { "shield",       "🛡️", "U+1F6E1", "Shield",                   "dev" },
    { "unlock_code",  "🔓", "U+1F513", "Unlocked",                  "dev" },
    { "lock_code",    "🔒", "U+1F512", "Locked",                    "dev" },
    { "construction", "🚧", "U+1F6A7", "Construction",              "dev" },

    /* network / comms */
    { "phone",        "📞", "U+1F4DE", "Telephone Receiver",        "network" },
    { "mobile",       "📱", "U+1F4F1", "Mobile Phone",              "network" },
    { "antenna",      "📡", "U+1F4E1", "Satellite Antenna",         "network" },
    { "globe",        "🌐", "U+1F310", "Globe with Meridians",      "network" },
    { "wifi",         "📶", "U+1F4F6", "Antenna Bars",              "network" },
    { "mailbox",      "📫", "U+1F4EB", "Closed Mailbox with Raised Flag", "network" },
    { "chat",         "💬", "U+1F4AC", "Speech Balloon",            "network" },
    { "thought",      "💭", "U+1F4AD", "Thought Balloon",           "network" },
    { "megaphone",    "📣", "U+1F4E3", "Cheering Megaphone",        "network" },
    { "loudspeaker",  "📢", "U+1F4E2", "Loudspeaker",               "network" },

    /* time / progress */
    { "hourglass",    "⌛", "U+231B",  "Hourglass Done",            "time" },
    { "timer",        "⏲️", "U+23F2",  "Timer Clock",               "time" },
    { "clock",        "🕒", "U+1F552", "Three O’Clock",             "time" },
    { "watch",        "⌚", "U+231A",  "Watch",                     "time" },
    { "repeat",       "🔁", "U+1F501", "Repeat Button",             "time" },
    { "refresh",      "🔄", "U+1F504", "Counterclockwise Arrows Button", "time" },
    { "loop",         "🔁", "U+1F501", "Repeat Button",             "time" },
    { "loading",      "🌀", "U+1F300", "Cyclone",                   "time" },

    /* nature / misc flair */
    { "sun",          "☀️", "U+2600",  "Sun",                       "nature" },
    { "moon",         "🌙", "U+1F319", "Crescent Moon",             "nature" },
    { "cloud",        "☁️", "U+2601",  "Cloud",                     "nature" },
    { "rainbow",      "🌈", "U+1F308", "Rainbow",                   "nature" },
    { "snow",         "❄️", "U+2744",  "Snowflake",                 "nature" },
    { "leaf",         "🍃", "U+1F343", "Leaf Fluttering in Wind",   "nature" },
    { "tree",         "🌳", "U+1F333", "Deciduous Tree",            "nature" },
    { "flower",       "🌸", "U+1F338", "Cherry Blossom",            "nature" },

    /* food / fun */
    { "coffee",       "☕", "U+2615",  "Hot Beverage",              "fun" },
    { "tea",          "🍵", "U+1F375", "Teacup Without Handle",     "fun" },
    { "beer",         "🍺", "U+1F37A", "Beer Mug",                  "fun" },
    { "pizza",        "🍕", "U+1F355", "Pizza",                     "fun" },
    { "cookie",       "🍪", "U+1F36A", "Cookie",                    "fun" },
    { "popcorn",      "🍿", "U+1F37F", "Popcorn",                   "fun" },
    { "gamepad",      "🎮", "U+1F3AE", "Video Game",                "fun" },
    { "dice",         "🎲", "U+1F3B2", "Game Die",                  "fun" },
    { "trophy",       "🏆", "U+1F3C6", "Trophy",                    "fun" },
    { "medal",        "🏅", "U+1F3C5", "Sports Medal",              "fun" },

    /* books / learning */
    { "book",         "📘", "U+1F4D8", "Blue Book",                 "learning" },
    { "books",        "📚", "U+1F4DA", "Books",                     "learning" },
    { "notebook",     "📓", "U+1F4D3", "Notebook",                  "learning" },
    { "memo",         "📝", "U+1F4DD", "Memo",                      "learning" },
    { "ruler",        "📏", "U+1F4CF", "Straight Ruler",            "learning" },
    { "tri_ruler",    "📐", "U+1F4D0", "Triangular Ruler",          "learning" },
    { "backpack",     "🎒", "U+1F392", "Backpack",                  "learning" },
    { "graduation",   "🎓", "U+1F393", "Graduation Cap",            "learning" }
};

static const size_t EMOJI_COUNT = sizeof(EMOJI_MAP) / sizeof(EMOJI_MAP[0]);

static inline size_t emoji_count(void){ return EMOJI_COUNT; }

static inline int emoji_group_isequal(const char *a, const char *b){
    // if (a == NULL || b == NULL) return 0;
    // return strcmp(a, b) == 0;
    return (a && b && strcmp(a, b) == 0);
}

static inline const emoji_t *emoji_get(const char *key){
    if (key == NULL) return NULL;

    for (size_t i = 0; i < EMOJI_COUNT; i++) {
        if (emoji_group_isequal(EMOJI_MAP[i].key, key)) return &EMOJI_MAP[i];
    }
    return NULL;
}

static inline const char *emoji_glyph(const char *key){
    const emoji_t *e = emoji_get(key);
    return (e != NULL) ? e->glyph : NULL;
}

static inline const char *emoji_ucode(const char *key){
    const emoji_t *e = emoji_get(key);
    return (e != NULL) ? e->ucode : NULL;
}

static inline const char *emoji_label(const char *key){
    const emoji_t *e = emoji_get(key);
    return (e != NULL) ? e->label : NULL;
}

static inline const char *emoji_group(const char *key){
    const emoji_t *e = emoji_get(key);
    return (e != NULL) ? e->group : NULL;
}


static inline const emoji_t *emoji_index(size_t index){
    if (index >= EMOJI_COUNT) return NULL;
    return &EMOJI_MAP[index];
}

static inline const emoji_t *emoji_findby_glyph(const char *glyph){
    if (glyph == NULL) return NULL;

    for (size_t i = 0; i < EMOJI_COUNT; i++) {
        if (emoji_group_isequal(EMOJI_MAP[i].glyph, glyph)) {
            return &EMOJI_MAP[i];
        }
    }
    return NULL;
}

static inline size_t emoji_groupcount(const char *group){
    if (group == NULL) return 0;

    size_t count = 0;
    for (size_t i = 0; i < EMOJI_COUNT; i++) {
        if (emoji_group_isequal(EMOJI_MAP[i].group, group)) count++;
    }
    return count;
}

static inline const emoji_t *emoji_firstin(const char *group){
    if (group == NULL) return NULL;

    for (size_t i = 0; i < EMOJI_COUNT; i++) {
        if (emoji_group_isequal(EMOJI_MAP[i].group, group)) {
            return &EMOJI_MAP[i];
        }
    }
    return NULL;
}

static inline const char *emoji_defer(const char *key, const char *defer){
    const char *glyph = emoji_glyph(key);
    return (glyph != NULL) ? glyph : defer;
}

const emoji_t *emoji_rand_glyph(const char *group){
    if (group == NULL) return NULL;
    size_t count = 0;
    size_t current = 0;

    /* First pass: count matching */
    for (size_t i = 0; i < EMOJI_COUNT; ++i) {
        if (strcmp(EMOJI_MAP[i].group, group) == 0) count++;
    }

    if (count == 0) return NULL;

    /* Pick random index within matches */
    size_t target = (size_t)(rand() % count);

    /* Second pass: select */
    for (size_t i = 0; i < EMOJI_COUNT; ++i) {
        if (strcmp(EMOJI_MAP[i].group, group) == 0) {
            if (current == target) return &EMOJI_MAP[i];
            current++;
        }
    }
    return NULL; /* should never hit */
}

static inline const char *emoji_defer(const char *key, const char *defer){
    const char *glyph = emoji_glyph(key);
    return (glyph != NULL) ? glyph : defer;
}

const emoji_t *emoji_rand_glyph(const char *group){
    if (group == NULL) return NULL;
    size_t count = 0;
    size_t current = 0;

    /* First pass: count matching */
    for (size_t i = 0; i < EMOJI_COUNT; ++i) {
        if (strcmp(EMOJI_MAP[i].group, group) == 0) count++;
    }

    if (count == 0) return NULL;

    /* Pick random index within matches */
    size_t target = (size_t)(rand() % count);

    /* Second pass: select */
    for (size_t i = 0; i < EMOJI_COUNT; ++i) {
        if (strcmp(EMOJI_MAP[i].group, group) == 0) {
            if (current == target) return &EMOJI_MAP[i];
            current++;
        }
    }
    return NULL; /* should never hit */
}

const emoji_t *emoji_rand_ucode(const char *ucode){
    if (ucode == NULL) return NULL;
    size_t count = 0;
    size_t current = 0;

    /* First pass: count matching */
    for (size_t i = 0; i < EMOJI_COUNT; ++i) {
        if (strcmp(EMOJI_MAP[i].ucode, ucode) == 0) count++;
    }

    if (count == 0) return NULL;

    /* Pick random index within matches */
    size_t target = (size_t)(rand() % count);

    /* Second pass: select */
    for (size_t i = 0; i < EMOJI_COUNT; ++i) {
        if (strcmp(EMOJI_MAP[i].ucode, ucode) == 0) {
            if (current == target) return &EMOJI_MAP[i];
            current++;
        }
    }
    return NULL; /* should never hit */
}

const char *emoji_randby_group(const char *group){
    const emoji_t *e = emoji_rand_glyph(group);
    return e ? e->glyph : NULL;
}

const char *emoji_randby_ucode(const char *ucode){
    const emoji_t *e = emoji_rand_ucode(ucode);
    return e ? e->glyph : NULL;
}

#endif /* JMOJI_IMPL */
