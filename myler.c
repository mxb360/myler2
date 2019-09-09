#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "myler.h"
#include "system.h"

static void myler_exit(myler_player_t *player)
{
    myler_quit(player, true);
}

/* 按键触发：反转列表界面显 示状态（私有函数） */
static void toggle_list_display(myler_player_t *player)
{
    myler_ui_set_list_display(player->ui, myler_ui_get_list_disable(player->ui));
}

/* 按键触发：反转主窗口界面显示状态（私有函数） */
static void toggle_main_window_display(myler_player_t *player)
{
    bool dis = myler_ui_get_main_window_disable(player->ui);
    myler_ui_set_main_window_display(player->ui, dis);
    if (dis)
        ;
}

/* 按键触发：反转搜索栏显示状态（私有函数） */
static void toggle_search_display(myler_player_t *player)
{
    myler_ui_set_search_display(player->ui, myler_ui_get_search_disable(player->ui));
}

/* 按键触发：反转时间和状态界面显示状态（私有函数） */
static void toggle_timer_display(myler_player_t *player)
{
    myler_ui_set_timer_display(player->ui, myler_ui_get_timer_disable(player->ui));
}

/* 按键触发：反转标题栏显示状态（私有函数） */
static void toggle_tittle_display(myler_player_t *player)
{
    myler_ui_set_title_display(player->ui, myler_ui_get_title_disable(player->ui));
}

/* 按键触发：暂停/继续播放音乐（私有函数） */
static void play_or_pause(myler_player_t *player)
{
    if (player->current_list->play_status == MUSIC_PAUSED)
        myler_list_resume(player->current_list);
    else if (player->current_list->play_status == MUSIC_PLAYING)
        myler_list_pasue(player->current_list);
}

/* 按键触发：下一曲（私有函数） */
static void next(myler_player_t *player)
{
    if (player->current_list) {
        if (player->play_mode == PlayRepeatOne)
            myler_list_set_next(player->current_list, PlayInOrder);
        else
            myler_list_set_next(player->current_list, player->play_mode);
        myler_ui_set_message(player->ui, 0, "下一曲");
    }
}

/* 按键触发：上一曲（私有函数） */
static void prev(myler_player_t *player)
{
    if (player->current_list) {
        myler_list_set_prev(player->current_list);
        myler_ui_set_message(player->ui, 0, "上一曲");
        player->current_list->is_play_end = false;
    }
}

static void replay(myler_player_t *player)
{
    if (player->current_list) {
        myler_list_clear_current(player->current_list);
        player->current_list->replay = true;
        myler_ui_set_message(player->ui, 0, "重新播放");
    }
}

/* 按键触发：快进（私有函数） */
static void speed_up(myler_player_t *player)
{
    if (player->current_list && player->current_list->current) {
        int t = player->current_list->current_time + player->speed_up_down * 1000;
        if (player->current_list->is_open) {
            if (t < player->current_list->current->length)
                music_play(player->current_list->current->music, t, 0, 0);
            else
                music_play(player->current_list->current->music,
                           player->current_list->current->length, 0, 0);
            myler_ui_set_message(player->ui, 0, "快进%d秒", player->speed_up_down);
        }
    }
}

/* 按键触发：快退（私有函数） */
static void speed_down(myler_player_t *player)
{
    if (player->current_list && player->current_list->current) {  
        if (player->current_list->is_open) {
            int t = player->current_list->current_time - player->speed_up_down * 1000;
            if (t > 0)
                music_play(player->current_list->current->music, t, 0, 0);
            else
                music_play(player->current_list->current->music, 0, 0, 0);
            myler_ui_set_message(player->ui, 0, "快退%d秒", player->speed_up_down);
        }
    }
}

/* 按键触发：音量+（私有函数） */
static void volume_up(myler_player_t *player)
{
    player->volume += player->volume_change;
    if (player->volume > 100)
        player->volume = 100;
    myler_ui_set_message(player->ui, 0, "音量：%d", player->volume);
    if (player->current_list->is_open)
        music_set_volume(player->current_list->current->music, player->volume * 10);
}

