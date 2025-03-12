#ifndef SERVER_HPP
# define SERVER_HPP

#include "ft_irc.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Task.hpp"

# define MAX_CONN_QUEUE 128

class Server
{
    private:
        std::vector<struct pollfd> pollfds;



        int initialize_listener(const int port);
        Client &getClientAtIndex(size_t index);
        void DisconnectClient(size_t index);//*use EraseClient instead
        void SetClientPolloutFlags();
    public:
        const char *const password;
        std::list<Client> clients;
        std::map<std::string, Client *> registered;
        std::map<std::string, Channel> channels;

        Server(const char *const password) : password(password) { }
        int init(int port);
        void AddClient(int fd, short flags);
		void EraseClient(Client &client, std::string quit_message);
        void AcceptClient();
        int OnClientRead(size_t index);
        int OnClientSend(size_t index);
        int cycle();
		void Rename(Client& client, std::string new_name);
};
#endif
