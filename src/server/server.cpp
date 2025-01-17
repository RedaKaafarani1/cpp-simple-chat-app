#include "server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <algorithm>

int handleClientExit(std::vector<int>& clients, std::map<int, std::string> &clientNames, int client_fd)
{
    close(client_fd);
    send(client_fd, "Goodbye...", 10, 0);
    std::string user_name = clientNames[client_fd];
    clients.erase(std::remove(clients.begin(), clients.end(), client_fd), clients.end());
    clientNames.erase(client_fd);
    std::string message = "User " + (user_name != "" ? user_name : std::to_string(client_fd)) + " has left the chat";
    std::cout<<message<<std::endl;
    for (int client: clients)
    {
        send(client, message.c_str(), message.length(), 0);
    }
    return 1;
}

int handleClientMessage(std::vector<int> &clients, std::map<int, std::string> &clientNames, std::string welcomeMessage, const char* message, int client_fd)
{
    std::string msg = charArrToString(message);
    if (msg[0] == '/')
    {
        if (msg == "/help")
        {
            std::string helpMessage = welcomeMessage.substr(23, welcomeMessage.length() - 1);
            send(client_fd, helpMessage.c_str(), helpMessage.length(), 0);
        }
        else if (msg == "/list")
        {
            send(client_fd, "List of connected users:\n", 26, 0);
            std::string userList = "";
            for (const auto &pair: clientNames)
            {
                std::cout<<"LOG: "<<pair.first<<" "<<pair.second<<std::endl;
            }
            for (const auto &pair: clientNames)
            {
                userList += (pair.second != "" ? pair.second + "\n" : "User " + std::to_string(pair.first) + "\n");
            }
            send(client_fd, userList.c_str(), userList.length(), 0);
        }
        else if (msg == "/exit")
        {
            handleClientExit(clients, clientNames, client_fd);
            return 1;
        }
        else if (msg.find("/name")!=std::string::npos)
        {
            int pos = 5;
            while (pos < (int)msg.length() && msg[pos] == ' ')
            {
                pos++;
            }
            if (pos >= (int)msg.length())
            {
                send(client_fd, "Invalid command, type /help for a list of available commands", 60, 0);
                return 1;
            }
            std::string name = msg.substr(pos, msg.length() - 1);
            clientNames[client_fd] = name;
            std::string nameSet = "Name set to " + name;
            send(client_fd, nameSet.c_str(), nameSet.length(), 0);
        }
        else
        {
            send(client_fd, "Invalid command, type /help for a list of available commands", 60, 0);
        }
        return 1;
    }
    return 0;
}

std::string charArrToString(const char* arr)
{
    std::string s(arr);
    return s;
}

void startServer(int port) {
    // create a socket
    int server_fd;
    sockaddr_in address{};
    std::vector<int> clients;
    std::map<int, std::string> clientNames;

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

    std::string welcomeMessage = "Welcome to the server!\nHere is a list of available commands:\n"
                                 "1. /help - displays a list of available commands\n"
                                 "2. /list - displays a list of connected users\n"
                                 "3. /exit - disconnects you from the server\n"
                                 "4. /name <name> - sets your name to <name>\n";

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
            clientNames[new_socket] = "";
            std::cout<<"New connection, socket fd is " << new_socket << std::endl;
            send(clients.back(), welcomeMessage.c_str(), welcomeMessage.length(), 0);
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
                    handleClientExit(clients, clientNames, sd);
                }
                else
                {
                    buffer[valread] = '\0';
                    std::cout << "Message from client " << (clientNames[sd] != "" ? clientNames[sd] : std::to_string(sd)) << ": " << buffer << std::endl;
                    int isCommand = handleClientMessage(clients, clientNames, welcomeMessage, buffer, sd);
                    if (isCommand)
                    {
                        ++it;
                        continue;
                    }
                    else
                    {
                        std::string finalMessage = clientNames[sd] != "" ? clientNames[sd] + ": " + charArrToString(buffer) : "User " + std::to_string(sd) + ": " + charArrToString(buffer);
                        for (int client: clients)
                        {
                            if (client != sd)
                            {
                                send(client, finalMessage.c_str(), finalMessage.length(), 0);
                            }
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