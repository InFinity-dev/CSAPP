//
// Created by InFinity on 2022/12/12.
//
/*두개의 정수(n1, n2)를 더하는 웹서버 프로그램*/
#include "csapp.h"

int main (void){
    char *buf, *p;
    char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
    int n1 = 0, n2 = 0;

    if ((buf = getenv("QUERY_STRING")) !=NULL){
        p = strchr(buf,'&');
        *p = '\0';
        strcpy(arg1, buf);
        strcpy(arg2, p+1);
        n1 = atoi(arg1); //atoi 함수 : 문자열로 받아온 정수를 int형으로 변환(문자의 경우 0 리턴)
        n2 = atoi(arg2);
    }

    sprintf(content, "QUERY_STRING=%s", buf);
    sprintf(content, "Welcome to add.com: ");
    sprintf(content, "%sTHE Internet addition portal. \r\n<p>", content);
    sprintf(content, "%sThe answer is : %d + %d = %d\r\n<p>", content, n1, n2, n1 + n2);
    sprintf(content, "%sThanks for visiting!\r\n", content);

    printf("Connection : close\r\n");
    printf("Content-length : %d\r\n", (int)strlen(content));
    printf("Content-type : text/html\r\n\r\n");
    printf("%s",content);
    fflush(stdout); //stdout 스트림 버퍼 비워주기

    exit(0);
}