#include "httpclient.h"

#include <winsock2.h>
#include <Ws2tcpip.h>

int inet_pton(int family, const char *strptr, void *addrptr);
const char *inet_ntop(int family, const void *addrptr, char *strptr, size_t len);  

WSADATA wsaData;
typedef SOCKET socket_t;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <ctype.h>

#define HTTP_POST "POST /%s HTTP/1.1\r\nHOST: %s:%d\r\nAccept: */*\r\n" \
                  "Content-Type:application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s"
#define HTTP_GET  "GET /%s HTTP/1.1\r\nHOST: %s\r\nConnection: close\r\nAccept: */*\r\n\r\n"

static char str_err_buf[1024];

/* 设置当前错误
 * errstr: 错误描述
 */
static int set_error(const char *errstr)
{
    int err_code, len;

    err_code = WSAGetLastError();
    sprintf(str_err_buf, "%s: ", errstr);
    len = strlen(str_err_buf);
    FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL, err_code, 0, str_err_buf + len, (sizeof str_err_buf) - len, NULL);
    return err_code;
}

int http_client_init(void)
{
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) 
        return set_error("WindowsWSAStartupError");
    return 0;
}

socket_t socket_create_client(const char *ipstr, unsigned int port)
{
    socket_t _socket;
    char _ipstr[20];              
    struct sockaddr_in _addr;

    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) 
        return -set_error("SocketClientCreateError");
    /* 绑定 */
    memset(&(_addr), 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, ipstr, &_addr.sin_addr.s_addr) <= 0) 
        return -set_error("SocketClinetGetIPError");

    strcpy(_ipstr, ipstr);
    _addr.sin_port = htons(port);
    port = port;
    inet_ntop(AF_INET, &_addr.sin_addr.s_addr, _ipstr, sizeof _ipstr);
    /* 连接 */
    if (connect(_socket, (struct sockaddr *)&_addr, sizeof(_addr)) == -1) 
        return -set_error("SocketClientConnectError");

    return _socket;
}

static char *get_host(char *host_name)
{
    int n = 0;
    if (!strncmp(host_name, "http://", 7))
        n = 7;
    if (!strncmp(host_name, "https://", 8))
        n = 8;
    if (n)
        strcpy(host_name, host_name + n--);
    while (host_name[++n] && host_name[n] != '/');
    host_name[n] = 0;

    return host_name;
}

static char *get_url(char *host_name)
{
    int n = 0;
    if (!strncmp(host_name, "http://", 7))
        n = 6;
    if (!strncmp(host_name, "https://", 8))
        n = 7;

    while (host_name[++n] && host_name[n] != '/');
    if (!n)
        return NULL;
    if (host_name[n] == '/')
        n++;
    strcpy(host_name, host_name + n); 

    return host_name;
}


int http_client_get(http_client_t *client, const char *url, HttpProessFunc func, void *arg)
{
    char buf[4096];
    char url_buf[512];
    char ip_buf[20];
    char host_name_buf[512];

    struct hostent *_host;
    int i, n, ln = 0;
    char *tph = NULL;
    socket_t _socket;

    memset(client, 0, sizeof(http_client_t));

    /* 解析URL */
    get_url(strcpy(url_buf, url));
    get_host(strcpy(host_name_buf, url));
    if ((_host = gethostbyname(host_name_buf)) == NULL) 
        return client->error_code = set_error("GetHostByNameError");
    inet_ntop(AF_INET, _host->h_addr, ip_buf, sizeof ip_buf);

    /* 创建客户端 */
    if ((_socket = socket_create_client(ip_buf, 80)) < 0) 
        return client->error_code = -_socket;

    #define client_end(code)  (closesocket(_socket), client->error_code = (code))

    sprintf(buf, HTTP_GET, url_buf, host_name_buf);
    if (send(_socket, buf, strlen(buf), 0) < 0) 
        return client_end(set_error("SocketSendError"));

    client->status_code = client->body_length = 0;
    if ((n = recv(_socket, buf, (sizeof buf) - 1, 0)) > 0) {
        client->status_code = (buf[9] - '0') * 100 + (buf[10] - '0') * 10 + buf[11] - '0';
        for (i = 13; buf[i] != '\r'; i++);
        buf[i] = 0;
        client->status_msg = (char *)malloc(i - 5);
        if (client->status_msg)
            strcpy(client->status_msg, buf + 9);
        buf[i] = '\r';
        tph = strstr(buf, "Location: ");
        if (tph) {
            for (i = 0; tph[i] != '\r'; i++);
            tph[i] = 0;
            return http_client_get(client, tph + strlen("Location: "), func, arg);
        }

        tph = strstr(buf, "Content-Length: ");
        if (tph) {
            tph += strlen("Content-Length: ");
            while (isdigit(*tph))
                client->body_length = client->body_length * 10 + *tph++ - '0';
        }
        tph = strstr(buf, "\r\n\r\n");
        if (tph) {
            *tph = 0;
            ln = tph - buf;
            if ((client->header = (char *)malloc(ln + 10)))
                strcpy(client->header, buf);
        }
    } else {
        free(client->header);
        closesocket(_socket);
        return client_end(set_error("SocketSendError"));
    }

    if (client->body_length) {
        ln += 4;
        client->body = (char *)malloc(client->body_length + 10);
        if (client->body) 
            memcpy(client->body, tph + 4, n - ln);
        else 
            return client_end(0);
        ln = n - ln;
    } else {
        client->body = NULL;
        return client_end(0);
    }

    while ((n = recv(_socket, buf, (sizeof buf) - 1, 0)) > 0) {
        memcpy(client->body + ln, buf, n);
        ln += n;
        if (func)
            func((int)(100. * ln / client->body_length), arg);
    }
    client->body[client->body_length] = 0;
    client->is_length_ok = (ln == client->body_length);
    return client_end(0);
}

int http_client_save_file(http_client_t *client, const char *file_name)
{

    if (!client || client->error_code || !client->body_length)
        return 1;

    FILE *fp = fopen(file_name, "wb");
    if (!fp)
        return 1;
    if (fwrite(client->body, 1, client->body_length, fp) == 0) {
        fclose(fp);
        return 1;
    }
        
    fclose(fp);
    return 0;
}

void http_client_free(http_client_t *client)
{
    if (client) {
        free(client->body);
        free(client->header);
        free(client->status_msg);
    }
    memset(client, 0, sizeof(http_client_t));
}

char *http_client_url_encode(char *result, const char *str)
{
    int i;
    int j = 0;
    char ch;
    int strSize = strlen(str);

    if ((str == NULL) || (result == NULL)) 
        return 0;

    for (i = 0; i < strSize; ++i) {
        ch = str[i];
        if (isalpha(ch) || isalnum(ch) || ch == '.' || ch == '-' || ch == '_' || ch == '*') 
            result[j++] = ch;
        else if (ch == ' ') 
            result[j++] = '+';
        else {
            sprintf(result + j, "%%%02X", (unsigned char)ch);
            j += 3;
        }
    }

    result[j] = '\0';
    return result;
}

const char *http_client_get_last_error(void)
{
    return str_err_buf;
}
