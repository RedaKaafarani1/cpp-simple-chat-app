#include "client.h"

int main()
{
    const std::string address = "127.0.0.1";
    int port = 8080;
    startClient(address, port);
    return 0;
}