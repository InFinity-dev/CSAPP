//
// Created by InFinity on 2022/12/12.
//

#ifndef WEBSERVERS_DEBUG_H
#define WEBSERVERS_DEBUG_H

# define dbg_printf(...) printf(__VA_ARGS__)
# define LOG(str) (printf("%s (%d) - <%s> %s\n", __FILE__ , __LINE__ ,__FUNCTION__, str))

#endif //WEBSERVERS_DEBUG_H
