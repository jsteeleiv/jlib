#pragma once
#ifndef JFILE_H
#define JFILE_H

#include "../jtype.h"

#ifndef _WIN32
#    define _POSIX_C_SOURCE 200809L
#    include <unistd.h>
#    include <dirent.h>
#    include <fcntl.h>
#    define PATH_SEP "/"
/* from dir.h
-------------
 DIRSIZ gives the minimum record length which will hold the directory entry.
 This requires the amount of space in struct direct without the d_name field,
    plus enough space for the name with a terminating null byte
        (dp->d_namlen+1), rounded up to a 4 byte boundary.
 */
#undef DIRSIZ
#define DIRSIZ(dp) \
    (((unsigned long)&((struct direct *)0)->d_name + (dp)->d_namlen+1 + 3) & ~3)
#else
#    define WIN32_MEAN_AND_LEAN
#    include "windows.h"
#    define PATH_SEP "\\"
typedef struct Dirent {
    char d_name[MAX_PATH+1];
} dirent_t;

typedef struct DIR DIR;

DIR *opendir(const char *dirpath);
struct dirent *readdir(DIR *dirp);
int closedir(DIR *dirp);
#endif

#include <stdio.h>
#include <stdlib.h>

#define JSTR_IMPL
#include "../jtype/jstr.h"
#include "../jstd/jerr.h"
#include "jproc.h"


#define MAX_FILES 128
#define PATH_MAX 256

#define FILE_STDIN 0
#define FILE_STDOUT 1
#define FILE_STDERR 2


typedef enum FileAccess {
    FILE_ACC_NONE = 0,
    FILE_ACC_READ,
    FILE_ACC_WRITE,
    FILE_ACC_EXEC,
    FILE_ACC_LINK,
    FILE_ACC_KILL,
} fileacc_t;

typedef enum FileState {
    FILE_ST_NONE = 0,
    FILE_ST_EXISTS,
    FILE_ST_STAT_OK,
    FILE_ST_PATH_OWNED,
    FILE_ST_PATH_ABSOL,
} filest_t;

typedef enum FileValidation {
    FILE_VALID_NONE = 0,
    FILE_VALID_PATH,
    FILE_VALID_BASE,
    FILE_VALID_EXT,
    FILE_VALID_SIZE,
    FILE_VALID_TIME,
    FILE_VALID_TYPE,
    FILE_VALID_ACC,
} filevalid_t;

typedef enum FIleType {
    FILE_TYPE_UNKNOWN = 0,
    FILE_TYPE_STD,
    FILE_TYPE_DIR,
    FILE_TYPE_ASCII,
    FILE_TYPE_SLINK,
    FILE_TYPE_HLINK,
    FILE_TYPE_PIPE,
    FILE_TYPE_FIFO,
    FILE_TYPE_LIFO,
    FILE_TYPE_LOFI,
    FILE_TYPE_BLOCK,
    FILE_TYPE_SOCKET,
} filetype_t;

/*
 * File types
 */
#define DT_UNKNOWN 0
#define DT_FIFO    1
#define DT_CHR     2
#define DT_DIR     4
#define DT_BLK     6
#define DT_REG     8
#define DT_LINK    10
#define DT_SOCK    12
#define DT_WHT     14

/* Convert between stat structure types and directory types. */
#define IFTODT(mode)    (((mode) & 0170000) >> 12)
#define DTTOIF(dirtype) ((dirtype) << 12)

const char **directories;

typedef struct FileLocation {
    const char *file;
    const char *line;
    const char *func;
    uint file_len;
    uint line_len;
    uint func_len;
} fileloc_t;

typedef struct FileInfo {
    size_t size;
    time_t ctime;
    time_t mtime;
    time_t atime;
    uint mode;
    uint fields;
    uint type_flg; /* file, dir, symlink, pipe, etc */
    uint state_flg; /* exists, ok, owned, absolute, hidden, etc */
    uint access_flg; /* read, write, execute, etc */
    bool error;
    char *path;  // mutable 'string' for runtime changes
    const char *base;
    const char *ext;
} fileinfo_t;

typedef struct File {
    fileinfo_t *info;
    fileloc_t loc;
    FILE *handle;
} file_t;

static inline FILE *file_open(const char *path, const char *rwb, error_t *err);
static inline void file_close(FILE *f, error_t *err);

Fd fd_open_for_read(Cstr path);
Fd fd_open_for_write(Cstr path);
void fd_close(Fd fd);


static inline int path_isdir(cstr_t path);
#define ISDIR(path) path_isdir(path)

static inline int path_exists(cstr_t path);
#define EXISTS(path) path_exists(path)

