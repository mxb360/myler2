#ifndef __MYLER_SEARCH_H__
#define __MYLER_SEARCH_H__

#include "myler_config.h"
#include "myler_list.h"

#include "httpclient.h"

int myler_search_song_parse(myler_list_t *list, http_client_t *client);

#endif
