#ifndef __MYLER_H__
#define __MYLER_H__

#include "myler_config.h"
#include "myler_utils.h"
#include "myler_lyrics.h"
#include "myler_ui.h"
#include "myler_list.h"
#include "myler_cmdline.h"
#include "myler_search.h"

#include "httpclient.h"

#define DefaultList      1
#define TempList         0
#define LocalList        2

struct myler_player_t;

typedef void (*myler_key_event_func_t)(struct myler_player_t *player);

typedef struct myler_key_event_t {
    int key;
    myler_key_event_func_t key_event_func;
} myler_key_event_t;

typedef enum enter_focus_t { NoFoucs, ListFocus, MainWindowFocus } enter_focus_t;

typedef struct myler_player_t {
    myler_ui_t *ui;
    myler_cmdline_t *cmdline;

    myler_list_t *list[MAX_LIST_COUNT];
    myler_list_t *search_song_list;
    int list_count;
    myler_list_display_t list_display;
    myler_list_display_t search_display;

    char local_list_path[MAX_STR_BUF];

    myler_list_t *current_list;

    myler_key_event_t key_events[MAX_KEY_EVEVT];
    int key_event_count;
    bool show_main_lyric;
    bool show_search;
    int enter_focus;

    int speed_up_down;
    int volume;
    int volume_change;
    int play_mode;
    search_engine_type_t engine;
    search_type_t search_type;
    char search_words[MAX_STR_BUF];

    char search_str_bak[UI_MAX_HEIGHT][MAX_STR_BUF];
} myler_player_t;


int myler_init(myler_player_t *player, myler_cmdline_t *cmd_line);
void myler_update(myler_player_t *player);
int myler_search_song(myler_player_t *player, search_engine_type_t engine, const char *words);
void myler_update_by_search_or_download(int proess, void *arg);
int myler_create_local_list(myler_player_t *player);
int myler_create_temp_list(myler_player_t *player);
int myler_create_default_list(myler_player_t *player);
void myler_add_key_event(myler_player_t *player, int key, myler_key_event_func_t key_event_func);
void myler_search_local_music(myler_player_t *player);
int myler_download_music(myler_player_t *player, myler_list_node_t *node);
void myler_quit(myler_player_t *player, bool clear);

#endif
