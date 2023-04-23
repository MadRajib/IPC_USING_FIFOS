#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SERVER_FIFO "/tmp/fifo_server"
#define MAX_BUF_SIZE 4096

typedef struct{
    char base[80];
    char path[80];
}url_t;

typedef struct {
    char client_fifo[80];
    url_t url;
}pkt_t;

void read_file(char *file_path, char *buf, size_t buf_len) {
    int fd, ret;

    if((fd = open(file_path, O_RDONLY)) == -1) {
        fprintf(stderr, "not able to open the file error: %d", errno);
        return;
    }

    ret = read(fd, buf, buf_len);
    if (ret <= 0) {
        fprintf(stderr, "not able to read from file %s error: %d", file_path, errno);
        goto ERROR;
    }

ERROR:
    close(fd);
    return;
}

void replace_buf(char *buf, const char * name) {
    memset(buf + 6, 0, 80);
    memcpy(buf + 6, name, strlen(name));
}

void process_request(char *buf, int fd) {
    char *str, *token, *saveptr;
    int i = 0, ret;
    pkt_t req;

    memset(&req, 0, sizeof(pkt_t));

    for (str = buf; ; str = NULL) {
        token = strtok_r(str, ";", &saveptr);
        if (token == NULL)
            break;

        if (i == 0) {        
            memcpy(&req.client_fifo, token, strlen(token));
            req.client_fifo[strlen(token)] = '\0';
        } else if(i == 1 ) {
            int j = 0; 
            for (str = token; ; str = NULL) {
                token = strtok_r(str, "/", &saveptr);
                if (token == NULL)
                    break;
                if( j == 0) {
                    memcpy(&req.url.base, token, strlen(token)); /*why -1 ? since \n is added to the end of the string*/
                    if(req.url.base[strlen(token) - 1] == '\n') {
                        req.url.base[strlen(token) -1] = '\0';
                    }
                    req.url.base[strlen(token)] = '\0';

                }else {
                    memcpy(&req.url.path, token, strlen(token)); /*why -1 ? since \n is added to the end of the string*/
                    if(req.url.path[strlen(token) - 1] == '\n') {
                        req.url.path[strlen(token) -1] = '\0';
                    }
                    req.url.path[strlen(token)] = '\0';
                }

                j++;
            }
        }

        i++;
    }

    printf("fifo_path:%s req_url:%s/%s\n",req.client_fifo, req.url.base, req.url.path);
    char rbuf[MAX_BUF_SIZE];

    read_file(req.url.base, rbuf, MAX_BUF_SIZE);
    if(strlen(req.url.path) > 1)
        replace_buf(rbuf, req.url.path);

    int wfd = open(req.client_fifo, O_WRONLY);
    if((ret = write(wfd, rbuf, strlen(rbuf)))== -1) {
        fprintf(stderr, "error writing to the fd error:%d", errno);
    } else {
        printf("Response sent!\n");
    }
    close(wfd);
   /* 

    */
}

int main (int argc, char **argv)
{
    int fd, bytes_read;
    char buf [MAX_BUF_SIZE];


    if ((mkfifo(SERVER_FIFO, 0664) == -1) && (errno != EEXIST)) {
        fprintf(stderr, "unable to create fifo error: %d\n", errno);
        exit (1);
    }

    if ((fd = open(SERVER_FIFO, O_RDONLY)) == -1)
        fprintf(stderr, "failed to open fifo %d\n", errno);

    printf("reader\n");

    for(;;) {
        
        memset (buf, '\0', sizeof(buf));  /*initialize buff to zeros*/
    
        if ((bytes_read = read (fd, buf, sizeof (buf))) == -1) /*start reading from the fifo*/
             fprintf(stderr, "unable to read from fifo erro %d\n", errno);

        /*re read if zero bytes read!*/
        if (bytes_read == 0)
             continue;

        buf[bytes_read] = '\0';
        printf("Server: read %d bytes: \"%s\"\n", bytes_read, buf);
        process_request(buf, fd); 
     }
ERROR:
    close(fd);
    unlink(SERVER_FIFO);

}
