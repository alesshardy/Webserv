#ifndef WEBSERV_HPP
#define WEBSERV_HPP

extern bool running;

#define REQUEST_EVENTS EPOLLIN | EPOLLRDHUP | EPOLLERR 
#define RESPONSE_EVENTS EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLERR

#define CHUNKED_THRESHOLD 1024 * 1024 // 1 MB

//Colors


// Reset
#define RESET "\033[0m"

// Text styles
#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"

// Text colors
#define BLACK "\033[30m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"

// Background colors
#define BG_BLACK "\033[40m"
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN "\033[46m"
#define BG_WHITE "\033[47m"

#include "Config.hpp"
#include "Server.hpp"
#include "CommandLineParser.hpp"
#include <iostream>
#include "LogManager.hpp"
#include "Socket.hpp"
#include <cstdlib> 
#include <csignal>

#endif