#include "client.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

void handleServerMessages(int client_fd)
{
    char buffer[1024] = {0};
    while (true)
    {
        int valread = read(client_fd, buffer, 1024);
        if (valread == 0)
        {
            std::cout << "Server disconnected" << std::endl;
            break;
        }
        else
        {
            buffer[valread] = '\0';
            std::cout << buffer << std::endl;
        }
    }
}

void startClient(const std::string& address, int port)
{
    int client_fd;
    sockaddr_in serverAddress{};

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    } 

    std::cout<<"Socket created"<<std::endl;

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (inet_pton(AF_INET, address.c_str(), &serverAddress.sin_addr) <= 0)
    {
        std::cerr << "Invalid address or address not supported" << std::endl;
        return;
    }

    if (connect(client_fd, (const sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
    {
        std::cerr <<"Connection failed, check the configuration"<<std::endl;
        return;
    }

    std::cout<<"Successfully connected to the server!"<<std::endl;

    std::thread serverThread(handleServerMessages, client_fd);

    std::string message;

    while (true)
    {
        std::getline(std::cin, message);
        send(client_fd, message.c_str(), message.length(), 0);
    }

    close(client_fd);
    serverThread.join();

}