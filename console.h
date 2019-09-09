#ifndef __CONSOLE_H__
#define __CONSOLE_H__

/* 颜色相关（仅支持16种颜色） */
#define BLACK         0    /* 黑色     */
#define BLUE          1    /* 蓝色     */
#define GREEN         2    /* 绿色     */
#define AQUA          3    /* 浅绿色   */
#define RED           4    /* 红色     */
#define PURPLE        5    /* 紫色     */
#define YELLOW        6    /* 黄色     */
#define WHITE         7    /* 白色     */
#define GRAY          8    /* 灰色     */
#define LBLUE         9    /* 淡蓝色   */
#define LGREEN        10   /* 淡绿色   */
#define LAQUA         11   /* 淡浅绿色 */
#define LRED          12   /* 淡红色   */
#define LPURPLE       13   /* 淡紫色   */
#define LYELLOW       14   /* 淡黄色   */
#define LWHITE        15   /* 亮白色   */

#define DCLR          16   /* 默认颜色 */

/* 按键码 */
#define KEY_DOWN  -1       /* 方向键 上    */
#define KEY_UP    -2       /* 方向键 下    */
#define KEY_LEFT  -3       /* 方向键 左    */
#define KEY_RIGHT -4       /* 方向键 右    */
#define KEY_BACK  8        /* 退格键 '\b'  */
#define KEY_ENTER 13       /* 回车键 '\r'  */
#define KEY_ESC   27       /* Esc键        */
#define KEY_TAB   9        /* Tab键  '\t'  */
#define KEY_SPACE 32       /* 空格键 ' '   */

#define MATCH_CASE     0
#define NO_MATCH_CASE  1

/* 密码回显模式 */
#define ECHO_NOTHING   0
#define ECHO_STAR      1
#define ECHO_CHAR      2

#ifdef __cplusplus
extern "C" {
#endif 

typedef unsigned short color_t;

void con_get_pos(int *x, int *y);

void con_get_origin(int *x, int *y);

void con_get_axis(int *dx, int *dy);

void con_get_buf_size(int *w, int *h);

void con_set_pos(int x, int y);

void con_set_fcolor(color_t color);

void con_set_bcolor(color_t color);

void con_set_origin(int x, int y);

void con_set_axis(int dx, int dy);

void con_set_buf_size(int w, int h);

void con_set_cur_visible(int visible);

int  con_have_key(void);

int  con_get_key(int _case);

void con_pause(const char *prompt);

void con_clear(void);

void con_reset(void);

void con_reset_color(void);

void con_use_color(int use_color);

void con_sleep(int ms);

char *con_get_password(char *buf, char size, int echo_mode);

char *con_utf2con(char *con_string, const char *utf8_string);

char *con_con2utf(char *utf8_string, const char *con_string);

#ifdef __cplusplus
}
#endif 

#endif  /* !__CONSOLE_H__ */
