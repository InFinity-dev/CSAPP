cmake_minimum_required(VERSION 3.23)
project(webservers C)

set(CMAKE_C_STANDARD 99)

add_executable(webservers tiny/csapp.c tiny/tiny.c tiny/cgi-bin/adder.c cache.h cache.c csapp.c error.h debug.h echo/echoserveri.c exercise/hostinfo.c echo/echoclient.c echo/echoserveri.c echo/echoserveri.c echo/echoserveri.c exercise/hostinfo.c exercise/global-hex2dd.c tiny/cgi-bin/post-adder.c)
