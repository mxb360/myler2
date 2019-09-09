#ifndef __MYLER_LIST_H__
#define __MYLER_LIST_H__

#include "myler_lyrics.h"
#include "myler_config.h"
#include "music.h"

struct myler_list_t;

typedef struct myler_list_node_t {
    struct myler_list_node_t *prev;
    struct myler_list_node_t *next;
    struct myler_list_t *list;

    music_t *music;
    myler_lyrics_t *lyrics;

    char id[MAX_STR_BUF];
    char path[MAX_STR_BUF];
    char name[MAX_STR_BUF];
    char singer[MAX_STR_BUF];
    int  length;
} myler_list_node_t;

typedef struct myler_list_t {
    char list_name[MAX_STR_BUF];
    myler_list_node_t *current;
    myler_list_node_t *head;
    myler_list_node_t *tail;
    int music_count;
    int current_time;
    bool is_open;
    int play_status;
    bool is_play_end;
    bool replay;
    bool is_expand;
} myler_list_t;

typedef struct myler_list_display_node_t {
    struct myler_list_display_node_t *next;
    struct myler_list_display_node_t *prev;
    int type;
    myler_list_t *list;
    myler_list_node_t *music;
    const char *str1;
    const char *str2;
} myler_list_display_node_t;

typedef struct myler_list_display_t {
    myler_list_display_node_t *display_list;
    myler_list_display_node_t *head;
    myler_list_display_node_t *tail;
    myler_list_display_node_t *current_choice;
    myler_list_display_node_t *default_choice;
    int display_list_count;
} myler_list_display_t;

enum {
    IsListType, IsListNodeType,
};

#define PLAY_MODE_COUNT     4
enum PlayMode {
    PlayInOrder, PlayListLoop, PlayRepeatOne, PlayShuffle,
};


myler_list_t *myler_list_create(const char *list_name);
void myler_list_add_node(myler_list_t *list, myler_list_node_t *node);
myler_list_node_t *myler_list_add_loacl_music(myler_list_t *list, const char *file_path);
myler_list_node_t *myler_list_set_current(myler_list_t *list, int index);
int myler_list_open(myler_list_t *list);
int myler_list_play(myler_list_t *list);
int myler_list_pasue(myler_list_t *list);
int myler_list_resume(myler_list_t *list);
int myler_list_set_next(myler_list_t *list, int play_mode);
int myler_list_set_prev(myler_list_t *list);
void myler_list_free(myler_list_t *list);
void myler_list_set_list(myler_list_display_t *list_display, myler_list_t *lists[], int list_count);
void myler_list_clear_current(myler_list_t *list);
void myler_list_display_free(myler_list_display_t *list_display);

#endif
