//
// Created by InFinity on 2022/12/10.
//
// ex) ./hostinfo twitter.com

#include "csapp.h"

int main(int argc, char **argv){
    struct addrinfo *p, *listp, hints;
    char buf[MAXLINE];
    int rc, flags;

    if (argc !=2){
        fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
        exit(0);
    }

    /* addrinfo record 리스트 가져오기 */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4 Only
    hints.ai_socktype = SOCK_STREAM; // Connections Only
    if ((rc = getaddrinfo(argv[1], NULL, &hints, &listp)) != 0){
        fprintf(stderr, "getaddrinfo error : %s\n", gai_strerror(rc));
        exit(1);
    }

    /* 리스트 순회하며 IP 주소 출력 */
    flags = NI_NUMERICHOST; // 도메인 이름 대신 주소 문자열 보여주기
    for (p = listp; p; p = p->ai_next){
        Getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, NULL, 0, flags);
        printf("%s\n", buf);
    }

    /* clean up */
    Freeaddrinfo(listp);

    exit(0);
}