static inline void path_mkdirs(cstrarr_t path);
#define MK(...) \
  do { \
    cstrarr_t path = cstr_array_make(__VA_ARGS__, NULL); \
    INFO("MKDIRS: %s", cstr_array_join(PATH_SEP, path)); \
    path_mkdirs(path); \
  } while (0)

static inline void path_rename(cstr_t old, cstr_t new);
#define RN(old, new) \
  do { \
    INFO("RENAME: %s -> %s", old, new); \
    path_rename(old, new) \
  } while (0)

static inline void path_remove(cstr_t path);
#define RM(path) \
  do { \
    INFO("REMOVE: %s", path); \
    path_remove(path); \
  } while (0)




  /* dir struct stuff */
  static inline void dir_list();
  static inline int dir_list_tui();

#endif /* JFILE_H */
#define JFILE_IMPL // #debug-mode
#ifdef JFILE_IMPL

/* DIRENT_H
-----------
struct _telldir; // forward reference 

//structure describing an open directory. 
typedef struct {
	int	__dd_fd;	                    // file descriptor associated with directory 
	long __dd_loc;                   // offset in current buffer 
	long __dd_size;	                // amount of data returned 
	char *__dd_buf;	                // data buffer 
	int	__dd_len;	                    // size of data buffer 
	long __dd_seek;	                // magic cookie returned 
	__unused long __padding;          // (__dd_rewind space left for bincompat) 
	int	__dd_flags;	                    // flags for readdir 
	__darwin_pthread_mutex_t __dd_lock; // for thread locking 
	struct _telldir *__dd_td;           // telldir position recording 
} DIR;

*/

static inline void dir_list(){
    DIR *dir = opendir(".");
    if (!dir){
        perror("Unable to read directory");
        return;
    }
    struct dirent *entry;
    char *files[MAX_FILES];
    static int count = 0;
    while ((entry = readdir(dir))){
        if (entry->d_type == DT_REG){
            files[count] = strdup(entry->d_name);
            count++;
        }
    }
    closedir(dir);
    for (int i = 0; i < count; ++i){
        printf("%s\n", files[i]);
        free(files[i]);
    }
}

static inline int dir_list_tui(){


    printf("before getcwd\n");

    char current_path[PATH_MAX];
    if (getcwd(current_path, sizeof(current_path)) == NULL) {
        // Handle error
        printf("the issue in inside getcwd!");
        endwin();
        return 1;
    }

    int ch;
    while (1) {
        clear();
        
        printf("I made it here!");
        
        list_directory(current_path);
        refresh();        

        ch = getch();
        switch (ch) {
            case 'q':
                endwin();
                return 0;
            case 'j':
                
                break;
            case 'k':
                
                break;
        }
    }
}

static inline FILE *file_open(const char *path, const char *rwb, error_t *err){
    FILE *f = fopen(path, rwb);
    if (!f) {
        // ERR_NEW("file not found", ERR_FILE_EXISTS, EXIT_FAILURE);
        // ERR_SET(err, "file not found", ERR_FILE_EXISTS, EXIT_FAILURE, false);
    }
    //SET_E(jerr, "set successful", NONE, SUCCESS, 0, false);
    return f;
}

static inline void file_close(FILE *f, error_t *err){
    if (!f) {
        //SET_E(jerr, "file not found", WARN, FILE_ERR, -1, false);    
    }
    fclose(f);
    //SET_E(jerr, "set successful", NONE, SUCCESS, 0, false);
}

int listdir(const char *path) 
{
  struct dirent *entry;
  DIR *dp;

  dp = opendir(path);
  if (dp == NULL) 
  {
    perror("opendir");
    return -1;
  }

  while((entry = readdir(dp)))
    puts(entry->d_name);

  closedir(dp);
  return 0;
}



int lsexample(int argc, char **argv) {
  int counter = 1;

  if (argc == 1)
	listdir(".");

  while (++counter <= argc) {
    printf("\nListing %s...\n", argv[counter-1]);
    listdir(argv[counter-1]);
  }

  return 0;
}




// void list_directory(const char *path) {
//     struct dirent *entry;
//     DIR *dir = opendir(path);

//     if (dir == NULL) {
//         //printf("%s not found", dir);
//         endwin();
//         return;
//     }

//     while ((entry = readdir(dir)) != NULL) {
    
//         int i = 0;
//         dir_list[i] = entry->d_name;
//         if(dir_list[0]){
            
//         }
//         else if(dir_list[1]){
//             //attron(COLOR_PAIR(1));
//             printf("I have removed the implied . directory");
//             printw("%s\n", entry->d_name);
//             //attroff(COLOR_PAIR(1));

//         }
//         else{
//             printw("%s\n", entry->d_name);

//         }
//         i++;
        
//         printw("%s\n", entry->d_name);
//     }

//     closedir(dir);
// }

void debug_path( const char *path) {
    (void) path;
}




#endif /* JFILE_IMPL */