/* 按键触发：音量-（私有函数） */
static void volume_down(myler_player_t *player)
{
    player->volume -= player->volume_change;
    if (player->volume < 0)
        player->volume = 0;
    myler_ui_set_message(player->ui, 0, "音量：%d", player->volume);
    if (player->current_list->is_open)
        music_set_volume(player->current_list->current->music, player->volume * 10);
}

static void move_up(myler_player_t *player)
{
    myler_list_display_t *display;
   
    if (player->enter_focus == ListFocus)
        display = &player->list_display;
    else if (player->enter_focus == MainWindowFocus)
        display = &player->search_display;
    else
        return;

    if (display->current_choice->prev) {
        display->current_choice = display->current_choice->prev;
        if (display->current_choice == display->head->prev)
            display->head = display->head->prev;
    }
}

static void move_down(myler_player_t *player)
{
    myler_list_display_t *display;

    if (player->enter_focus == ListFocus)
        display = &player->list_display;
    else if (player->enter_focus == MainWindowFocus)
        display = &player->search_display;
    else
        return;

    if (display->current_choice->next) {
        display->current_choice = display->current_choice->next;
        if (display->current_choice == display->tail->next)
            display->head = display->head->next;
    }
}

static void show_lyrics(myler_player_t *player)
{
    player->show_main_lyric = true;
    player->show_search = false;
    player->enter_focus = ListFocus;
}

static void show_search(myler_player_t *player)
{
    player->show_main_lyric = false;
    player->show_search = true;
    player->enter_focus = MainWindowFocus;
}

static void play_choice(myler_player_t *player)
{   
    if (player->enter_focus == ListFocus) {
        if (player->list_display.current_choice->type == IsListNodeType) {
            myler_list_clear_current(player->current_list);
            player->current_list = player->list_display.current_choice->list;
            player->current_list->current = player->list_display.current_choice->music;
            player->current_list->is_play_end = false;
            player->current_list->replay = true;
            myler_ui_set_message(player->ui, 0, "开始播放“%s - %s”", player->current_list->current->name, player->current_list->current->singer);
        }
    } else if (player->enter_focus == MainWindowFocus) {
        if (player->search_display.current_choice->type == IsListNodeType) {
            myler_list_node_t *node = player->search_display.current_choice->music;
            myler_ui_set_message(player->ui, 0, "开始下载“%s - %s”", node->name, node->singer);
            myler_ui_update(player->ui);
            if (myler_download_music(player, node)) {
                myler_ui_update(player->ui);
                return;
            }

            myler_ui_set_message(player->ui, 0, "“%s - %s”下载完成", node->name, node->singer);
            myler_list_add_node(player->list[TempList], node);
            myler_list_set_list(&player->list_display, player->list, player->list_count);
            myler_list_clear_current(player->current_list);
            player->current_list = player->list[TempList];
            player->current_list->current = node;
            player->current_list->is_play_end = false;
            player->current_list->replay = true;
            show_lyrics(player);
        }
    }
}

static void search(myler_player_t *player)
{
    char buf[MAX_STR_BUF + 50];

    if (player->search_type == Song && player->search_words[0]) {
        myler_list_free(player->search_song_list);
        myler_ui_clear_main_window(player->ui);
        myler_ui_set_main_window_line(player->ui, 0, LWHITE, AlignLeft, "搜索歌曲：“%s”中...", player->search_words);
        myler_ui_update(player->ui);

        sprintf(buf, "“%s”的搜索结果：", player->search_words);
        player->search_song_list = myler_list_create(buf);

        if (myler_search_song(player, player->engine, player->search_words)) {
            //myler_ui_set_message(player->ui, 1, "搜索失败：%s", http_client_get_last_error());
            myler_ui_update(player->ui);
            return;
        }
        player->show_main_lyric = false;
        player->show_search = true;
        player->enter_focus = MainWindowFocus;

        myler_list_set_list(&player->search_display, &player->search_song_list, 1);
    }
}

static const char *_mode_str[] = {
    "顺序播放", "列表循环", "单曲循环", "随机播放",
};

