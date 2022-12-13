/* $begin tinymain */
/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
 *     GET method to serve static and dynamic content.
 *
 * Updated 11/2019 droh
 *   - Fixed sprintf() aliasing issue in serve_static(), and clienterror().
 */
#include "csapp.h"

void doit(int fd);

// 11.12 POST Method 처리 위해 void 리턴함수에서 int 리턴 함수로 수정, char *method 매개변수 추가
int read_requesthdrs(rio_t *rp, char *method);

int parse_uri(char *uri, char *filename, char *cgiargs);

// 11.11 HEAD Method 처리 위해 char *method 매개변수 추가
void serve_static(int fd, char *filename, int filesize, char *method);

void get_filetype(char *filename, char *filetype);

// 11.11 HEAD Method 처리 위해 char *method 매개변수 추가
void serve_dynamic(int fd, char *filename, char *cgiargs, char *method);

void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
//11.5 A. tiny를 수정해서 모든 요청 라인과 요청 헤더를 echo 하도록 하라.
void echo(int connfd);

//*************************************TINY MAIN STARTS HERE*************************************//
int main(int argc, char **argv) {
    int listenfd, connfd;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;

    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *) &clientaddr,&clientlen);  // line:netp:tiny:accept
        Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE,
                    0);
        printf("Accepted connection from (%s, %s)\n", hostname, port);

//        echo(connfd);
        doit(connfd);   // line:netp:tiny:doit
        Close(connfd);  // line:netp:tiny:close
    }
}
//*************************************TINY MAIN ENDS HERE*************************************//

/*한개의 HTTP 트랜젝션을 처리한다. rio_readlineb 함수를 사용 해서 요청라인을 읽고 분석한뒤
 * GET 메소드가 아닌경우 501 에러
 * 정적 컨텐츠인지, 동적 컨텐츠인지 플래그 설정
 * 요청한 파일이 없으면 404 에러
 * 정적 컨텐츠 분기 - 권한 체크 - 권한 없을시 403 에러 - 있을 경우 serve
 * 동적 컨텐츠 분기 - 권한 체크 - 권한 없을시 403 에러 - 있을 경우 serve
 * */
void doit(int fd) {
    int is_static;
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;

    /*Read Request line and headers*/
    Rio_readinitb(&rio, fd);
    Rio_readlineb(&rio, buf, MAXLINE);
    printf("Request headers:\n");
    printf("%s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);

    /*501 에러 체크 : 11.11 HEAD Method 조건 추가, 11.12 POST Method 조건 추가 */
    if (!(strcasecmp(method, "GET") == 0 || strcasecmp(method, "POST") == 0 || strcasecmp(method,"HEAD") == 0)) {
        clienterror(fd, method, "501", "Not implemented",
                    "Tiny does not implement this method");
        return;
    }
    /*// 11.12 POST Method 추가
    read_requesthdrs(&rio); //POST 메소드 무시*/
    int param_len = read_requesthdrs(&rio, method);
    Rio_readnb(&rio, buf, param_len);

    /*Parse URI from GET request*/
    is_static = parse_uri(uri, filename, cgiargs);

    /*404 에러 체크*/
    if (stat(filename, &sbuf) < 0) {
        clienterror(fd, filename, "404", "Not Found",
                    "Tiny couldn't find this file");
        return;
    }

    /*정적 컨텐츠 요청 : 파일에 대한 읽기 권한 체크*/
    if (is_static) {
        /*403 에러 체크 : 권한 없을 경우*/
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
            clienterror(fd, filename, "403", "Frobidden",
                        "Tiny couldn't read the file");
            return;
        }
        //11.11 method 매개변수 추가
        serve_static(fd, filename, sbuf.st_size, method);
    }
    /*동적 컨텐츠 요청 : 파일에 대한 읽기 권한 체크*/
    else {
        /*403 에러 체크 : 권한 없을 경우*/
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
            clienterror(fd, filename, "403", "Forbidden",
                        "Tiny couldn't run the CGI Program");
            return;
        }
        // 11.12 POST Method 추가로 분기문으로 수정
        /*// 11.11 method 매개변수 추가
        serve_dynamic(fd, filename, cgiargs, method);*/
        if (strcasecmp(method, "POST") == 0){
            serve_dynamic(fd, filename, buf, method);
        } else {
            serve_dynamic(fd, filename, cgiargs, method);
        }
    }
}

/*에러 화면 출력 페이지
 * HTTP 응답을 응답 라인에 적절한 샅애 코드와 상태 메세지와 함께 클라이언트에 보내며
 * 브라우저 사용자에게 에러를 설명하는 HTML을 포함
 * */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
    char buf[MAXLINE], body[MAXBUF];

    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web Server</em>\r\n", body);

    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Contents-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Cotent-lenth: %d\r\n\r\n", (int) strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}

