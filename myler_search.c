#include "myler_utils.h"
#include "myler.h"
#include "myler_search.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int myler_search_get_line(const char *str, char *buf)
{
    static const char *_str = NULL;

    if (str) _str = str;

    char *next_str = strstr(_str, "<br>");
    if (next_str == NULL)
        return -1;
    int index = next_str - _str;

    strncpy(buf, _str, index);
    buf[index] = 0;
    _str += index + 4;

    return index;
}

int myler_search_song_parse(myler_list_t *list, http_client_t *client)
{
    char buf[MAX_STR_BUF];

    int code = 1;
    int cont = -1;
    if (myler_search_get_line(client->body, buf) <= 0 || sscanf(buf, "%d", &code) != 1 || code != 0)
        return -1;
    if (myler_search_get_line(NULL, buf) <= 0 || sscanf(buf, "%d", &cont) != 1 || cont == -1) 
        return -1;
    if (cont == 0)
        return 0;
    list->is_expand = true;
    while (1) {
        int len;

        myler_list_node_t *node = malloc(sizeof (myler_list_node_t));
        if (node == NULL)
            break;
        if ((len = myler_search_get_line(NULL, buf)) <= 0) 
            break;
        strcpy(node->id, buf);
        if ((len = myler_search_get_line(NULL, buf)) <= 0) 
            break;
        con_utf2con(node->name, buf);
        if ((len = myler_search_get_line(NULL, buf)) <= 0) 
            break;
        con_utf2con(node->singer, buf);
        if ((len = myler_search_get_line(NULL, buf)) <= 0) 
            break;
        sscanf(buf, "%d", &node->length);
        node->length *= 1000;
        myler_search_get_line(NULL, buf);

        node->music = NULL;
        node->lyrics = NULL;
        node->path[0] = 0;
        myler_list_add_node(list, node);
    }

    return 0;
}
