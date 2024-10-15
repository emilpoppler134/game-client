#ifndef __UDP_H__
#define __UDP_H__

// Dependencies
// __________________________________________________________
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>

#include "common.h"

// Networking
// __________________________________________________________
#if defined(_WIN32)
// For win based system
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
// For unix based system
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#endif

// Socket in windows returns a unsingde int and in onix system it returns a int
#if !defined(_WIN32)
#define SOCKET int
#endif

// Socket returns INVALID_SOCKET if it fails on windos and negativ number on Onix. But SOCKET is an unsignid int on windows
#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#endif

// Windows need a special funtion to close sockets
#if defined(_WIN32)
#define CLOSESOCKET(s) closesocket(s)
#else
#define CLOSESOCKET(s) close(s)
#endif

// On windows you retrive errors with the WSAGetLastError fuktion and on Onix errno
#if defined(_WIN32)
#define GETSOCKETERRNO() (WSAGetLastError())
#else
#define GETSOCKETERRNO() (errno)
#endif

// This is needed on windos to indicate if a terminal input is wating
#if defined(_WIN32)
#include <conio.h>
#endif

// UDP
// _____________________________________________________________________________
typedef struct udp_args_t
{
    player_t *opponents[18];
    vec2_t *position;
    int id;
} udp_args_t;

// METHODS
// _____________________________________________________________________________
void connect_udp_server();

#endif