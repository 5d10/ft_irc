#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "ft_irc.hpp"

class Channel;

class Client
{
    private:
        int fd;
        std::string rd_buff;
        std::string wr_buff;
        //short *events;
        Client();
        std::string format_buffer(std::string buffer) const;
        void print_buffer(std::string buffer) const;

    public:
		bool registered;
		bool passed;
		bool nicked;
		bool usernamed;
		std::string nickname;
		std::string username;
		std::string realname;
        std::map<std::string, Channel *> joined;

        Client(int fd/*, short *events_ref*/);
        Client(const Client &other);
        Client &operator=(const Client &other);
        ~Client();
        const int &GetFD() const;
        const std::string &GetReadBuffer() const;
        const std::string &GetWriteBuffer() const;

        void ClearReadBuffer();
        void AddToWriteBuffer(std::string msg);

        ssize_t Read();
        ssize_t Send();
};

#endif
