#ifndef SERVER_H
#define SERVER_H
#include <iostream>
#include <vector>
#include <map>

int handleClientExit(std::vector<int>& clients, std::map<int, std::string> &clientNames, int client_fd);
int handleClientMessage(std::vector<int> &clients, std::map<int, std::string> &clientNames, std::string welcomeMessage, const char* message, int client_fd);
std::string charArrToString(const char* arr);
void startServer(int port);

#endif