//
// Created by InFinity on 2022/12/12.
//
//텍스트 줄을 읽고 echo해주는 echo 함수
#include "csapp.h"

void echo(int connfd)
{
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0)
    {
        printf("server received %d bytes\n", (int)n);
        Rio_writen(connfd, buf, n);
    }
}