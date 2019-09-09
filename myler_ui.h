#ifndef __MYLER_UI_H__
#define __MYLER_UI_H__

#include "myler_config.h"
#include "myler_list.h"
#include "console.h"

typedef struct {
    char src_str[UI_MAX_WIDTH];
    char str[UI_MAX_WIDTH];
    char end_str[7];
    color_t color;
    int align_style;
    bool status;
} ui_string_line_t;

typedef struct {
    ui_string_line_t search_line;
    ui_string_line_t main_window_lines[UI_MAX_HEIGHT];
    ui_string_line_t list_lines[UI_MAX_HEIGHT];
    ui_string_line_t status_line;
    ui_string_line_t timer_line;
    ui_string_line_t title_line;
    ui_string_line_t bottom_line;
} ui_string_buffer_t;


typedef struct {
    int x, y, w, h;
    int disable;
    bool status;
} ui_frame_t;

typedef struct myler_ui_t {
    int width, height;
    int old_width, old_height;
    int max_width, max_height;

    ui_frame_t list_frame;
    ui_frame_t main_window_frame;
    ui_frame_t search_frame;
    ui_frame_t timer_frame;
    ui_frame_t title_frame;

    ui_string_buffer_t buffer;

    unsigned int current_time, total_time;
    bool redraw;

    char message_buf[MAX_STR_BUF];
    bool message_status;
    int message_type;
} myler_ui_t;

typedef enum  {
    AlignLeft, AlignCenter, AlignRight,
} align_style_t;

void myler_ui_init(myler_ui_t *ui);
void myler_ui_set_search(myler_ui_t *ui, int search_name, int search_type, const char *search_words);
void myler_ui_set_timer(myler_ui_t *ui, unsigned int current_time, unsigned int total_time);
void myler_ui_set_list_line(myler_ui_t *ui, int line, color_t fcolor, int align_style, const char *format, ...);
void myler_ui_set_main_window_line(myler_ui_t *ui, int line, color_t fcolor, int align_style, const char *format, ...);
void myler_ui_set_status_line(myler_ui_t *ui, const char *title_format, ...);
void myler_ui_set_title(myler_ui_t *ui, const char *title_format, ...);
void myler_ui_set_bottom_line(myler_ui_t *ui, int align_style, color_t color, const char *format, ...);
void myler_ui_set_list_display(myler_ui_t *ui, bool enable);
void myler_ui_set_message(myler_ui_t *ui, int type, const char *format, ...);
void myler_ui_clear_main_window(myler_ui_t *ui);
void myler_ui_update(myler_ui_t *ui);
void myler_ui_set_main_window_display(myler_ui_t *ui, bool enable);
void myler_ui_set_search_display(myler_ui_t *ui, bool enable);
void myler_ui_set_timer_display(myler_ui_t *ui, bool enable);
bool myler_ui_get_list_disable(myler_ui_t *ui);
bool myler_ui_get_main_window_disable(myler_ui_t *ui);
bool myler_ui_get_search_disable(myler_ui_t *ui);
void myler_ui_set_title_display(myler_ui_t *ui, bool enable);
bool myler_ui_get_title_disable(myler_ui_t *ui);
bool myler_ui_get_timer_disable(myler_ui_t *ui);
void myler_ui_free(myler_ui_t *ui, bool clear);

extern const char *search_name_string[];
extern const char *search_type_string[];

#endif
