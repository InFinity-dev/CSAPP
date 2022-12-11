cmake_minimum_required(VERSION 3.23)
project(webservers C)

set(CMAKE_C_STANDARD 99)

add_executable(webservers hostinfo.c echoclient.c echoserveri.c echo.c adder.c tiny/csapp.c tiny/tiny.c global-hex2dd.c dd2hex.c hostinfo-ntop.c)