static void set_play_mode(myler_player_t *player)
{
    player->play_mode = (player->play_mode + 1) % PLAY_MODE_COUNT;
    myler_ui_set_message(player->ui, 0, "播放模式：%s", _mode_str[player->play_mode]);
}

static int add_local_file_to_list(myler_list_t *list, const char *file_path)
{
    char *files[MAX_LIST_SIZE];
    char buf[MAX_STR_BUF + 10];
    int n;

    strcpy(buf, file_path);
    if ((n = get_files(files, MAX_LIST_SIZE, buf)) < 0) 
        return -1; 

    for (int i = 0; i < n; i++) {
        if (!strcmp(files[i] + strlen(files[i]) - strlen(".mp3"), ".mp3"))
            myler_list_add_loacl_music(list, files[i]);
    }
    list->is_expand = true;
    return 0;
}

static void execute_cmdline(myler_player_t *player)
{
    if (player->cmdline->volume >= 0)
        player->volume = player->cmdline->volume;
    if (player->cmdline->have_loop)
        player->play_mode = PlayListLoop;
    if (player->cmdline->have_order)
        player->play_mode = PlayInOrder;
    if (player->cmdline->have_repeat)
        player->play_mode = PlayRepeatOne;
    if (player->cmdline->have_shuffle)
        player->play_mode = PlayShuffle;
    if (player->cmdline->words)
        strcpy(player->search_words, player->cmdline->words);

    for (int i = 0; i < player->cmdline->music_name_count; i++) {
        add_local_file_to_list(player->list[TempList], player->cmdline->music_name[i]);
    }
}

/* 播放器初始化 */
int myler_init(myler_player_t *player, myler_cmdline_t *cmd_line)
{
    myler_assert(player != NULL);
    myler_assert(player != NULL);

    srand((unsigned int)time(NULL));

    player->current_list = NULL;
    player->list_count = 0;
    player->key_event_count = 0;
    player->show_main_lyric = true;
    player->show_search = false;
    player->enter_focus = ListFocus;

    player->speed_up_down = DEFAULT_SPEED_UP_DOWN;
    player->volume_change = DEFAULT_VOLUME_CHANGE;
    player->volume = DEFAULT_VOLUME;
    player->play_mode = PlayInOrder;

    player->engine = NeteaseMusic;
    player->search_type = Song;
    strcpy(player->search_words, "take me to your heart");

    player->cmdline = malloc(sizeof(myler_cmdline_t));
    player->ui = malloc(sizeof(myler_ui_t));

    player->search_song_list = NULL;
    player->cmdline = cmd_line;

    // 创建本地列表
    myler_create_default_list(player);
    myler_create_temp_list(player);
    strncpy(player->local_list_path, LOCAL_LIST_PATH, MAX_STR_BUF);
    if (myler_create_local_list(player)) {
        myler_print_error(false, "本地列表创建失败");
        return -1;
    }

    execute_cmdline(player);
    myler_ui_init(player->ui);
        
    if (player->cmdline->have_mini) {
        myler_ui_set_title_display(player->ui, false);
        myler_ui_set_list_display(player->ui, false);
        myler_ui_set_main_window_display(player->ui, false);
        player->show_main_lyric = false;
    }
    myler_ui_set_search(player->ui, player->engine, player->search_type, player->search_words);

    // 默认的按键事件绑定
    myler_add_key_event(player, 'A', toggle_list_display);
    myler_add_key_event(player, 'Q', myler_exit);
    myler_add_key_event(player, 'S', toggle_main_window_display);
    myler_add_key_event(player, 'Z', toggle_timer_display);
    myler_add_key_event(player, 'L', toggle_search_display);
    myler_add_key_event(player, 'W', toggle_tittle_display);
    myler_add_key_event(player, '.', next);
    myler_add_key_event(player, ' ', play_or_pause);
    myler_add_key_event(player, ',', prev);
    myler_add_key_event(player, KEY_LEFT, speed_down);
    myler_add_key_event(player, KEY_RIGHT, speed_up);
    myler_add_key_event(player, ']', volume_up);
    myler_add_key_event(player, '[', volume_down);
    myler_add_key_event(player, KEY_UP, move_up);
    myler_add_key_event(player, KEY_DOWN, move_down);
    myler_add_key_event(player, KEY_ENTER, play_choice);
    myler_add_key_event(player, 'M', set_play_mode);
    myler_add_key_event(player, 'R', replay);
    myler_add_key_event(player, 'Y', show_search);
    myler_add_key_event(player, 'U', show_lyrics);
    myler_add_key_event(player, 'E', search);

    player->current_list = player->list[TempList]->music_count ?  player->list[TempList] : player->list[LocalList];
    int current_index;
    if (player->current_list->music_count) {
        current_index = player->play_mode == PlayShuffle ? rand() % player->current_list->music_count : 0;
        myler_list_set_current(player->current_list, current_index);
    }

    myler_list_set_list(&player->list_display, player->list, player->list_count);
    if (player->cmdline->have_stop)
        player->current_list->replay = false;

    http_client_init();
    return 0;
}

