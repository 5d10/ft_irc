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
    private:
        int fd;
        std::string rd_buff;
        std::string wr_buff;
    public:
        Client()
        {
            this->fd = -1;
            rd_buff = "";
            wr_buff = "";
        }
        Client(int fd)
        {
            this->fd = fd;
            rd_buff = "";
            wr_buff = "";
        }
        Client(const Client &other)
        {
            fd = other.fd;
            rd_buff = other.rd_buff;
            wr_buff = other.wr_buff;
        }
        Client &operator=(const Client &other)
        {
            fd = other.fd;
            rd_buff = other.rd_buff;
            wr_buff = other.wr_buff;
            return *this;
        }
        ~Client() {}
        int GetFD() { return fd; }
        std::string GetReadBuffer() { return rd_buff; }
        std::string GetWriteBuffer() { return wr_buff; }

        void ClearReadBuffer() { rd_buff = ""; }
        void AddToWriteBuffer(std::string msg) { wr_buff += msg; }

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
        void DisconnectClient(size_t index);
        void AcceptClient();
        int OnClientRead(size_t index);
        int OnClientSend(size_t index);
        int cycle();

};
#endif
