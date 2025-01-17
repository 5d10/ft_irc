#ifndef SERVER_HPP
# define SERVER_HPP

#include "ft_irc.hpp"
#include "Client.hpp"
#include "Channel.hpp"

# define MAX_CONN_QUEUE 128

# define DEBUG 1

class Server
{
    private:
        const char *const password;
        std::vector<struct pollfd> pollfds;
        std::list<Client> clients;
        std::map<std::string, Client&> client_map;

        int initialize_listener(const int port);
        Client &getClientAtIndex(size_t index);
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
