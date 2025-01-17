#include "server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <string.h>

std::string charArrToString(char* arr)
{
    std::string s(arr);
    return s;
}

void startServer(int port) {
    // create a socket
    int server_fd;
    sockaddr_in address{};
    std::vector<int> clients;
    if ( (server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Socket creation error" << std::endl;
    }

    // define server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // allow socket to be reused
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        std::cerr << "Failed to set socket options" << std::endl;
    }

    // bind the socket to the address
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        std::cerr << "Failed to bind socket to the address" << std::endl;
    }

    // listen for incoming connections
    if (listen(server_fd, 5) < 0)
    {
        std::cerr << "Failed to listen for incoming connections" << std::endl;
    }

    std::cout << "Server started on port " << port << std::endl;

    fd_set readfds;
    while (true)
    {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        int max_sd = server_fd;

        for (int client : clients)
        {
            FD_SET(client, &readfds);
            if (client > max_sd)
            {
                max_sd = client;
            }
        }

        int activity = select(max_sd + 1, &readfds, nullptr, nullptr, nullptr);
        if ((activity < 0) && (errno != EINTR))
        {
            std::cerr << "Select error" << std::endl;
        }

        if (FD_ISSET(server_fd, &readfds))
        {
            int new_socket;
            socklen_t addrlen = sizeof(address);
            if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0)
            {
                std::cerr << "Failed to accept incoming connection" << std::endl;
                return;
            }
            clients.push_back(new_socket);
            std::cout<<"New connection, socket fd is " << new_socket << std::endl;
        }

        for (auto it = clients.begin(); it!=clients.end(); )
        {
            int sd = *it;
            if (FD_ISSET(sd, &readfds))
            {
                char buffer[1024] = {0};
                int valread = read(sd, buffer, 1024);
                if (valread == 0)
                {
                    close(sd);
                    it = clients.erase(it);
                    std::cout << "Client disconnected, socket fd is " << sd << std::endl;
                }
                else
                {
                    buffer[valread] = '\0';
                    std::cout << "Message from client " << sd << ": " << buffer << std::endl;
                    std::string finalMessage = "User " + std::to_string(sd) + ": " + charArrToString(buffer);
                    for (int client: clients)
                    {
                        if (client != sd)
                        {
                            send(client, finalMessage.c_str(), finalMessage.length(), 0);
                        }
                    }
                    ++it;
                }
            }
            else
            {
                ++it;
            }
        }
    }
    
    close(server_fd);
}