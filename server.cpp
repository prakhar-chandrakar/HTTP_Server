#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <WinSock2.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <regex>

using namespace std;

string extractRequestData(const string &requestBody)
{
    // Define a regex pattern to match the username and password
    regex pattern("\\{\"username\":\"([^\"]+)\",\"password\":\"([^\"]+)\"\\}");
    smatch matches;
    if (std::regex_search(requestBody, matches, pattern))
    {

        string username = matches[1];
        string password = matches[2];
        string resp = (username == "prakhar" && password == "prakhar") ? "Login successful" : "Login failed";
        cout << "-> Response : " << resp << endl;
        return resp;
    }
}

int main()
{
    cout << "\n-> Attempting to create a server... " << endl;
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
    cout << "-> Server successfully created, listening on " << serverIp << ":" << port << endl
         << endl;

    int bytes = 0;
    while (true)
    {
        // accept client request
        new_wsocket = accept(wsocket, (SOCKADDR *)&server, &server_len);
        if (new_wsocket == INVALID_SOCKET)
        {
            cout << "Could not accept socket connection";
        }

        // read client request
        char buffer[BUFFER_SIZE] = {0};
        bytes = recv(new_wsocket, buffer, BUFFER_SIZE, 0);
        if (bytes < 0)
        {
            cout << "Coud not read form client request";
        }

        string request(buffer);

        if (request.find("OPTIONS") == 0)
        {
            cout << "-> Received preflight OPTIONS request, responding CORS allowed\n";
            string optionsResponse =
                "HTTP/1.1 204 No Content\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
                "Access-Control-Allow-Headers: Content-Type\r\n"
                "Content-Length: 0\r\n"
                "\r\n";

            send(new_wsocket, optionsResponse.c_str(), optionsResponse.size(), 0);
            closesocket(new_wsocket);
            continue;
        }

        cout << "-> Received actual request from client\n\n";
        // cout << "-> Client request : \n" << request << endl;
        string response;
        size_t bodyStart = request.find("\r\n\r\n");
        if (bodyStart != string::npos)
        {
            string requestBody = request.substr(bodyStart + 4);
            cout << "-> Request Body: " << requestBody << endl;
            response = extractRequestData(requestBody);
        }

        string serverMessage =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n"
            "Content-Length: ";

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
                cout << "Could not send response";
            }
            totalBytesSent += bytesSent;
        }
        cout << "\n-> Sent response back to client" << endl;
        closesocket(new_wsocket);
    }
    closesocket(wsocket);
    WSACleanup();
    return 0;
}