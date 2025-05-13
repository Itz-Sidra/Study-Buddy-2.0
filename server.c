#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET server, client;
    struct sockaddr_in server_addr, client_addr;
    int client_len;
    char buffer[1024], response[2048];

    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup failed. Error Code: %d", WSAGetLastError());
        return 1;
    }

    // Create socket
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET) {
        printf("Could not create socket: %d", WSAGetLastError());
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(server, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d", WSAGetLastError());
        return 1;
    }

    listen(server, 3);
    printf("Listening on port 8080...\n");

    client_len = sizeof(struct sockaddr_in);
    client = accept(server, (struct sockaddr *)&client_addr, &client_len);
    if (client == INVALID_SOCKET) {
        printf("Accept failed: %d", WSAGetLastError());
        return 1;
    }

    recv(client, buffer, sizeof(buffer) - 1, 0);
    printf("Received request:\n%s\n", buffer);

    // Corrected response with proper Content-Length
    const char *body = "<html><body><h1>Hello from C Server!</h1></body></html>";
    sprintf(response,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s", strlen(body), body);

    send(client, response, strlen(response), 0);

    closesocket(client);
    closesocket(server);
    WSACleanup();

    return 0;
}