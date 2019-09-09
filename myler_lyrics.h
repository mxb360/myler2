#ifndef __MYLER_LYRICS_H__
#define __MYLER_LYRICS_H__

#define MAX_LYRIC_LINE   1000
#define MAX_LYRIC_TIME   0xffffffff

typedef struct myler_lyrics_words_t {
    unsigned long time;
    const char *words;
} myler_lyrics_words_t;

typedef struct myler_lyrics_t {
    const char *file_name;
    myler_lyrics_words_t *lyrics;
    int  lyrics_cnt;
    int currect_lyrics;
    int  offset;
    char **words;
    int  words_cnt;
} myler_lyrics_t;

myler_lyrics_t *myler_lyrics_get_lyrics_by_file(const char *file);
void  myler_lyrics_print(myler_lyrics_t *lyrics);
void  myler_lyrics_free(myler_lyrics_t *lyrics);
int   myler_lyrics_set_current_time(myler_lyrics_t *lyrics, unsigned long current_time);
const char *myler_lyrics_get_lyrics(myler_lyrics_t *lyrics, int line);

#endif  /* !__MYLER_LYRICS_H__ */
