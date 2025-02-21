#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "ft_irc.hpp"

class Channel;

// TODO: MOVE CODE TO CPP FILE FFS

class Client
{
    private:
        int fd;
        std::string rd_buff;
        std::string wr_buff;
        //short *events;
        Client()
        {
            this->fd = -1;
            rd_buff = "";
            wr_buff = "";
			registered = false;
        }

    public:
		bool registered;
		bool passed;
		bool nicked;
		bool usernamed;
		std::string nickname;
		std::string username;
		std::string realname;
        std::map<std::string, Channel *> joined;

        Client(int fd/*, short *events_ref*/)
        {
            this->fd = fd;
            //events = events_ref;
            rd_buff = "";
            wr_buff = "";
			registered = false;
			passed = false;
			nicked = false;
			usernamed = false;
            //events = NULL;
        }
        Client(const Client &other)
        {
            fd = other.fd;
            rd_buff = other.rd_buff;
            wr_buff = other.wr_buff;
			registered = other.registered;
			passed = other.passed;
			nicked = other.nicked;
			usernamed = other.usernamed;
            //events = other.events;
        }
        Client &operator=(const Client &other)
        {
            fd = other.fd;
            rd_buff = other.rd_buff;
            wr_buff = other.wr_buff;
			registered = other.registered;
			passed = other.passed;
			nicked = other.nicked;
			usernamed = other.usernamed;
           //events = other.events;
            return *this;
        }
        ~Client() {}
        int GetFD() { return fd; }
        std::string GetReadBuffer() { return rd_buff; }
        std::string GetWriteBuffer() { return wr_buff; }

        void ClearReadBuffer() { rd_buff = ""; }
        void AddToWriteBuffer(std::string msg)
        {
            //*events |= POLLOUT;
            wr_buff += msg; }

        ssize_t Read();
        ssize_t Send();
};
#endif
