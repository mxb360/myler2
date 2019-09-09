#include "console.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#define SET_COLOR(x)      printf("\033[%dm", (x))
#define SET_POS(x, y)     printf("\033[%d;%dH", (y)+1, (x)+1)
#define GET_POS()         printf("\033[6n")

#define __mybe_should_init_console__  if (!is_console_init) con_init();

static int is_console_init = 0;

static const color_t color_tab[8] = {0, 4, 2, 6, 1, 5, 3, 7};
static color_t current_bcolor = DCLR, current_fcolor = DCLR;
static int origin_x = 0, origin_y = 0;
static int axis_x = 1, axis_y = 1;
static int use_color = 1;

static int _kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if(ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

static int _getch(void) 
{
    int c;
    struct termios org_opts, new_opts;

    tcgetattr(STDIN_FILENO, &org_opts);
    memcpy(&new_opts, &org_opts, sizeof(new_opts));
    new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOKE | ICRNL);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
    c = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);

    return c;
}

static void con_init()
{
    current_bcolor = current_fcolor = DCLR;
    axis_x = axis_y = 1;
    origin_x = origin_y = 0;
    use_color = 1;
    SET_COLOR(0);
    is_console_init = 1;
}

/* 设置当前屏幕所设置的前景色为fcolor */
void con_set_fcolor(color_t fcolor)
{
    __mybe_should_init_console__
    
    if (!use_color)
        return;
    
    if (fcolor < 8)
        SET_COLOR(0), SET_COLOR(30 + color_tab[fcolor]);
    else if (fcolor < 16)
        SET_COLOR(1), SET_COLOR(30 + color_tab[fcolor - 8]);
    else if (fcolor == DCLR)
    {
        SET_COLOR(0);
        if (current_bcolor != DCLR)
            con_set_bcolor(current_bcolor);
    }
}

/* 设置当前屏幕所设置的背景色为bcolor */
void con_set_bcolor(color_t bcolor)
{
    __mybe_should_init_console__
    
    if (!use_color)
        return;
    
    if (bcolor < 8)
        SET_COLOR(40 + color_tab[bcolor]);
    else if (bcolor < 16)
        SET_COLOR(100 + color_tab[bcolor - 8]);
    else if (bcolor == DCLR)
    {
        SET_COLOR(0);
        if (current_fcolor != DCLR)
            con_set_fcolor(current_bcolor);
    }
}

/* 设置屏幕缓冲区的大小
 */
void con_set_buf_size(int width, int height)
{
    (void)width;
    (void)height;
    __mybe_should_init_console__
}

/* 获取屏幕缓冲区的大小
 */
void con_get_buf_size(int *w, int *h)
{
    __mybe_should_init_console__
    *w = -1;
    *h = -1;
}

/* 重置控制台的颜色为原始颜色 */
void con_reset_color()
{
    __mybe_should_init_console__
    SET_COLOR(0);
    current_bcolor = current_fcolor = DCLR;
}

void con_use_color(int _use_color)
{
    __mybe_should_init_console__
    use_color = _use_color;
}

/* 设置当前光标的坐标 */
void con_set_pos(int x, int y)
{
    __mybe_should_init_console__
    SET_POS(origin_x + axis_x * x, origin_y + axis_y * y);
}

/* 获取当前光标的坐标 */
void con_get_pos(int *x, int *y)
{
    int _x = 0, _y = 0, ch;
    __mybe_should_init_console__

    GET_POS();

    _getch(), _getch();
    while (isdigit(ch = _getch()))
        _y = _y * 10 + ch - '0';
    while (isdigit(ch = _getch()))
        _x = _x * 10 + ch - '0';
    //printf("\b\b");

    *x = (_x - 1 - origin_x) / axis_x;
    *y = (_y - 1 - origin_y) / axis_y;	
}

/* 设置逻辑坐标原点的绝对坐标位置
 * (x, y)为逻辑坐标原点的绝对坐标
 * 默认的原点为(0, 0)
 */
void con_set_origin(int x, int y)
{
    __mybe_should_init_console__
    origin_x = x;
    origin_y = y;
}

/* 获取逻辑坐标原点的绝对坐标位置
 * (x, y)为逻辑坐标原点的绝对坐标
 * 默认的原点为(0, 0)
 */
void con_get_origin(int *x, int *y)
{
    __mybe_should_init_console__
    *x = origin_x;
    *y = origin_y;
}

/* 设置逻辑x, y坐标轴的单位长度和方向(可以是负数)
 * dx, dy分别表示x, y坐标轴的单位长度
 * 如果值为负，表示逻辑坐标的方向与绝对坐标方向相反
 * 默认的值为(1, 1)
 */
void con_set_axis(int x, int y)
{	
    __mybe_should_init_console__
    axis_x = x;
    axis_y = y;
}

