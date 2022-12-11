//
// Created by InFinity on 2022/12/12.
//

#include "csapp.h"

void echo(int connfd);

int main(int argc, char **argv){
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE];

    if (argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    listenfd = Open_clientfd(argv[1]);
    while(1){
        clientlen = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); //포인터 타입 캐스팅
        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port,
                    MAXLINE, 0);
        echo(connfd);
        Close(connfd);
    }
    exit(0);
}
