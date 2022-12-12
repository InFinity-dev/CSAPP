//
// Created by InFinity on 2022/12/11.
//

/*echo Client
 * 서버와의 연결을 수립한 이후에 클라이언트는 표준 입력에서 텍스트 줄을 반복해서 읽는 루프에 진ㅇ비하고, 서버에 텍스트 줄을
 * 전송하고, 서버에서 echo줄을 읽어서 그 결과를 표준출력을 인쇄한다, 루프는 fgets가 EOF 표준 입력을 만나면 종료되는데,
 * 그 이유는 사용자가 Ctrl+D를 눌렀거나 파일로 텍스트 줄을 모두 소진했기 때문이다.
 * 루프가 종료한 후에 클라이언트는 식별자를 닫는다. 이렇게 하면 서버로 EOF 라는 통지가 전송되며,
 * 서버는 rio_readlineb 합수에서 리턴 코드 0을 받으면 이 사실을 감지한다. 자신의 식별자를 닫은 후에 클라이언트는 종료한다.
 * 클라이언트의 커널이 프로세스가 종료할 때 자동으로 열었던 모든 식별자들을 닫아주기 때문에 Close(Clientfd)는 불필요하지만
 * 열었던 모든 식별자들을 명시적으로 닫아주는 것이 좋은 프로그래밍 습관이다.
 * */
#include "csapp.h"

int main(int argc, char **argv){
    int clientfd;
    char *host, *port, buf[MAXLINE];
    rio_t rio;

    if (argc !=3){
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = argv[2];

    clientfd= Open_clientfd(host, port);
    Rio_readinitb(&rio, clientfd);

    while (Fgets(buf, MAXLINE, stdin) != NULL)
    {
        Rio_writen(clientfd, buf, strlen(buf));
        Rio_readlineb(&rio, buf, MAXLINE);
        Fputs(buf, stdout);
    }
    Close(clientfd);
    exit(0);
}