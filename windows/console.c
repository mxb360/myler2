#include "console.h"

#include <stdio.h>
#include <Windows.h>
#include <conio.h>

static int is_console_init;
static COORD origin;  
static COORD axis;
static color_t raw_color;
static int use_color;
static HANDLE hOut;

/* 重置控制台的各个属性
 */
void con_reset(void)
{
    origin.X = origin.Y = 0;
    axis.X = axis.Y = 1;

    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!is_console_init)
    {
        use_color = 1;
        CONSOLE_SCREEN_BUFFER_INFO bInfo;
        GetConsoleScreenBufferInfo(hOut, &bInfo);
        raw_color = bInfo.wAttributes;
    }
    is_console_init = 1;
}

/* 重置控制台的颜色为原始颜色
 */
void con_reset_color(void)
{
    if (!is_console_init)
        con_reset();
    if (use_color)
        SetConsoleTextAttribute(hOut, raw_color);
}

void con_use_color(int _use_color)
{
    if (!is_console_init)
        con_reset();
    use_color = _use_color;
}

/* 获取当前光标的坐标
 */
void con_get_pos(int *x, int *y)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;

    if (!is_console_init)
        con_reset();
    GetConsoleScreenBufferInfo(hOut, &bInfo);
    *x = (bInfo.dwCursorPosition.X - origin.X) / axis.X;
    *y = (bInfo.dwCursorPosition.Y - origin.Y) / axis.Y;
}

/* 获取逻辑坐标原点的绝对坐标位置
 * (x, y)为逻辑坐标原点的绝对坐标
 * 默认的原点为(0, 0)
 */
void con_get_origin(int *x, int *y)
{
    if (!is_console_init)
        con_reset();
    *x = origin.X;
    *y = origin.Y;
}

/* 获取逻辑x, y坐标轴的单位长度和方向
 * dx, dy分别表示x, y坐标轴的单位长度
 * 如果值为负，表示逻辑坐标的方向与绝对坐标方向相反
 * 默认的值为(1, 1)
 */
void con_get_axis(int *dx, int *dy)
{
    if (!is_console_init)
        con_reset();
    *dx = axis.X;
    *dy = axis.Y;
}

/* 获取屏幕缓冲区的大小
 */
void con_get_buf_size(int *w, int *h)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;

    if (!is_console_init)
        con_reset();
    GetConsoleScreenBufferInfo(hOut, &bInfo);
    *w = bInfo.dwSize.X;
    *h = bInfo.dwSize.Y;
}

/* 移动光标到逻辑坐标(x, y)处
 * 此函数调用之后，程序的便从(x, y)处开始输出
 */
void con_set_pos(int x, int y)
{
    COORD coord;

    if (!is_console_init)
        con_reset();
    coord.X = origin.X + x * axis.X;
    coord.Y = origin.Y + y * axis.Y;
    SetConsoleCursorPosition(hOut, coord);
}

/* 设置当前屏幕所设置的前景色为color
 */
void con_set_fcolor(color_t color)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;

    if (!is_console_init)
        con_reset();
    if (use_color) {
        GetConsoleScreenBufferInfo(hOut, &bInfo);
        SetConsoleTextAttribute(hOut, (bInfo.wAttributes & 0xf0) | (color & 0x0f));
    }
}

/* 设置当前屏幕所设置的背景色为color
 */
void con_set_bcolor(color_t color)
{
    CONSOLE_SCREEN_BUFFER_INFO bInfo;

    if (!is_console_init)
        con_reset();
    if (use_color) {
        GetConsoleScreenBufferInfo(hOut, &bInfo);
        SetConsoleTextAttribute(hOut, (bInfo.wAttributes & 0x0f) | (color << 4));
    }
}

/* 设置屏幕缓冲区的大小
 */
void con_set_buf_size(int width, int height)
{
    COORD coord = {width, height};
    if (!is_console_init)
        con_reset();
    SetConsoleScreenBufferSize(hOut, coord);
}

/* 设置逻辑坐标原点的绝对坐标位置
 * (x, y)为逻辑坐标原点的绝对坐标
 * 默认的原点为(0, 0)
 */
void con_set_origin(int x, int y)
{
    if (!is_console_init)
        con_reset();
    origin.X = x;
    origin.Y = y;
}

