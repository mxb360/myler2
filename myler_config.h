#ifndef __MYLER_CONFIG_H__
#define __MYLER_CONFIG_H__

#include <stdbool.h>

#define MYLER_DEBUG

#define myler_stdout          stdout
#define myler_stdin           stdin
#define myler_stderr          stderr

// UI配置
#define UI_MAX_WIDTH          100
#define UI_MAX_HEIGHT         30
#define UI_COLOR              LYELLOW
#define UI_SEARCH_COLOR       LPURPLE
#define SEARCH_BUF_SIZE       (UI_MAX_WIDTH/3*2-2)
#define UI_TIMER_LENGTH       (UI_MAX_WIDTH-20)
#define UI_TITLE_COLOR        LYELLOW
#define UI_STATUS_COLOR       LGREEN
#define UI_MAIN_WINDOW_WITDH  (UI_MAX_WIDTH/3*2-2)
#define UI_MAIN_WINDOW_HEIGHT (UI_MAX_HEIGHT-8)

#define UI_TIMER_COLOR1       LAQUA
#define UI_TIMER_COLOR2       LBLUE
#define UI_TIMER_COLOR3       LGREEN

#define UI_LYRICS_COLOR1      LWHITE
#define UI_LYRICS_COLOR2      LAQUA

#define MAX_STR_BUF           256
#define MAX_TIME              0xffffffff
#define MAX_KEY_EVEVT         50

#define MAX_LIST_COUNT        100
#define MAX_LIST_SIZE         1000

#define LOCAL_LIST_PATH        "LocalMusic"
#define DOWNLOAD_PATH          "DownloadMusic"

#define SEARCH_SONG_URL        "http://39.108.3.243/music/api/search?type=song"
#define DOWNLOAD_LRC_URL       "http://39.108.3.243/music/api/lyrics"
#define DOWNLOAD_SONG_URL      "http://39.108.3.243/music/api/song"

#define DEFAULT_SPEED_UP_DOWN  10
#define DEFAULT_VOLUME_CHANGE  5
#define DEFAULT_VOLUME         20

// http://39.108.3.243/music/api/search?engine=kugou&type=song&words=%E7%BB%BF%E8%89%B2
// http://39.108.3.243/music/api/song?id=9198b18815ee8ce42ae368ae29276f78&engine=kugou
// http://39.108.3.243/music/api/lyrics?id=9198b18815ee8ce42ae368ae29276f78&engine=kugou

typedef enum search_engine_type_t { NeteaseMusic, QQMusic, KuGouMusic } search_engine_type_t;
typedef enum search_type_t { Song, PlayList, Album, Lyrics } search_type_t;

#define MAX_SEARCH             100

#endif // !__MYLER_CONFIG_H__
