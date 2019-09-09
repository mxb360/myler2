#include <signal.h>
#include <stdlib.h>

#include "myler.h"
#include "myler_cmdline.h"

static bool break_loop = false;

// Ctrl-C 退出
void break_by_ctrl_c(int sig)
{
    break_loop = true;
    signal(SIGINT, break_by_ctrl_c);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, break_by_ctrl_c);

    myler_player_t player;
    myler_cmdline_t cmd_line;

    /* 解析命令行
     * 如果命令行解析失败，程序会直接退出
     */
    myler_cmdline_init(&cmd_line, argc, argv);
    
    // 初始化播放器
    if (myler_init(&player, &cmd_line))
        return -1;

    // 循环更新播放器状态
    while (!break_loop) {
        myler_update(&player);
        con_sleep(100);
    }

    // 退出播放器
    myler_quit(&player, false);
    return 0;
}