/* 为播放器添加按键绑定 */
void myler_add_key_event(myler_player_t *player, int key, myler_key_event_func_t key_event_func)
{
    myler_assert(player);
    myler_assert(key_event_func);

    player->key_events[player->key_event_count++] = (myler_key_event_t){key, key_event_func};
}

int myler_create_default_list(myler_player_t *player)
{
    player->list[DefaultList] = myler_list_create("默认列表");
    if (!player->list[DefaultList])
        return -1;
    player->list_count++;
    return 0;
}

int myler_create_temp_list(myler_player_t *player)
{
    player->list[TempList] = myler_list_create("临时列表");
    if (!player->list[TempList])
        return -1;
    player->list_count++;
    return 0;
}

int myler_create_local_list(myler_player_t *player)
{
    player->list[LocalList] = myler_list_create("本地列表");
    if (!player->list[LocalList])
        return -1;
    char *files[MAX_LIST_SIZE];
    char buf[MAX_STR_BUF + 10];
    int n;

    if (!file_exists(player->local_list_path)) {
        myler_print_warning("本地歌曲文件目录不存在");
        if (!create_dir(player->local_list_path)) {
            myler_print_warning("创建本地歌曲文件目录失败");
            return 0;
        }
    }

    strcpy(buf, player->local_list_path);
    if ((n = get_files(files, MAX_LIST_SIZE, join_path(buf, "*.mp3"))) < 0) {
        return 0;
    }

    for (int i = 0; i < n; i++)
        myler_list_add_loacl_music(player->list[LocalList], files[i]);
    player->list[LocalList]->is_expand = true;
    player->list_count++;
    return 0;
} 

void myler_search_local_music(myler_player_t *player)
{

}

void myler_update_by_search_or_download(int proess, void *arg)
{
    myler_player_t *player = (myler_player_t *)arg;
    myler_ui_set_message(player->ui, 0, "下载进度：%d%%", proess);
    myler_ui_update(player->ui);
}

static void myler_list_display_update(myler_player_t *player, myler_list_display_t *list_display, int line_count)
{
    int line = 0;
    myler_list_display_node_t *node = list_display->head;
    void (*myler_set_line_func)(myler_ui_t *, int, color_t, int, const char *, ...);

    if (list_display == &player->list_display)
        myler_set_line_func = myler_ui_set_list_line;
    else if (list_display == &player->search_display)
        myler_set_line_func = myler_ui_set_main_window_line;
    else
        return;

    while (node && line < line_count)
    {
        if (node->type == IsListType && node->list->is_expand) {
            if (list_display->current_choice != node)
                myler_set_line_func(player->ui, line, LBLUE, AlignLeft,  "- %s[%d首]", node->str1, node->list->music_count);
            else
                myler_set_line_func(player->ui, line, GREEN, AlignLeft,  "->%s[%d首]", node->str1, node->list->music_count);
        } else if (node->type == IsListType && !node->list->is_expand) {
            if (list_display->current_choice != node)
                myler_set_line_func(player->ui, line, LBLUE, AlignLeft,  "+ %s[%d首]", node->str1, node->list->music_count);
            else
                myler_set_line_func(player->ui, line, GREEN, AlignLeft,  "->%s[%d首]",  node->str1, node->list->music_count);
        } else {
            if (list_display->current_choice == node)   
                myler_set_line_func(player->ui, line, LGREEN, AlignLeft, "->* %s - %s", node->str1, node->str2);
            else if (node->music == player->current_list->current)
                myler_set_line_func(player->ui, line, GREEN, AlignLeft,  ">>* %s - %s", node->str1, node->str2);
            else
                myler_set_line_func(player->ui, line, LWHITE, AlignLeft, "  * %s - %s", node->str1, node->str2);
        }
        list_display->tail = node;
        node = node->next;
        line++;
    }   
    
    while (line < line_count - 1) {
        myler_ui_set_list_line(player->ui, line, WHITE, AlignLeft, "  ");
        line++;
    }
}

