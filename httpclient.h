#ifndef __SOCKET_H__
#define __SOCKET_H__

typedef struct _HttpClient {
    int header_length;
    int body_length;
    int is_length_ok;

    char *header;
    char *body;
    char *status_msg;

    int status_code;
    int error_code;
} http_client_t;

typedef void(*HttpProessFunc)(int proess, void *arg);

int http_client_init(void);

const char *http_client_get_last_error(void);

char *http_client_url_encode(char *result, const char *str);

int http_client_get(http_client_t *client, const char *url, HttpProessFunc func, void *arg);

void http_client_free(http_client_t * client);

int http_client_save_file(http_client_t *client, const char *file_name);

#endif
