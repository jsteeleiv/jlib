#pragma once
#ifndef JFILE_H
#define JFILE_H

#include <ncurses/ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

//#include <jerr.h>


typedef struct FileInfo {
    size_t size;
    time_t created;
    time_t modified;
    uint32_t mode;
    bool exists;
    char *path;
} fileinfo_t;


#endif /* JFILE_H */


#ifdef JFILE_IMPL

FILE *jopen(const char *path, const char *rwb, Jerror *jerr){
    FILE *f = fopen(path, rwb);
    if (!f) {
        //SET_E(jerr, "file not found", WARN, FILE_ERR, -1, false);
    }
    //SET_E(jerr, "set successful", NONE, SUCCESS, 0, false);
    return f;
}

void jclose(FILE *f, Jerror *jerr){
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

#define PATH_MAX 256
#define DEFAULT_COLOR 1

const char **dir_list;



void list_directory(const char *path) {
    struct dirent *entry;
    DIR *dir = opendir(path);

    if (dir == NULL) {
        //printf("%s not found", dir);
        endwin();
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
    
        int i = 0;
        dir_list[i] = entry->d_name;
        if(dir_list[0]){
            
        }
        else if(dir_list[1]){
            //attron(COLOR_PAIR(1));
            printf("I have removed the implied . directory");
            printw("%s\n", entry->d_name);
            //attroff(COLOR_PAIR(1));

        }
        else{
            printw("%s\n", entry->d_name);

        }
        i++;
        
        printw("%s\n", entry->d_name);
    }

    closedir(dir);
}

void debug_path( const char *path) {
    (void) path;
}

int start() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    start_color();
    noecho();
    
    // Define color pairs
    init_pair(DEFAULT_COLOR, COLOR_BLACK, COLOR_WHITE);

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

    endwin();
    return 0;
}


#endif /* JFILE_IMPL */
