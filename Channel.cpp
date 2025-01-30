#include "Channel.hpp"

Channel::Channel(std::string channelName, std::string user, std::map<std::string, const Client&>& client_direction)
{
	name = channelName;
	isInviteOnly = channelName[0] == '&'; // ?
	isOperator[user] = true;
	clients = client_direction;
}

Channel::~Channel()
{

}

bool Channel::containsUser(std::string name) const
{
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

void Channel::broadcast(std::string msg)
{
	std::map<std::string, bool>::iterator i  = isOperator.begin();
	std::map<std::string, bool>::iterator end  = isOperator.end();

	while (i != end)
	{
		clients.at(i->first).AddToWriteBuffer(msg);
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
