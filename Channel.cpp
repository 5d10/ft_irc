#include "Channel.hpp"

Channel::Channel(std::string channelName, std::string user)
{
	name = channelName;
	isOperator[user] = true;
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
	std::map<std::string, bool>::iterator it = isOperator.begin();
	while (it != isOperator.end())
	{
		if (isOperator[it->first])
			s += "@";
		s += it->first;
		it++;
		if (it != isOperator.end())
			s += ",";
	}
}

void Channel::addUser(std::string name)
{
    isOperator[name] = false;
}

bool Channel::isValidChannelName(std::string name)
{
    if (name.size() > 200)
		return false;
	if (name[0] != '#' || name[0] != '&')
		return false;
	for (unsigned int i = 1; i < name.size(); i++)
		if (name[i] == ' ' || name[i] == ',' || name[i] == 7)
			return false;
	return true;
}
