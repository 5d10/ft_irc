#ifndef FT_IRC_HPP
# define FT_IRC_HPP

# include <stdlib.h>
# include <sys/socket.h>
# include <netdb.h>
# include <poll.h>
# include <iostream>
# include <errno.h>
# include <unistd.h>
# include <strings.h>
# include <vector>

# define MAX_CONN_QUEUE 128

# define DEBUG 0

class Client
{
    public:
        int fd;
        std::string rd_buff;
        std::string wr_buff;

        Client(int fd)
        {
            this->fd = fd;
            rd_buff = "";
            wr_buff = "";
        }

        Client &operator=(const Client &other)
        {
            fd = other.fd;
            rd_buff = other.rd_buff;
            wr_buff = other.wr_buff;
            return *this;
        }

        ssize_t Read();
        ssize_t Send();
};

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
        void RemoveClient(size_t index);
        void AcceptClient();
        int OnClientRead(size_t index);
        int OnClientSend(size_t index);
        int cycle();

};
#endif