/* 获取逻辑x, y坐标轴的单位长度和方向
 * dx, dy分别表示x, y坐标轴的单位长度
 * 如果值为负，表示逻辑坐标的方向与绝对坐标方向相反
 * 默认的值为(1, 1)
 */
void con_get_axis(int *x, int *y)
{
    __mybe_should_init_console__
    *x = axis_x;
    *y = axis_y;
}

/* 判断当前是否有按键按下
 * 如果有，函数返回1，否则返回0
 * 此函数通常与函数con_get_key()配对使用
 */
int con_have_key(void)
{
    return !!_kbhit();
}

/* 获取当前的按键
 * _case： 是否区分字母的大小写，0表示区分，否则不区分
 * 如果不区分大小写，对于所有字母，得到的都将是大写字母的ascii码
 * 返回字符对应的ascii码
 * 但是对于方向键，返回的是负数(KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT)
 * 对于部分不可打印的可用的ascii码，提供了对应的宏使用(KEY_BACK, KEY_ESC, KEY_TAB, ...)
 * 如果当前没有键按下，此函数会一直等待，直到用户按下有效的键
 * 如果不想等待，你可以先调用函数con_have_key()判断当前是否有按键，如果有才调用此函数
 * 当调用此函数时，屏幕上不会有任何显示
 */
int con_get_key(int _case)
{
    int key;
    __mybe_should_init_console__

    key = _getch();        /* 获取按键 */
    
    if (key == 91 && _kbhit()) {      /* 方向键 */
        switch (_getch()) {
        case 65: return KEY_UP;
        case 68: return KEY_LEFT;
        case 67: return KEY_RIGHT;
        case 66: return KEY_DOWN;
        }
    } else if (key) {              /* 其他键 */
        if (_case)
            return toupper(key);  /* 如果忽略大小写，对于字母，都转为大写 */
        else
            return key;
    }

    return key;
}

/* 使程序暂停下来，显示提示字符串str后，按下任意键后继续
 * 如果str为NULL则显示系统默认的字符，一般是： 请按任意键继续. . .
 * 如果什么也不想显示，str可以为空串""
 */
void con_pause(const char *prompt)
{
    __mybe_should_init_console__
    if (!prompt)
        prompt = "请按任意键继续. . .";
    printf("%s", prompt);
    _getch();
}

/* 清除屏幕上显示的所有文字，并将光标移动至窗口坐标原点 */
void con_clear(void)
{
    printf("\033[2J");
}

/* 重置控制台的各个属性 */
void con_reset(void)
{
    con_init();
}

/* 从控制台获取密码
 * 将密码字符串存入到buf中，最多允许输入size字节
 * echo_mode:
 *    ECHO_NOTHING: 不回显输入的字符
 *    ECHO_STAR:    将输入的字符全用*回显
 *    ECHO_CHAR:    回显输入的字符
 * 返回buf指针
 */
char *con_get_password(char *buf, char size, int echo_mode)
{
    int top = 0, ch = -1;
    while (1) {
        ch = con_get_key(MATCH_CASE);
        if (ch >= ' ' && top < size) {
            buf[top++] = ch;
            if (echo_mode == ECHO_STAR)
                putchar('*');
            else if (echo_mode == ECHO_CHAR)
                putchar(ch);
        } else if (ch == '\b' && top) {
            top--;
            if (echo_mode == ECHO_CHAR || echo_mode == ECHO_STAR)
                printf("\b \b");
        } else if (ch == '\n' || ch == '\r')
            break;
    }
    putchar('\n');
    buf[top] = '\0';
    return buf;
}

/* 将UTF-8字符串转化为终端字符串
 * 可用于在控制台中显示UTF-8编码的文件的内容
 * 注意：
 *    由于Linux终端编码默认就是UTF-8，所以无需做字符转换，此函数仅是单纯的复制字符串
 *    而其他系统（如Windows）终端不一定是UTF-8编码，此函数的功能便不一致了
 *    为了可移植，如果你需要UTF-8与终端交互，始终考虑此函数，其他情况均不要使用此函数
 */
char *con_utf2con(char *con_string, const char *utf8_string)
{
    return strcpy(con_string, utf8_string);
}

/* 将终端编码转化为UTF-8编码
 * 可用于在控制台中显示UTF-8编码的文件的内容
 * 注意：
 *    由于Linux终端编码默认就是UTF-8，所以无需做字符转换，此函数仅是单纯的复制字符串
 *    而其他系统（如Windows）终端不一定是UTF-8编码，此函数的功能便不一致了
 *    为了可移植，如果你需要UTF-8与终端交互，始终考虑此函数，其他情况均不要使用此函数
 */
char *con_con2utf(char *utf8_string, const char *gbk_string)
{
    return strcpy(utf8_string, gbk_string);
}
