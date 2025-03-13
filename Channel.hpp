#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "ft_irc.hpp"

#include "Client.hpp"

class Channel
{
    private:
		//WHOOPS all public -glopez-m

    public:
        std::string name;
        std::string password;
        bool isPasswordNeeded;
        std::map<std::string, bool> isOperator; // doubles as client list (keys) and operator flags (values)
        std::map<std::string, Client *>& serverClients;
        std::list<std::string> invitedUsers;
        bool isInviteOnly;
        std::string topic;
        bool isTopicCommandOpOnly;
        ssize_t userLimit;
        

        Channel(std::string channelName, std::string user, std::map<std::string, Client *>& client_direction);
        Channel(const Channel &other);
        Channel &operator=(const Channel &other);
        ~Channel();

        bool containsUser(std::string user) const;
        std::string getUserList() const;
        void addUser(std::string name);
        void removeUser(std::string name);
        void broadcast(std::string msg) const;
        void broadcast(std::string msg, std::string sender) const;
        static bool isValidChannelName(std::string name);
		void Rename(std::string old_name, std::string new_name);
};

#endif
