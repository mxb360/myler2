#ifndef __MUSIC_H__
#define __MUSIC_H__

#define MUSIC_NOT_READY  0
#define MUSIC_PAUSED     1
#define MUSIC_PLAYING    2
#define MUSIC_STOPPED    3

#define MUSIC_ERROR     -1
#define MUSIC_OK         0

#ifdef __cplusplus
extern "C" {
#endif 

typedef unsigned long mlen_t;
typedef struct _music_t music_t;

music_t *music_create(const char *file_name);

int music_play(music_t *music, mlen_t start, mlen_t end, int repeat);

void music_free(music_t *music);

int music_pause(music_t *music);

int music_resume(music_t* music);

int music_stop(music_t *music);

mlen_t music_get_length(music_t *music);

mlen_t music_get_current_length(music_t *music);

int music_set_volume(music_t *music, int volume);

int music_get_volume(music_t *music);

int music_get_status(music_t *music);

const char *music_get_file_name(music_t *music);

const char *music_get_last_error(void);

#ifdef __cplusplus
}
#endif 

#endif  /* !__MUSIC_H__ */
