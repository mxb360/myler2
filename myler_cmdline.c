#include "myler_cmdline.h"
#include "myler_utils.h"
#include "console.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define IS_SHORT_OPT(str)  ((str) && (str)[0] == '-' && (str)[1] && !(str)[2])
//#define IS_LONG_OPT(str)   ((str) && (str)[0] == '-' && (str)[1] == '-' && (str)[2])
#define IS_LONG_OPT(str)   ((str) && (str)[0] == '-' && (str)[1] && (str)[2])

static void myler_cmdline_print_help(void)
{
    con_set_fcolor(LYELLOW);
    fprintf(myler_stdout, "Myler Player 多功能音乐播放器 V2.0\n\n");
    con_reset_color();
    fprintf(myler_stdout,
        "用法：myler [选项...] [文件...]\n\n"
        "文件:\n"
        "   指的是音乐(MP3)文件，支持多个文件、通配符、相对路径、绝对路径；\n"
        "   添加的文件将会被添加到临时列表中，文件名可重复，自动忽略非MP3文件。\n\n"
        "选项:\n"
        "  -h, --help              显示此帮助信息后退出。\n"
        "  -V, --version           显示版本信息后退出。\n"
        "  -E, --exit              列表播放完成后退出。\n"
        "  -m, --mini              迷你版界面（仅含进度状态窗口）。\n"
        "  -v, --volume VOL        设置播放音量为VOL，VOL取值(0-100)。\n"
        "  -O, --order             设置播放模式为：顺序播放。\n"
        "  -L, --loop              设置播放模式为：列表循环。\n"
        "  -R, --repeat            设置播放模式为：单曲循环。\n"
        "  -S, --shuffle           设置播放模式为：随机播放。\n" 
        "  --stop                  进入播放器后暂停音乐的播放。\n"
        "  -w, --worlds <words>    设置搜索关键字。\n"
        "  -e, --engine <engine>   设置搜索引擎(Netease: 网易云 QQ: QQ音乐 KuGou: 酷狗音乐 Local: 本地音乐)。\n"
        "  -t, --type <type>       设置搜索类型(Song: 歌曲 List：歌单 Album：专辑 Lyric：歌词。\n"
        "  -s, --search            立即进入搜索界面并开始搜索。\n"
        "  --no-color              禁用界面、输出提示颜色。\n");

    fprintf(myler_stdout, " \n注意：\n  如果选项有冲突，程序以后给出的选项为准。\n  带*的选项暂未实现。\n");
    fprintf(myler_stdout, " \n示例：\n");
    con_set_fcolor(LGREEN);
    fprintf(myler_stdout, "  myler -v 20 --mini --exit hello.mp3\n");
    con_reset_color();
    fprintf(myler_stdout, "  以迷你界面播放hello.mp3，设置音量为20%%并在播放完成之后退出。\n");
}

static void myler_cmdline_print_version(void)
{
    con_set_fcolor(LYELLOW);
    fprintf(myler_stdout, "Myler Player 多功能音乐播放器 V2.0\n");
    con_reset_color();
}

void myler_cmdline_resolve(myler_cmdline_t *cmdline)
{
    cmdline->music_name_count = 0;

    const char *unknown_opt = NULL;
    const char *invalid_arg = NULL;

    #define IS_STR(str1, str2)        !strcmp((str1), cmdline->argv[i]) || !strcmp((str2), cmdline->argv[i]) 
    #define GET_INT_ARG(value, a, b)  (i + 1 < cmdline->argc && sscanf(cmdline->argv[i + 1], "%d", &(value)) == 1 \
                                       && cmdline->volume >= (a) && cmdline->volume <= (b))
    #define GET_STR_ARG(value)        (i + 1 < cmdline->argc && (value = cmdline->argv[i + 1]))
    
    for (int i = 1; i < cmdline->argc; i++) {
        if (IS_STR("-h", "--help")) {
            cmdline->have_help = true;
            myler_cmdline_print_help();
            exit(0);
        } else if (IS_STR("-V", "--version")) {
            cmdline->have_version = true;
            myler_cmdline_print_version();
            exit(0);
        } else if (IS_STR("-E", "--exit"))
            cmdline->have_exit = true;
        else if (IS_STR("-m", "--mini"))
            cmdline->have_mini = true;
        else if (IS_STR("-v", "--volume")) {
            if (!GET_INT_ARG(cmdline->volume, 0, 100) && !invalid_arg) 
                invalid_arg = cmdline->argv[i];
        } else if (IS_STR("", "--stop"))
            cmdline->have_stop = true;
        else if (IS_STR("-O", "--order"))
            cmdline->have_order = true, cmdline->have_loop = cmdline->have_repeat = cmdline->have_shuffle = false;
        else if (IS_STR("-L", "--loop"))
            cmdline->have_loop = true, cmdline->have_order = cmdline->have_repeat = cmdline->have_shuffle = false;
        else if (IS_STR("-R", "--repeat"))
            cmdline->have_repeat = true, cmdline->have_loop = cmdline->have_order = cmdline->have_shuffle = false;
        else if (IS_STR("-S", "--shuffle"))
            cmdline->have_shuffle = true, cmdline->have_loop = cmdline->have_repeat = cmdline->have_order = false;
        else if (IS_STR("-w", "--words")) {
            if (!GET_STR_ARG(cmdline->words)) 
                 invalid_arg = cmdline->argv[i];
        } else if (IS_STR("-s", "--search"))
            cmdline->have_search = true;
        else if (IS_STR("", "--no-color"))
            con_use_color(false);
        
        else if ((IS_SHORT_OPT(cmdline->argv[i]) || IS_LONG_OPT(cmdline->argv[i])) && !unknown_opt) 
            unknown_opt = cmdline->argv[i];
        else
            cmdline->music_name[cmdline->music_name_count++] = cmdline->argv[i];
    }
    if (unknown_opt) {
        myler_print_error(true, "无法识别的选项“%s”", unknown_opt);
        exit(1);
    } else if (invalid_arg) {
        myler_print_error(true, "选项“%s”参数非法", invalid_arg);
        exit(1);
    }
}

void myler_cmdline_init(myler_cmdline_t *cmdline, int argc, char **argv)
{
    myler_assert(cmdline != NULL);
    memset(cmdline, 0, sizeof(myler_cmdline_t));
    cmdline->argc = argc;
    cmdline->argv = argv;
    cmdline->volume = -1;

    myler_cmdline_resolve(cmdline);
}
