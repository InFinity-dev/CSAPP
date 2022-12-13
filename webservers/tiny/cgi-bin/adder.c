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
        /* 책에 나온 코드는 테스트 용으로 GET 방식으로 넘겨줄때 변수에 대한 이름을 지정해 주지 않았지만
         * home.html에 form 을 통한 인자 전달시 name항목이 들어가게 되므로 코드 변경이 필요하다
         * http://127.0.0.1:8000/cgi-bin/adder?1&2*/

//        strcpy(arg1, buf);
//        strcpy(arg2, p+1);
//        n1 = atoi(arg1); //atoi 함수 : 문자열로 받아온 정수를 int형으로 변환(문자의 경우 0 리턴)
//        n2 = atoi(arg2);

        /* form 내부에 arg1, arg2 name으로 변수를 입력받아 GET Method로 /cgi-bin/adder 로 넘겨준다
         * 전달되는 URL형태는 http://127.0.0.1:8000/cgi-bin/adder?arg1=1&arg2=2 와 같은 형태
         * sscanf 통해 각각 n1, n2로 받아올수 있다.
         * <form action="/cgi-bin/adder" method="get" target="_blank">
            <label>Adder argument 1:</label>
            <input type="text" name="arg1"><br><br>
            <label>Adder argument 1:</label>
            <input type="text" name="arg2"><br><br>
            <input type="submit" value="Submit">
        </form>*/

        sscanf(buf, "arg1=%d", &n1);
        sscanf(p+1, "arg2=%d", &n2);

    }

    sprintf(content, "QUERY_STRING=%s", buf);
    sprintf(content, "Welcome to add.com: ");
    sprintf(content, "%sTHE Internet addition portal. \r\n<p>", content);
    sprintf(content, "%sThe answer is %d + %d = %d\r\n<p>", content, n1, n2, n1 + n2);
    sprintf(content, "%sThanks for visiting!\r\n", content);

    printf("Connection : close\r\n");
    printf("Content-length : %d\r\n", (int)strlen(content));
    printf("Content-type : text/html\r\n\r\n");
    printf("%s",content);
    fflush(stdout); //stdout 스트림 버퍼 비워주기

    exit(0);
}