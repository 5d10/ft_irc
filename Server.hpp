#ifndef SERVER_HPP
# define SERVER_HPP

#include "ft_irc.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Task.hpp"

# define MAX_CONN_QUEUE 128

# define DEBUG 1

class Server
{
    private:
        std::vector<struct pollfd> pollfds;
        std::map<std::string, const Client&> registered;

        int initialize_listener(const int port);
        Client &getClientAtIndex(size_t index);
    public:
        const char *const password;
        std::list<Client> clients;

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