/*요청 헤더를 읽는 함수.
* 11.12 POST Method 처리 위해 void 리턴함수에서 int 리턴 함수로 수정, char *method 매개변수 추가 */
int read_requesthdrs(rio_t *rp, char *method) {
    char buf[MAXLINE];

    //11.12
    /*Rio_readlineb(rp, buf, MAXLINE);
    while (strcmp(buf, "\r\n")) {
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    }
    return;*/

    int content_len = 0;

    do {
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
        if (strcasecmp(method, "POST") == 0 && strncasecmp(buf, "Content-Length:", 15) == 0) {
            sscanf(buf, "Content-Length: %d", &content_len);
        }
    } while(strcmp(buf, "\r\n"));

    return content_len;
}

/*URI 파싱 함수
 * 정적 컨텐츠를 위한 홈디렉토리 = 현재 자신의 디렉토리
 * 실행파일의 홈 디렉토리 = /cgi-bin
 * 스트링 "cgi-bin"을 포함하는 모든 uri는 동적 컨텐츠를 요청하는 것으로 가정.
 * 기본 파일 이름은 ./home.html
 * URI파일 이름과 옵션으로 CGI 인자 스트링을 파싱하여
 *
 * 정적 컨텐츠 일때 : CGI 스트링을 지우고 URI를 ./index.html 같은 상태 리눅스 경로 이름으로 변환
 * 만약 URI 가 '/' 로 끝난다면 기본파일 이름("home.html")을 추가
 *
 * 동적 컨텐츠 일때 : 모든 CGI인자를 추출하고, 나머지 URI 부분을 상태 리눅스 파일 이름으로 변환
 * */
int parse_uri(char *uri, char *filename, char *cgiargs) {
    char *ptr;

    /*정적 컨텐츠*/
    if (!strstr(uri, "cgi-bin")) {
        strcpy(cgiargs, "");
        strcpy(filename, ".");
        strcat(filename, uri);
        if (uri[strlen(uri) - 1] == '/') {
            strcat(filename, "home.html");
        }
        return 1;
    }
    /*동적 컨텐츠*/
    else {
        ptr = index(uri, '?');
        if (ptr) {
            strcpy(cgiargs, ptr + 1);
            *ptr = '\0';
        } else {
            strcpy(cgiargs, "");
        }
        strcpy(filename, ".");
        strcat(filename, uri);
        return 0;
    }
}

/*파일 종류 판별 함수*/
void get_filetype(char *filename, char *filetype) {
    if (strstr(filename, ".html")) {
        strcpy(filetype, "text/html");
    } else if (strstr(filename, ".gif")) {
        strcpy(filetype, "image/gif");
    } else if (strstr(filename, ".png")) {
        strcpy(filetype, "image/png");
    } else if (strstr(filename, ".jpg")) {
        strcpy(filetype, "image/jpg");
    } else if (strstr(filename, ".mp4")) { //11.7 Tiny를 확장해서 MPG비디오 파일을 처리하도록 하시오.
        strcpy(filetype, "video/mp4");
    } else {
        strcpy(filetype, "text/plain");
    }
}

/* 정적 컨텐츠 제공 함수
 * TINY webserver는 다섯개의 서로 다른 정적 컨텐츠 타입을 지원한다.
 * HTML, txt, GIF, PNG, JPEG
 * serve_static 함수는 지역 파일의 내용을 포함하고 있는 본체를 갖는 HTTP 응답을 보낸다.
 * get_filetype 함수로 파일 이름의 확장자를 검사해서 파일 타입을 결정하고
 * 클라이언트에 응답 줄과 응답 헤더를 보냄.
 *
 * 요청한 파일의 내용을 연결 식별자 fd로 복사하여 응답 body를 보냄.
 *
 * 11.11 HEAD Method 처리 위해 char *method 매개변수 추가
 * */