/* 设置逻辑x, y坐标轴的单位长度和方向(可以是负数)
 * dx, dy分别表示x, y坐标轴的单位长度
 * 如果值为负，表示逻辑坐标的方向与绝对坐标方向相反
 * 默认的值为(1, 1)
 */
void con_set_axis(int dx, int dy)
{
    if (!is_console_init)
        con_reset();
    axis.X = dx;
    axis.Y = dy;
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
 * 如果不想等待，你可以先调用函数have_key()判断当前是否有按键，如果有才调用此函数
 * 当调用此函数时，屏幕上不会有任何显示
 */
int con_get_key(int _case)
{
    int key;

    if (!is_console_init)
        con_reset();

//  while (1) {
    key = _getch();        /* 获取按键 */
    
    if (key == 224) {      /* 方向键 */
        switch (_getch()) {
        case 72: return KEY_UP;
        case 75: return KEY_LEFT;
        case 77: return KEY_RIGHT;
        case 80: return KEY_DOWN;
        }
    } else if (key) {              /* 其他键 */
        if (_case)
            return toupper(key);  /* 如果忽略大小写，对于字母，都转为大写 */
        else
            return key;
    }
//}                                 /* 如果得到0值，认为是无效按键，重新获取 */
    return key;
}

/* 使程序暂停下来，显示提示字符串str后，按下任意键后继续
 * 如果str为NULL则显示系统默认的字符，一般是： 请按任意键继续. . .
 * 如果什么也不想显示，str可以为空串""
 */
void con_pause(const char *prompt)
{
    if (!is_console_init)
        con_reset();
    if (prompt == NULL)
        system("pause");
    else {
        printf(prompt);
        system("pause>nul");
    }
}

/* 休眠ms毫秒
 */
void con_sleep(int ms)
{
    Sleep(ms);
}

/* 清屏
 * 清除屏幕上显示的所有文字，并将光标移动至窗口坐标原点
 */
void con_clear(void)
{
    if (!is_console_init)
        con_reset();
    system("cls");
}

void con_set_cur_visible(int visible)
{
    CONSOLE_CURSOR_INFO cur_info = {1, 0};
    GetConsoleCursorInfo(hOut, &cur_info);
    cur_info.bVisible = visible;
    SetConsoleCursorInfo(hOut, &cur_info);
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
 *    由于windows终端采用GBK编码，此代码将UTF-8转为GBK
 *    而其他系统（如Linux）终端不一定是GBK编码，此函数的功能便不一致了
 *    为了可移植，如果你需要UTF-8与终端交互，始终考虑此函数，其他情况均不要使用此函数
 */
char *con_utf2con(char *con_string, const char *utf8_string)
{
    wchar_t *unicodeStr = NULL;
    int nRetLen = 0;
    nRetLen = MultiByteToWideChar(CP_UTF8, 0, utf8_string, -1, NULL, 0);
    unicodeStr = (wchar_t *)malloc(nRetLen * sizeof(wchar_t));
    nRetLen = MultiByteToWideChar(CP_UTF8, 0, utf8_string, -1, unicodeStr, nRetLen);
    nRetLen = WideCharToMultiByte(CP_ACP, 0, unicodeStr, -1, NULL, 0, NULL, 0);
    nRetLen = WideCharToMultiByte(CP_ACP, 0, unicodeStr, -1, con_string, nRetLen, NULL, 0);
    free(unicodeStr);
    return con_string;
}

/* 将终端字符串转化为UTF-8字符串
 * 可用于在控制台中显示UTF-8编码的文件的内容
 * 注意：
 *    由于windows终端采用GBK编码，此代码将GBK转为UTF-8
 *    而其他系统（如Linux）终端不一定是GBK编码，此函数的功能便不一致了
 *    为了可移植，如果你需要UTF-8与终端交互，始终考虑此函数，其他情况均不要使用此函数
 */
char *con_con2utf(char *utf8_string, const char *con_string)
{
    int len = MultiByteToWideChar(CP_ACP, 0, con_string, -1, NULL, 0);
    wchar_t* wstr = malloc(sizeof(wchar_t) * (len + 1));
    MultiByteToWideChar(CP_ACP, 0, con_string, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, utf8_string, len, NULL, NULL);
    free(wstr);
    return utf8_string;
}
