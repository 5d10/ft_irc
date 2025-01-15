#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "ft_irc.hpp"

class Client
{
    private:
        int fd;
        std::string rd_buff;
        std::string wr_buff;

        Client()
        {
            this->fd = -1;
            rd_buff = "";
            wr_buff = "";
        }

    public:
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
#endif