void serve_static(int fd, char *filename, int filesize, char *method) {
    int srcfd;
    char *srcp, filetype[MAXLINE], buf[MAXBUF];

    /*클라이언트에게 응답 헤더 보냄*/
    get_filetype(filename, filetype);
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer : Tiny Web Server\r\n", buf);
    sprintf(buf, "%sConnection : close\r\n", buf);
    sprintf(buf, "%sContents-length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContents-type : %s\r\n\r\n", buf, filetype);
    Rio_writen(fd, buf, strlen(buf));
    printf("Response headers:\n");
    printf("%s", buf);

    // 11.11 HEAD 메소드 추가
    if (!strcasecmp(method, "HEAD")) {
        return;
    }

    /*클라이언트에게 응답 body 보냄*/
    srcfd = Open(filename, O_RDONLY, 0); // 파일 이름 읽어옴

/*    // 1. MMAP Method : CSAPP 원본 코드
    srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    // 읽어온 파일을 mmap 함수를 통해 가상메모리 영역으로 매핑
    // mmap 호출시 파일 srcfd의 첫번째 filesize 바이트를 주소 srcp에서 시작하는 사적 읽기-허용 가상메모리 영역으로 매핑
    Close(srcfd); // 파일을 가상메모리 영역으로 매핑 이후에는 파일을 닫아도 된다.
    Rio_writen(fd, srcp, filesize);
    Munmap(srcp, filesize); // 매핑된 가상메모리 주소 반환*/

    // 2. Malloc Method : Exercise 11.9 Tiny를 수정해서 정적 컨텐츠를 처리할 때 요청한 파일을 mmap 과 rio_readn 대신에
    //                    malloc, rio_readn, rio_written을 사용해서 연결 식별자에게 복사하도록 하시오.
    srcp = (char*)Malloc(filesize);// filesize 크기 만큼의 가상메모리 할당
    Rio_readn(srcfd, srcp, filesize); // 읽을 파일(srcfd), 할당된 가상 메모리 공간 주소(srcp), 공간 사이즈(filesize)
    Close(srcfd);
    Rio_writen(fd, srcp, filesize);
    free(srcp); //할당된 가상 메모리 해제

}

/*동적컨텐츠 제공 함수
 * TINY는 자식 프로세스를 Fork 하고, 그 후에 CGI 프로그램을 자식의 컨텍스트에서 실행하며 모든 종류의 동적 컨첸츠를 제공한다.
 * HTTP 응답으로 성공 응답 라인 보내고
 * CGI 프로그램을 실행함으로서 나머지 부분 보냄. (즉, CGI 프로그램에서 에러를 만날경우 클라이언트 사이드는 모름)
 * 자식은 QUERY_STRING 환경변수를 요청 URI CGI 인자들로 초기화.
 * 자식은 자식의 표준 출력을 연결 파일 식별자로 재지정하고 (Dup2)
 * 그후에 CGI 프로그램을 로드하고 실행. (Execve)
 * 부모는 자식이 종료되어 정리되는 것을 기다리기 위해 wait 함수에서 블록.
 *
 * 11.11 HEAD Method 처리 위해 char *method 매개변수 추가
 * */
void serve_dynamic(int fd, char *filename, char *cgiargs, char *method) {
    char buf[MAXLINE], *emptylist[] = {NULL};

    /*Return first part of HTTP response*/
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server : Tiny Web Server\r\n");
    Rio_writen(fd, buf, strlen(buf));

    // fork() 함수는 함수를 호출한 프로세스를 복사하는 기능을 함
    // 프로세스 id, 즉 pid 를 반환하게 되는데 이때 부모 프로세스에서는 자식 pid가 반환되고
    // 자식 프로세스에서는 0이 반환된다. 만약 fork() 함수 실행이 실패하면 -1을 반환한다.
    if (Fork() == 0) {
        setenv("QUERY_STRING", cgiargs, 1);
        setenv("REQUEST_METHOD", method, 1); // 11.11 HEAD 메소드 추가
        Dup2(fd, STDOUT_FILENO);
        /*
         * 어떤 CGI 프로그램이 동적 컨텐츠를 클라이언트에 보낼 필요가 있다고 가정하자.
         * 이것은 일반적으로 CGI프로그램이 표준 출력으로 컨덴츠를 전송하는 방식으로 구현된다.
         * 어떻게 이 컨텐츠가 클라이언트로 전송되는지 설명하시오.
         *  -> CGI 프로그램을 실행하는 프로세스가 로드되기 전에 리눅스 dup2함수는 표준 출력을
         *  클라이언트와 연관된 연결식별자로 재지정한다.
         *  따라서 CGI 프로그램이 표준 출력으로 쓰는 모든 것은 클라이언트로 직접 전송된다.
         * */
        Execve(filename, emptylist, environ); // CGI 프로그램 실행
    }
    Wait(NULL);
}

/* 11.6 echo implement */
void echo(int connfd) {
    size_t n;
    char buf[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while ((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        if (strcmp(buf, "\r\n") == 0) {
            break;
        }
        printf("server received %d bytes\n", (int) n);
        Rio_writen(connfd, buf, n);
    }
}