# CPP simple chat App

This is a simple server/client application written in C++. A simple TCP server is opened and then clients can be connected to chat.

## How to start using it
1. Start server in a terminal `./server`
2. Start client in a another terminal `./client`
3. As a client, you can type some commands to show help list, change name, to list connected users and to exit
4. As a server, you see all the commands and messages from all the users and you get appropriate messages for connecting/disconnecting users

## To go further

This app is simple and was done in a very short amount of time. It was not fully tested and hence it's prone to vulnerabilities and bugs.
Feel free to modify the code to add functionality or fix bugs.

## How to rebuild?

If you change the code in any way, you can simple rebuild using `make`. Make sure CMAKE is installed on your machine.
