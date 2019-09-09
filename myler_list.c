#include <string.h>
#include <stdlib.h>

#include "myler_list.h"
#include "myler_utils.h"


static void get_name_and_singer(const char *file_name, char *name, char *singer, int mode)
{
    int name_pos = 0, singer_pos = 0;

    const char *point = strrchr(file_name, '.');
    if (mode) {
        char *p = singer;
        singer = name;
        name = p;
    }

    while (*file_name == ' ')
        file_name++;
    while (*file_name && *file_name != '-' && file_name != point)
        name[name_pos++] = *file_name++;
    while (name_pos > 0 && name[name_pos - 1] == ' ')
        name_pos--;

    if (*file_name == '-')
        file_name++;

    while (*file_name == ' ')
        file_name++;
    while (file_name != point)
        singer[singer_pos++] = *file_name++;
    while (singer_pos && singer[singer_pos - 1] == ' ')
        singer_pos--;

    if (name_pos)
        name[name_pos] = 0;
    else
        strcpy(name, "<未知歌名>");
    if (singer_pos)
        singer[singer_pos] = 0;
    else
        strcpy(singer, "<未知歌手>");
}

static const char *get_file_name(const char *path)
{
    int i = strlen(path);

    while (--i >= 0 && path[i] != '\\' && path[i] != '/');
    return i ? path + i + 1 : path;
}

myler_list_t *myler_list_create(const char *list_name)
{
    myler_list_t *list = (myler_list_t *)malloc(sizeof(myler_list_t));
    if (list) {
        strcpy(list->list_name, list_name);
        list->head = list->tail = NULL;
        list->music_count = 0;
        list->current = NULL;
        list->is_open = false;
        list->play_status = MUSIC_NOT_READY;
        list->is_play_end = true;
        list->replay = false;
        list->current_time = 0;
        list->is_expand = true;
    }
    return list;
}

void myler_list_free(myler_list_t *list)
{
    if (list == NULL)
        return;

    myler_list_node_t *node, *node2;
    node = list->head;
    for (int i = 0; i < list->music_count; i++) {
        music_free(node->music);
        myler_lyrics_free(node->lyrics);
        node2 = node;
        node = node->next;
        free(node2);
    }
}

void myler_list_add_node(myler_list_t *list, myler_list_node_t *node)
{
    myler_assert(list);
    myler_assert(node);

    node->list = list;
    node->next = NULL;
    node->prev = list->tail;

    if (!list->head)
        list->head = node;
    if (list->tail)
        list->tail->next = node;
    list->tail = node;

    list->music_count++;
}

myler_list_node_t *myler_list_add_loacl_music(myler_list_t *list, const char *file_path)
{
    myler_assert(list);
    myler_list_node_t *node;

    if (!(node = (myler_list_node_t *)malloc(sizeof(myler_list_node_t)))) 
        return NULL;

    node->music = NULL;
    node->length = -1;
    node->lyrics = NULL;
    node->id[0] = 0;
    get_name_and_singer(get_file_name(file_path), node->name, node->singer, 0);
    strcpy(node->path, file_path);
    myler_list_add_node(list, node);

    return node;
}

void myler_list_clear_current(myler_list_t *list)
{
    myler_assert(list);
    if (list->current && list->current->music) {
        music_free(list->current->music);
        myler_lyrics_free(list->current->lyrics);
        list->current->music = NULL;
        list->is_open = false;
        list->is_play_end = false;
    }
}

myler_list_node_t *myler_list_set_current(myler_list_t *list, int index)
{
    myler_assert(list);
    myler_assert(index >= 0 && index < list->music_count);

    myler_list_node_t *node = list->head;
    myler_list_clear_current(list);

    for (int i = 0; i < index; i++)
        node = node->next;

    list->current = node;
    list->is_play_end = false;
    list->replay = true;

    return node;
}

/* 生成音乐文件名对应的歌词文件名 */
static char *get_lyric_file_name(char *file_name)
{
    int len = strlen(file_name);
    while (--len > 0 && file_name[len] != '.');
    if (len == 0)
        return "";
    strcpy(file_name + len, ".lrc");
    return file_name;
}

