#include "include/server.h"

void get_mac_address(char *mac_address)
{
#ifdef _WIN32
  IP_ADAPTER_INFO AdapterInfo[16];
  DWORD dwBufLen = sizeof(AdapterInfo);

  DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
  if (dwStatus != ERROR_SUCCESS)
  {
    perror("GetAdaptersInfo");
    exit(EXIT_FAILURE);
  }

  PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
  sprintf(mac_address, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", pAdapterInfo->Address[0], pAdapterInfo->Address[1], pAdapterInfo->Address[2], pAdapterInfo->Address[3], pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
#elif __APPLE__
  struct ifaddrs *ifap, *ifaptr;
  unsigned char *ptr;
  int found = 0;

  if (getifaddrs(&ifap) != 0)
  {
    perror("getifaddrs");
    exit(1);
  }

  for (ifaptr = ifap; ifaptr != NULL; ifaptr = ifaptr->ifa_next)
  {
    if (ifaptr->ifa_addr != NULL && ifaptr->ifa_addr->sa_family == AF_LINK)
    {
      struct sockaddr_dl *sdl = (struct sockaddr_dl *)ifaptr->ifa_addr;
      if (sdl->sdl_type == IFT_ETHER)
      {
        ptr = (unsigned char *)LLADDR(sdl);
        sprintf(mac_address, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);
        found = 1;
        break;
      }
    }
  }

  freeifaddrs(ifap);

  if (!found)
  {
    fprintf(stderr, "No MAC address found\n");
    exit(1);
  }
#endif
}

connection_t connect_tcp_server(inet_address_t server_ip)
{
  connection_t connection = {0};

  pthread_t connect_tid;
  struct sockaddr_in server_addr;

  int server_socket = 0;
  server_socket = socket(AF_INET, SOCK_STREAM, 0);

  if (server_socket < 0)
  {
    perror("Error in socket");
    exit(1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8100);
  server_addr.sin_addr.s_addr = inet_addr(server_ip);

  if (connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("Error in connect");
    exit(1);
  }

  connection.tcp_socket = server_socket;

  char mac_address[18] = {0};
  get_mac_address(mac_address);

  payload_u payload = {0};
  strcpy(payload.mac_address, mac_address);
  tcp_request_package_t request = init_request(PACKAGE_IDENTIFY, payload);
  tcp_response_package_t response = send_tcp_package(connection, request);

  if (response.type == ERROR)
  {
    fprintf(stderr, "Server is full\n");
    exit(1);
  }

  connection.id = response.data.id;
  connection.mac_address = mac_address;

  return connection;
}

tcp_request_package_t init_request(tcp_request_type_e type, payload_u payload)
{
  tcp_request_package_t package = {0};
  package.type = type;
  package.payload = payload;
  return package;
}

tcp_response_package_t send_tcp_package(connection_t connection, tcp_request_package_t request)
{
  // Send payload to tcp server
  ssize_t bytes_sent = send(connection.tcp_socket, &request, sizeof(tcp_request_package_t), 0);
  if (bytes_sent < 0)
  {
    perror("Error in send");
    exit(1);
  }

  // Wait for response from tcp server
  tcp_response_package_t response = {0};
  size_t response_size = sizeof(tcp_response_package_t);
  unsigned char buffer[response_size];

  ssize_t bytes_received = recv(connection.tcp_socket, buffer, sizeof(buffer), 0);
  if (bytes_received <= 0)
  {
    perror("Error in recv");
    exit(1);
  }

  memcpy(&response, buffer, response_size);

  return response;
}

void close_tcp_server(connection_t connection)
{
  close(connection.tcp_socket);
}
