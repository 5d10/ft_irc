#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "ft_irc.hpp"

#include "Client.hpp"

class Channel
{
    private:
        std::string name;
        std::string password;
        bool isPasswordNeeded; // ?
        std::map<std::string, bool> isOperator; // doubles as client list (keys) and operator flags (values)
        std::list<std::string> invitedUsers;
        bool isInviteOnly;
        std::string topic;
        bool isTopicCommandOpOnly;
        ssize_t userLimit;
        std::map<std::string, Client&>& clients;
    public:
        Channel(std::string channelName, std::string user, std::map<std::string, Client&>& client_direction);
        Channel(const Channel &other);
        Channel &operator=(const Channel &other);
        ~Channel();

        bool containsUser(std::string user) const;
        std::string getUserList() const;
        void addUser(std::string name);
        void removeUser(std::string name);
        void broadcast(std::string msg) const;
        static bool isValidChannelName(std::string name);
};

#endif
