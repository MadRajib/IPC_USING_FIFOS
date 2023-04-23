#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define SERVER_FIFO "/tmp/fifo_server"
#define CLIENT_FIFO "/tmp/fifo_client"
#define MAX_BUF_SIZE 4096


int main (int argc, char **argv)
{
    int wfd, rfd, ret, bytes_read;
    char wbuf [MAX_BUF_SIZE];
    char rbuf [MAX_BUF_SIZE];
    int len = strlen(CLIENT_FIFO);

    memcpy(wbuf, CLIENT_FIFO, len); 
    memcpy(wbuf + len, ";", 1);
    len++;


    if ((mkfifo(CLIENT_FIFO, 0664) == -1) && (errno != EEXIST)) {
        fprintf(stderr, "unable to create fifo error: %d\n", errno);
        exit (1);
    }

    if ((wfd = open(SERVER_FIFO, O_WRONLY)) == -1)
        fprintf(stderr, "failed to open server fifo %d\n", errno);

    while (fgets(wbuf+len, sizeof(wbuf) -len, stdin), !feof(stdin)) {
            printf("%s\n", wbuf);
            if ((ret = write(wfd, wbuf, strlen(wbuf))) == -1) {
		fprintf(stderr, "write error :%d\n", errno);
            } else{
                
                memset (rbuf, '\0', sizeof(rbuf));  /*initialize buff to zeros*/

                if ((rfd = open(CLIENT_FIFO, O_RDONLY)) == -1) {
                    fprintf(stderr, "failed to open fifo %d\n", errno);
                    continue;
                }
    
                if ((bytes_read = read (rfd, rbuf, sizeof (rbuf))) == -1) /*start reading from the fifo*/
                    fprintf(stderr, "unable to read from fifo erro %d\n", errno);

                 /*re read if zero bytes read!*/
                if (bytes_read == 0)
                    continue;

                rbuf[bytes_read] = '\0';
                    printf("Server: %d bytes: \"%s\"\n", bytes_read, rbuf);
            }
     }
ERROR:
    close(wfd);
    close(rfd);
    unlink(SERVER_FIFO);

}
