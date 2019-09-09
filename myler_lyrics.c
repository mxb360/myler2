#include "myler_lyrics.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* 依据时间比较两段歌词的大小，用于对歌词段的排序 */
static int lyrics_words_compare(const void *l1, const void *l2)
{
    return ((myler_lyrics_words_t *)l1)->time - ((myler_lyrics_words_t *)l2)->time;
}

/* 歌词解析器，从文件file中解析歌词保存到MylerLyricsFile中 */
static int Lyrics_Parse(myler_lyrics_t *lyrics, FILE *file)
{
    char buf[256];
    unsigned long time1, time2, time3; /* 歌词文件格式 [time1:time2.time3][...] lyrics */
    int start_time, end_time;

    if (!lyrics || !file)
        return -1;

    lyrics->lyrics_cnt = 1;
    lyrics->currect_lyrics = 0;
    lyrics->lyrics = (myler_lyrics_words_t *)malloc(sizeof(struct myler_lyrics_words_t) * MAX_LYRIC_LINE);
    lyrics->lyrics[0].time = 0;
    lyrics->lyrics[0].words = ""; 
    lyrics->words_cnt = 0;
    lyrics->words = (char **)malloc(sizeof(char *) * MAX_LYRIC_LINE);

    while (fgets(buf, sizeof buf, file) && lyrics->lyrics_cnt <= MAX_LYRIC_LINE) {
        char *pbuf = buf;

        start_time = lyrics->lyrics_cnt;   /* 首个[]对的位置 */
        while (1) {                        /* 解析当前行歌词的时间，如果出错，忽略该行 */
            if (sscanf(pbuf, "[%lu:%lu.%lu]", &time1, &time2, &time3) != 3)
                break;
            pbuf = strchr(pbuf, ']') + 1;
            /* 计算总时长(毫秒) */
            time3 = time3 < 99 ? time3 * 10 : time3;  /* 区别time3的两位情况和三位情况 */
            lyrics->lyrics[lyrics->lyrics_cnt].time = time1 * 60000 + time2 * 1000 + time3;
            lyrics->lyrics_cnt++;
        }

        end_time = lyrics->lyrics_cnt;   /* 最后一个[]对之后的位置 */
        if (start_time < end_time) {     /* 绑定所有的[]对对应的歌词字符串 */
            int i, len;

            len = strlen(pbuf);
            lyrics->words[lyrics->words_cnt] = (char *)malloc(len + 1);
            strcpy(lyrics->words[lyrics->words_cnt], pbuf);
            if (isspace(lyrics->words[lyrics->words_cnt][len - 1]))
                lyrics->words[lyrics->words_cnt][len - 1] = '\0';
            if (isspace(lyrics->words[lyrics->words_cnt][len - 2]))
                lyrics->words[lyrics->words_cnt][len - 2] = '\0';
            for (i = start_time; i < end_time; i++) 
                lyrics->lyrics[i].words = lyrics->words[lyrics->words_cnt];
            lyrics->words_cnt++;
        }
    }

    lyrics->lyrics = realloc(lyrics->lyrics, sizeof(myler_lyrics_words_t) * lyrics->lyrics_cnt);
    lyrics->words = realloc(lyrics->words, sizeof(char *) * lyrics->words_cnt);
    /* 将生成的歌词按时间排序 */
    qsort(lyrics->lyrics, lyrics->lyrics_cnt, sizeof(myler_lyrics_words_t), lyrics_words_compare);

    return 0;
}

/* 释放MylerLyrics所申请的所有内存 */
void myler_lyrics_free(myler_lyrics_t *lyrics)
{
    if (lyrics) {
        int i;
        for (i = 0; i < lyrics->words_cnt; i++) 
            free(lyrics->words[i]);
        free(lyrics->lyrics);
        free(lyrics->words);
        free(lyrics);
    }
}

/* MylerLyrics_GotoCurrentTime
 * 设置当前应该显示的歌词的时间
 * 此函数调用成功后，当前显示的歌词将变为设current_time对应的歌词
 * lyrics:         存放歌词内容的指针
 * current_time： 待设置的时间，通常是当前音乐的播放位置
 * 返回：         成功返回0，出错返回-1 
 */
int myler_lyrics_set_current_time(myler_lyrics_t *lyrics, unsigned long current_time)
{
    if (!lyrics)
        return -1;

    int i = lyrics->currect_lyrics;
    if (lyrics->lyrics[i].time + lyrics->offset < current_time)
        for (; i < lyrics->lyrics_cnt && lyrics->lyrics[i].time + lyrics->offset < current_time; i++);
    else if (lyrics->lyrics[i].time + lyrics->offset > current_time) 
        for (; i > 0 && lyrics->lyrics[i].time + lyrics->offset > current_time; i--);

    lyrics->currect_lyrics = i ? i - 1 : 0;
    return 0;
}

/* myler_lyrics_get_lyrics
 * 返回相对于当前应该显示的歌词的第line行歌词
 * lyrics:   存放歌词内容的指针
 * line:    先对于当前歌词的偏移行数，为0表示当前行，
 *          负数表示前面的行，正数表示后面的行
 * 返回：   对应行的歌词，如果没有对应的行，返回空串（即""）
 *          出错返回空串 
 */
const char *myler_lyrics_get_lyrics(myler_lyrics_t *lyrics, int line)
{
    int _line;
    if (!lyrics)
        return "";
    _line = lyrics->currect_lyrics + line;
    if (_line >= 0 && _line < lyrics->lyrics_cnt)
        return lyrics->lyrics[_line].words;
    return "";
}

/* 从文件中获取歌词对象，会解析文件内容，生成MylerLyrics对象 
 * 失败返回NULL
 */
myler_lyrics_t *myler_lyrics_get_lyrics_by_file(const char *file_name)
{
    myler_lyrics_t *lyrics;
    FILE *file;

    lyrics = (myler_lyrics_t *)malloc(sizeof(struct myler_lyrics_t));
    if (lyrics) {
        lyrics->file_name = file_name;
        lyrics->offset = 0;
        if ((file = fopen(file_name, "r")) == NULL || Lyrics_Parse(lyrics, file)) {
            free(lyrics);
            lyrics = NULL;
        }
        if (file)
            fclose(file);
    }
    return lyrics;
}
