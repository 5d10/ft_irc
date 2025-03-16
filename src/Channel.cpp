#include "Channel.hpp"

Channel::Channel(std::string channelName, std::string user, std::map<std::string, Client *>& client_direction) : serverClients(client_direction)
{
	name = channelName;
	isPasswordNeeded = false;
	isInviteOnly = false;
	isOperator[user] = true;
	userLimit = 2;
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
	isInviteOnly = other.isInviteOnly;
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
    return isOperator.find(name) != isOperator.end();
}

std::string Channel::getUserList() const
{
	std::string s = "";
	std::map<std::string, bool>::const_iterator it = isOperator.begin();
	while (it != isOperator.end())
	{
		if (it->second)
			s += '@';
		s += it->first;
		it++;
		if (it != isOperator.end())
			s += ' ';
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
	//* would be nice if erasure from invitedUsers was done as well
	/*
		imagine you are a channop and you want to invite someone to your invite only chan,
		then you invite the wrong person, but it doesn't join in right away,
		you might want to remove said person from the invited list since they aren't on the channel.
	*/
}

void Channel::broadcast(std::string msg) const
{
	std::map<std::string, Client *>::const_iterator i  = serverClients.begin();
	std::map<std::string, Client *>::const_iterator end  = serverClients.end();

	#if DEBUG
		std::cout << "Broadcasting(1) to: ";
	#endif
	while (i != end)
	{
		#if DEBUG
			std::cout << serverClients.at(i->first)->nickname << ", ";
		#endif
		serverClients.at(i->first)->AddToWriteBuffer(msg);
		++i;
	}
	#if DEBUG
		std::cout << std::endl;
	#endif
}

void Channel::broadcast(std::string msg, std::string sender) const
{
	std::map<std::string, Client *>::const_iterator i  = serverClients.begin();
	std::map<std::string, Client *>::const_iterator end  = serverClients.end();

	#if DEBUG
		std::cout << "Broadcasting(2) to: ";
	#endif
	while (i != end)
	{
		#if DEBUG
			std::cout << serverClients.at(i->first)->nickname << ", ";
		#endif
		if (i->first != sender)
			serverClients.at(i->first)->AddToWriteBuffer(msg);
		++i;
	}
	#if DEBUG
		std::cout << std::endl;
	#endif
}


bool Channel::isValidChannelName(std::string name)
{
    if (name.size() < 2 || name.size() > 200)
		return false;
	if (name[0] != '#' && name[0] != '&' && name[0] != '!')
		return false;
	for (unsigned int i = 1; i < name.size(); i++)
		if (name[i] == ' ' || name[i] == ',' || name[i] == 7)
			return false;
	return true;
}

void Channel::Rename(std::string old_name, std::string new_name)
{
	if (isOperator.find(old_name) != isOperator.end())
	{
		bool temp = isOperator.at(old_name);
		isOperator.erase(old_name);
		isOperator.insert(std::pair<std::string, bool>(new_name, temp));
	}
	else
	{
		std::list<std::string>::iterator i = invitedUsers.begin();
		std::list<std::string>::iterator end = invitedUsers.end();
		while (i != end)
		{
			if (*i == old_name)
			{
				*i = new_name;
				break;
			}
			++i;
		}
	}
}
