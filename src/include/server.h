#ifndef __SERVER_H__
#define __SERVER_H__

#include "raylib.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>

#ifdef _WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#elif __APPLE__
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#endif

#define MAC_ADDRSTRLEN 18
#define MAX_CLIENTS 16

// UDP CONNECTION
// _____________________________________________________________________________

typedef Vector2 vec2_t;

typedef struct player_t
{
  vec2_t position;
} player_t;

typedef struct udp_request_package_t
{
  int id;
  player_t player;
} udp_request_package_t;

typedef struct client_t
{
  char mac_address[MAC_ADDRSTRLEN];
  int tcp_socket;
  int connected;
  int occupied;
  vec2_t position;
} client_t;

typedef struct udp_response_package_t
{
  client_t clients[MAX_CLIENTS];
} udp_response_package_t;

// TCP CONNECTION
// _____________________________________________________________________________
typedef char inet_address_t[INET_ADDRSTRLEN];

typedef struct connection_t
{
  int id;
  char mac_address[MAC_ADDRSTRLEN];
  int tcp_socket;
  int udp_socket;
} connection_t;

// TCP PACKAGES
// _____________________________________________________________________________
typedef enum tcp_request_type_e
{
  PACKAGE_IDENTIFY,
} tcp_request_type_e;

typedef enum tcp_response_type_e
{
  OK = 0,
  ERROR = -1,
} tcp_response_type_e;

typedef union payload_u
{
  char mac_address[MAC_ADDRSTRLEN];
} payload_u;

typedef union data_u
{
  int id;
} data_u;

typedef struct tcp_request_package_t
{
  tcp_request_type_e type;
  payload_u payload;
} tcp_request_package_t;

typedef struct tcp_response_package_t
{
  tcp_response_type_e type;
  data_u data;
} tcp_response_package_t;

// METHODS
// _____________________________________________________________________________

connection_t connect_tcp_server(inet_address_t);
tcp_request_package_t init_request(tcp_request_type_e, payload_u);
tcp_response_package_t send_tcp_package(connection_t, tcp_request_package_t);
void close_tcp_server(connection_t);

#endif
