#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <WinSock2.h>
#include <string.h>

using namespace std;

int main()
{
    cout << " Attempting to create a server" << endl;
    SOCKET wsocket;
    SOCKET new_wsocket;
    WSADATA wsaData; // to hold windows socket information
    struct sockaddr_in server;
    int server_len;
    int BUFFER_SIZE = 30720;

    // initializa
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        cout << "WSAStartup failed with error code: " << WSAGetLastError();
    }

    // create a socket
    wsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (wsocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed with error code: " << WSAGetLastError();
    }

    // bind socket to specific address
    int port = 8080;
    string serverIp = "127.0.0.1";
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(serverIp.c_str());
    server.sin_port = htons(port);
    server_len = sizeof(server);

    if (bind(wsocket, (SOCKADDR *)&server, server_len) != 0)
    {
        cout << "Socket bind failed with error code: " << WSAGetLastError();
    }

    // listen to our address
    if (listen(wsocket, 20))
    {
        cout << "Socket listen failed with error code: " << WSAGetLastError();
    }
    cout << " Server successfully created, listening on " << serverIp << ":" << port << endl;

    int bytes = 0;
    while (true)
    {
        // accept client request
        new_wsocket = accept(wsocket, (SOCKADDR *)&server, &server_len);
        if (new_wsocket == INVALID_SOCKET)
        {
            cout << " Could not accept socket connection";
        }
        cout << " Received request from client" << endl;

        // read client request
        char buff[30720] = {0};
        bytes = recv(new_wsocket, buff, BUFFER_SIZE, 0);
        if (bytes < 0)
        {
            cout << "coud not read form client request";
        }

        string serverMessage = "HTTP/1.1 200 OK\ncontent-Type: text/html\nContent-Length: ";
        string response = "<html><h1>Hello World</h1></html>";
        serverMessage.append(to_string(response.size()));
        serverMessage.append(("\n\n"));
        serverMessage.append(response);

        // send resp to client
        int bytesSent = 0;
        int totalBytesSent = 0;
        while (totalBytesSent < serverMessage.size())
        {
            bytesSent = send(new_wsocket, serverMessage.c_str(), serverMessage.size(), 0);
            if (bytesSent < 0)
            {
                cout << " Could not send response";
            }
            totalBytesSent += bytesSent;
        }
        cout << " Sent response to client" << endl;
        closesocket(new_wsocket);
    }
    closesocket(wsocket);
    WSACleanup();
    return 0;
}