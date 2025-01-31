#include "Channel.hpp"

Channel::Channel(std::string channelName, std::string user, std::map<std::string, Client&>& client_direction) : serverClients(client_direction)
{
	name = channelName;
	isInviteOnly = channelName[0] == '&'; // ?
	isOperator[user] = true;
}

 Channel::Channel(const Channel &other): serverClients(other.serverClients)
 {
 	*this = other;
 }

 Channel &Channel::operator=(const Channel &other)
 {
 	name = other.name;
	password = other.password;
	isPasswordNeeded = other.isPasswordNeeded;
	isOperator = other.isOperator;
	invitedUsers = other.invitedUsers;
	topic = other.topic;
	isTopicCommandOpOnly = other.isTopicCommandOpOnly;
	userLimit = other.userLimit;
	return *this;
 }

Channel::~Channel()
{

}

bool Channel::containsUser(std::string name) const
{
	//*maybe we should consider getting rid of this or making it an always inline
    return isOperator.find(name) != isOperator.end();
}

std::string Channel::getUserList() const
{
	std::string s = "";
	std::map<std::string, bool>::const_iterator it = isOperator.begin();
	while (it != isOperator.end())
	{
		if (it->second)
			s += "@";
		s += it->first;
		it++;
		if (it != isOperator.end())
			s += ",";
	}
	return s;
}

void Channel::addUser(std::string name)
{
    isOperator[name] = false;
}

void Channel::removeUser(std::string name)
{
	isOperator.erase(name);
}

void Channel::broadcast(std::string msg) const
{
	std::map<std::string, Client&>::const_iterator i  = serverClients.begin();
	std::map<std::string, Client&>::const_iterator end  = serverClients.end();

	while (i != end)
	{
		serverClients.at(i->first).AddToWriteBuffer(msg);
		++i;
	}
}

bool Channel::isValidChannelName(std::string name)
{
    if (name.size() < 2 || name.size() > 200)
		return false;
	if (name[0] != '#' || name[0] != '&')
		return false;
	for (unsigned int i = 1; i < name.size(); i++)
		if (name[i] == ' ' || name[i] == ',' || name[i] == 7)
			return false;
	return true;
}