void myler_update(myler_player_t *player)
{
    // 按键事件
    while (con_have_key()) {
        int key = con_get_key(1);
        for (int i = 0; i < player->key_event_count; i++)
            if (key == player->key_events[i].key)
                player->key_events[i].key_event_func(player);
    }

    // 列表
    myler_list_display_update(player, &player->list_display, player->ui->list_frame.h - 2);
    if (player->show_search)
        myler_list_display_update(player, &player->search_display, player->ui->main_window_frame.h - 2);

    // 播放
    if (player->current_list->replay) {
        if (myler_list_open(player->current_list) == -2 || myler_list_play(player->current_list) == -2) {
            myler_ui_update(player->ui);
            con_sleep(500);
            myler_ui_set_message(player->ui, 1, "音乐“%s - %s”播放失败：%s", player->current_list->current->name,
                               player->current_list->current->singer, music_get_last_error());
            myler_ui_update(player->ui);
            con_sleep(1000);
            myler_ui_set_message(player->ui, 0, "尝试播放下一首 ...", music_get_last_error());
            if (player->play_mode == PlayRepeatOne)
                myler_list_set_next(player->current_list, PlayInOrder);
            else
                myler_list_set_next(player->current_list, player->play_mode);
            myler_ui_update(player->ui);
            return;
        }
        music_set_volume(player->current_list->current->music, player->volume * 10);
        player->current_list->replay = false;
    }

    // 进度和时间
    if (player->current_list->is_open) {
        player->current_list->current_time = music_get_current_length(player->current_list->current->music);
        player->current_list->play_status = music_get_status(player->current_list->current->music);
        myler_ui_set_timer(player->ui, player->current_list->current_time / 1000, player->current_list->current->length / 1000);
        if (player->current_list->play_status == MUSIC_STOPPED) 
            myler_list_set_next(player->current_list, player->play_mode);
    }

    // 状态栏
    if (player->current_list->is_play_end) {
        if (player->cmdline->have_exit)
            myler_quit(player, true);
        myler_ui_set_status_line(player->ui, "播放器空闲中...");
        myler_ui_set_timer(player->ui, 0, 0);
        myler_ui_clear_main_window(player->ui);
        myler_ui_set_bottom_line(player->ui, AlignCenter, WHITE, " ");
        myler_ui_update(player->ui);
        return;
    } else if (player->current_list->play_status == MUSIC_PLAYING)
        myler_ui_set_status_line(player->ui, "正在播放[%s - %s]", player->current_list->current->name,
                         player->current_list->current->singer);
    else if (player->current_list->play_status == MUSIC_PAUSED)
         myler_ui_set_status_line(player->ui, "已暂停播放[%s - %s]", player->current_list->current->name,
                         player->current_list->current->singer);

    // 歌词
    int lyrics_line_count = player->ui->main_window_frame.h - 1;
    char buf[MAX_STR_BUF];
    if (player->show_main_lyric)
        myler_ui_set_bottom_line(player->ui, AlignRight, LWHITE, "H: 帮助   ");
    if (player->current_list->play_status == MUSIC_PLAYING) {
        myler_lyrics_set_current_time(player->current_list->current->lyrics, player->current_list->current_time);
        for (int i = -lyrics_line_count/2; i < lyrics_line_count/2; i++) {
            con_utf2con(buf, myler_lyrics_get_lyrics(player->current_list->current->lyrics, i));
            color_t color;
            if (i == 0) {
                color = UI_LYRICS_COLOR2;
                if (player->current_list->current->lyrics) {
                    if (player->show_main_lyric)
                        myler_ui_set_main_window_line(player->ui, lyrics_line_count / 2 + i, color, AlignCenter, ">>>>  %s  <<<<", buf);
                    //else
                        myler_ui_set_bottom_line(player->ui, AlignCenter, UI_LYRICS_COLOR2, buf);
                } else if (player->show_main_lyric)
                    myler_ui_set_main_window_line(player->ui, lyrics_line_count / 2 + i, LRED, AlignCenter, ">>>>  未找到歌词文件  <<<<");
            } else if (player->show_main_lyric) {
                color = UI_LYRICS_COLOR1;
                myler_ui_set_main_window_line(player->ui, lyrics_line_count / 2 + i, color, AlignCenter, buf);
            }
        }
    }

    // 搜索
    if (player->cmdline->have_search) {
        search(player);
        player->cmdline->have_search = false;
    }

    // 界面
    myler_ui_update(player->ui);
}

