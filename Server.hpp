#ifndef SERVER_HPP
# define SERVER_HPP

# include <vector>
# include <iostream>
# include <unistd.h>
# include <sys/socket.h>
# include <netdb.h>
# include <poll.h>
# include <strings.h>
# include <errno.h>
# include "Client.hpp"

# define MAX_CONN_QUEUE 128

# define DEBUG 1

class Server
{
    private:
        const char *const password;
        std::vector<struct pollfd> pollfds;
        std::vector<Client> clients;

        int initialize_listener(const int port);

    public:
        Server(const char *const password) : password(password) { }
        int init(int port);
        void AddClient(int fd, short flags);
        void DisconnectClient(size_t index);
        void AcceptClient();
        int OnClientRead(size_t index);
        int OnClientSend(size_t index);
        int cycle();

};
#endif