int myler_list_open(myler_list_t *list)
{
    myler_assert(list);

    if (!list->current)
        return -1;
    if (!(list->current->music = music_create(list->current->path))) {
        return -2;
    }

    char buf[MAX_STR_BUF];
    list->is_open = true;
    list->is_play_end = false;
    list->current->length = music_get_length(list->current->music);
    list->current->lyrics = myler_lyrics_get_lyrics_by_file(get_lyric_file_name(strcpy(buf, list->current->path)));

    return 0;
}

int myler_list_play(myler_list_t *list)
{
    myler_assert(list);

    if (!list->current || !list->is_open)
        return -1;
    list->is_play_end = false;

    if (music_play(list->current->music, 0, 0, 0))
        return -2;
    return 0;
}

int myler_list_pasue(myler_list_t *list)
{
    myler_assert(list);

    if (!list->current || !list->is_open)
        return -1;
    return music_pause(list->current->music);
}

int myler_list_resume(myler_list_t *list)
{
    myler_assert(list);

    if (!list->current || !list->is_open)
        return -1;
    return music_resume(list->current->music);
}

int myler_list_set_next(myler_list_t *list, int play_mode)
{
    myler_assert(list);

    if (!list->current)
        return -1;

    list->replay = true;
    myler_list_clear_current(list);

    if (play_mode == PlayRepeatOne) 
        return 0;
    else if (list->current == list->tail && play_mode == PlayInOrder)
        list->is_play_end = true, list->replay = false;
    else if (list->current == list->tail && play_mode == PlayListLoop)
        list->current = list->head;
    else if (list->current != list->tail && play_mode != PlayShuffle) 
        list->current = list->current->next;
    else if (play_mode == PlayShuffle)
        myler_list_set_current(list, rand() % list->music_count);
    return 0;
}

int myler_list_set_prev(myler_list_t *list)
{
    myler_assert(list);

    if (!list->current)
        return -1;
    myler_list_clear_current(list);
    if (list->is_play_end)
        list->current = list->tail;
    else if (list->current != list->head)
        list->current = list->current->prev;
    list->replay = true;
    return 0;
}

void myler_list_set_list(myler_list_display_t *list_display, myler_list_t *lists[], int list_count)
{
    myler_assert(list_display);
    myler_assert(lists);

    myler_list_display_node_t *last_node = NULL;

    for (int line = 0; line < list_count; line++) {
        myler_list_display_node_t *node = (myler_list_display_node_t *)malloc(sizeof(struct myler_list_display_node_t));
        if (!node)
            return;
        
        node->list = lists[line];
        node->type = IsListType;
        node->prev = last_node;
        node->music = NULL;
        node->str1 = lists[line]->list_name;
        node->str2 = NULL;
        if (last_node)
            last_node->next = node;
        else
            list_display->head = node;

        last_node = node;
        list_display->display_list_count++;
        if (lists[line]->is_expand) {
            myler_list_node_t *music = lists[line]->head;
            for (int count = 0; count < lists[line]->music_count; count++) {
                myler_list_display_node_t *node = (myler_list_display_node_t *)malloc(sizeof(struct myler_list_display_node_t));
                if (!node)
                    return;
                
                node->list = lists[line];
                node->type = IsListNodeType;
                node->prev = last_node;
                node->music = music;
                node->str1 = music->name;
                node->str2 = music->singer;
                last_node->next = node;

                last_node = node;
                music = music->next;
                list_display->display_list_count++;
            }
        }
    }
    list_display->display_list = list_display->head;
    last_node->next = NULL;
    list_display->tail = last_node;
    list_display->current_choice = list_display->head;
    list_display->default_choice = NULL;
}

void myler_list_display_free(myler_list_display_t *list_display)
{
    if (list_display == NULL)
        return;
    myler_list_display_node_t *node = list_display->head;
    while (node)
    {
        myler_list_display_node_t *node2 = node;
        node = node->next;
        free(node2);
    }
}