static const char *engine_str[] = {"netease", "tencent", "kugou"};

int myler_search_song(myler_player_t *player, search_engine_type_t engine, const char *words)
{
    char url_buf[MAX_STR_BUF];
    char str_buf1[MAX_STR_BUF / 2];
    char str_buf2[MAX_STR_BUF / 2];
    http_client_t client;
    myler_assert(player != NULL);

    con_con2utf(str_buf1, words);
    http_client_url_encode(str_buf2, str_buf1);
    sprintf(url_buf, "%s&engine=%s&words=%s", SEARCH_SONG_URL, engine_str[engine], str_buf2);
    http_client_get(&client, url_buf,NULL, NULL);

    const char *err_string;

    if (client.error_code)
        err_string = http_client_get_last_error();
    else if (client.status_code == 200)  {
        int res = myler_search_song_parse(player->search_song_list, &client);
        if (res)
            myler_ui_set_message(player->ui, 1, "搜索失败: 数据解析错误");
        http_client_free(&client);
        return res;
    } else
        err_string = client.status_msg;
    myler_ui_set_message(player->ui, 1, "搜索失败: %s", err_string);
    http_client_free(&client);
    return 1;
}

int myler_download_music(myler_player_t *player, myler_list_node_t *node)
{
    char url_buf[MAX_STR_BUF * 2 + 100];
    http_client_t client;
    myler_assert(player != NULL);

    if (node->id[0] == 0) 
        return 1;

    sprintf(url_buf, "%s?engine=%s&id=%s", DOWNLOAD_LRC_URL, engine_str[player->engine], node->id);
    http_client_get(&client, url_buf,NULL, NULL);
    if (!client.error_code && client.status_code == 200) {
        sprintf(url_buf, "%s/%s - %s.lrc", DOWNLOAD_PATH, node->name, node->singer);
        http_client_save_file(&client, url_buf);
    }

    http_client_free(&client);
    sprintf(url_buf, "%s?engine=%s&id=%s", DOWNLOAD_SONG_URL, engine_str[player->engine], node->id);
    http_client_get(&client, url_buf, myler_update_by_search_or_download, player);
    if (!client.error_code && client.status_code == 200) {
        sprintf(url_buf, "%s/%s - %s.mp3", DOWNLOAD_PATH, node->name, node->singer);
        int res =  http_client_save_file(&client, url_buf);
        strcpy(node->path, url_buf);
        http_client_free(&client);
        return res;
    }

    const char *err_string = client.error_code ? http_client_get_last_error() : client.status_msg;
    myler_ui_set_message(player->ui, 0, "“%s - %s”下载失败(%s)", node->name, node->singer, err_string);
    http_client_free(&client);

    return 1;
}

void myler_quit(myler_player_t *player, bool clear)
{
    for (int i = 0; i < player->list_count; i++)
        myler_list_free(player->list[i]);
    myler_list_free(player->search_song_list);

    myler_ui_free(player->ui, clear);
    exit(0);
}
