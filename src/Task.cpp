#include "Task.hpp"

Task::Task()
{
    cmd = "";
}

Task::Task(std::string fullCmd)
{
    parse(fullCmd);
}

Task::Task(const Task &other)
{
    cmd = other.cmd;
    args = other.args;
}

Task &Task::operator=(const Task &other)
{
    cmd = other.cmd;
    args = other.args;
    return *this;
}

Task::~Task()
{

}

void Task::parse(std::string fullCmd)
{
    std::vector<std::string> split;
	std::size_t lastArgStart = fullCmd.find(" :");
	std::string lastArg;

	fullCmd.resize(fullCmd.size() - 2);

	if (lastArgStart != std::string::npos) {
		lastArg = fullCmd.substr(lastArgStart + 2);
		fullCmd.erase(lastArgStart);
	}
    while (fullCmd.size() > 0)
    {
        if (fullCmd[0] == ' ')
        {
            unsigned int newStart = 0;
            while (newStart < fullCmd.size() && fullCmd[newStart] == ' ')
                newStart++;
            fullCmd = newStart < fullCmd.size() ? fullCmd.substr(newStart, fullCmd.size()) : "";
        }
        if (fullCmd.size() > 0)
        {
            unsigned int newStart = 0;
            while (newStart < fullCmd.size() && fullCmd[newStart] != ' ')
                newStart++;
            split.push_back(fullCmd.substr(0, newStart));
            fullCmd = fullCmd.substr(newStart, fullCmd.size());
        }
    }
    cmd = split[0];
	if (lastArgStart != std::string::npos)
	{
		args.reserve(split.size() + 1);
  		args.insert(args.begin(), split.begin() + 1, split.end());
		args.push_back(lastArg);
	} else {
		args.reserve(split.size());
		args.insert(args.begin(), split.begin() + 1, split.end());
	}
	#if DEBUG
    std::cout << "COMMAND: " << cmd << std::endl;
    std::cout << "ARGS: " << std::endl;
    for (unsigned int i = 0; i < args.size(); i++)
        std::cout << "- " << args[i] << std::endl;
	#endif
}

void Task::ping(Client &c)
{
    if (args.size() != 1)
        return;
    bool isValidArg = true;
    bool hasLagPrefix = args[0].rfind("LAG", 0) == 0;
    for (unsigned int i = hasLagPrefix ? 3 : 0; i < args[0].size() && isValidArg; i++)
        isValidArg = std::isdigit(args[0][i]);
    if (isValidArg)
    {
        std::string pongMsg = ":" + c.nickname + "!" + c.username + "@localhost PONG :";
        if (hasLagPrefix)
            pongMsg += "LAG";
        pongMsg += args[0].substr(hasLagPrefix ? 3 : 0, args[0].size());
        pongMsg += "\r\n";
        c.AddToWriteBuffer(pongMsg);
    }
}

void Task::pass(Client &c, Server &s)
{
	if (args.size() < 1)
	{
		#if DEBUG
			std::cout << "PASS: no password given" << std::endl;
		#endif
		c.AddToWriteBuffer(ERR_NEEDMOREPARAMS(c.nickname, "PASS"));
	}
	else if (c.passed)
	{
		#if DEBUG
			std::cout << "PASS: already passed" << std::endl;
		#endif
		c.AddToWriteBuffer(ERR_ALREADYREGISTRED(c.nickname));
	}
	else if (args[0] == s.password)
	{
		#if DEBUG
			std::cout << "PASS: password match" << std::endl;
		#endif
		c.passed = true;
	}
	else
	{
		#if DEBUG
			std::cout << "PASS: wrong password" << std::endl;
		#endif
		c.AddToWriteBuffer(ERR_PASSWDMISMATCH(c.nickname));
	}
}

void Task::nick(Client &c, Server &s)
{
	if (args.size() < 1)
	{
		#if DEBUG
			std::cout << "NICK: no nickname given" << std::endl;
		#endif
		c.AddToWriteBuffer(ERR_NONICKNAMEGIVEN(c.nickname));
		return;
	}

	std::list<Client>::iterator i = s.clients.begin();
	const std::list<Client>::iterator end = s.clients.end();
	while (i != end)
	{
		if (i->nickname == args[0])
		{
			#if DEBUG
				std::cout << "NICK: nickname in use" << std::endl;
			#endif
			c.AddToWriteBuffer(ERR_NICKNAMEINUSE(c.nickname, args[0]));
			return;
		}
		++i;
	}
	s.Rename(c, args[0]);
	c.nicked = true;
	#if DEBUG
		std::cout << "NICK: success" << std::endl;
	#endif
}

void Task::user(Client &c)
{
	if (c.usernamed) {
		c.AddToWriteBuffer(ERR_ALREADYREGISTRED(c.nickname));
		return; }
	if (args.size() < 4) {
		c.AddToWriteBuffer(ERR_NEEDMOREPARAMS(c.nickname, cmd));
		return; }
	c.username = args[0];
	c.realname = args[3];
	c.usernamed = true;
}

void Task::join(Client &c, Server &s)
{
	if (args.size() < 1) {
		c.AddToWriteBuffer(ERR_NEEDMOREPARAMS(c.nickname, cmd));
		return; }

	std::vector<std::string> joining = string_split(args[0], ',');
	std::vector<std::string> passwords;
	if (1 < args.size())
		 passwords = string_split(args[1], ',');//consecutive ',' generate many entries, which we want
	unsigned int i = joining.size();
	while (i--)
	{
		if (!Channel::isValidChannelName(joining[i]))
		{
			#if DEBUG
				std::cout << "JOIN: invalid channel name" << std::endl;
			#endif
			c.AddToWriteBuffer(ERR_NOSUCHCHANNEL(c.nickname, joining[i]));
		}
		else if (s.channels.find(joining[i]) == s.channels.end())
		{//the channel does not exist yet
			#if DEBUG
				std::cout << "JOIN: creating channel " << joining[i] << std::endl;
			#endif
			s.channels.insert(std::pair<std::string, Channel>(joining[i], Channel(joining[i], c.nickname, s.registered)));
			c.AddToWriteBuffer(":" + c.nickname + '!' + c.username + "@localhost" + " JOIN :" + joining[i] + "\r\n");
			c.AddToWriteBuffer(RPL_NOTOPIC(c.nickname, joining[i]));
			c.AddToWriteBuffer(RPL_NAMREPLY(c.nickname, joining[i], s.channels.at(joining[i]).getUserList()));
		}
		else
		{//the channel does exist
			#if DEBUG
				std::cout << "JOIN: trying to join channel " << joining[i] << std::endl;
			#endif
			Channel* attempting = &(s.channels.at(joining[i]));
			std::list<std::string>::iterator invitation = list_find(attempting->invitedUsers, c.nickname);

			#if DEBUG
				std::cout << "JOIN: isInviteOnly = " << attempting->isInviteOnly << std::endl;
				std::cout << "JOIN: userLimit = " << attempting->userLimit << std::endl;
				std::cout << "JOIN: userCount = " << static_cast<ssize_t>(attempting->isOperator.size()) << std::endl;
				std::cout << "JOIN: pwdNeeded = " << attempting->isPasswordNeeded << std::endl;
				std::cout << "JOIN: givenPwds = " << passwords.size() << std::endl;
				std::cout << "JOIN: isPasswordNeeded = " << attempting->isPasswordNeeded << std::endl;
				std::cout << "JOIN: hasInorrectPwd = " << (passwords.size() <= i || passwords[i] != attempting->password) << std::endl;
			#endif
			//subject does not require us for bans
			if (attempting->userLimit && attempting->userLimit <= static_cast<ssize_t>(attempting->isOperator.size()))
				c.AddToWriteBuffer(ERR_CHANNELISFULLL(c.nickname, joining[i]));
			else if (attempting->isInviteOnly && invitation == attempting->invitedUsers.end())
				c.AddToWriteBuffer(ERR_INVITEONLYCHAN(c.nickname, joining[i]));
			else if (attempting->isPasswordNeeded && (passwords.size() <= i || passwords[i] != attempting->password))
			{
				#if DEBUG
					std::cout << "JOIN: Sending BADCHANNELKEY" << std::endl;
				#endif
			 	c.AddToWriteBuffer(ERR_BADCHANNELKEY(c.nickname, joining[i]));
			}
			else
			{//join in
				#if DEBUG
					std::cout << "JOIN: joining channel " << joining[i] << std::endl;
				#endif
				attempting->addUser(c.nickname);
				if (attempting->isInviteOnly)
				{
					#if DEBUG
						std::cout << "JOIN: clipping invite of " << *invitation << std::endl;
					#endif
					attempting->invitedUsers.erase(invitation);
				}
				c.AddToWriteBuffer(":" + c.nickname + '!' + c.username + "@localhost" + " JOIN :" + joining[i] + "\r\n");
				if (attempting->topic.empty())
					c.AddToWriteBuffer(RPL_NOTOPIC(c.nickname, joining[i]));
				else
					c.AddToWriteBuffer(RPL_TOPIC(c.nickname, joining[i], attempting->topic));
				c.AddToWriteBuffer(RPL_NAMREPLY(c.nickname, joining[i], s.channels.at(joining[i]).getUserList()));
				c.AddToWriteBuffer(RPL_ENDOFNAMES(c.nickname, joining[i]));
			}
		}
		#if DEBUG
			std::cout << "JOIN: done"  << std::endl;
		#endif
	}
}

void Task::quit(Client &c, Server &s)
{
	std::string quit_message = ':' + c.nickname + '!' + c.username + "@localhost QUIT :";
	if (args.size() < 1)
		quit_message += c.nickname + " has left the chat\r\n";
	else
		quit_message += args[0] + "\r\n";

	#if DEBUG
		std::cout << "QUIT: message is `" << "QUIT " << quit_message << '\'' << std::endl;
	#endif
	s.EraseClient(c, quit_message);
	#if DEBUG
		std::cout << "QUIT: success" << std::endl;
	#endif
}

//*Numeric replies
//*  ERR_NORECIPIENT                 ERR_NOTEXTTOSEND
//*  ERR_CANNOTSENDTOCHAN            ERR_NOTOPLEVEL
//*  ERR_WILDTOPLEVEL                ERR_TOOMANYTARGETS
//*  ERR_NOSUCHNICK
//*  RPL_AWAY
void Task::privmsg(Client &c, Server &s)
{
	#if DEBUG
		std::cout << "ENTERING PRIVMSG" << std::endl;
	#endif
	std::vector<std::string> targets;
	if (args.size() < 2) {
		c.AddToWriteBuffer(ERR_NEEDMOREPARAMS(c.nickname, "PRIVMSG"));
	}
	targets = string_split(args[0], ',');
	for (size_t i = 0; i < targets.size(); i++)
	{
		if (s.registered.find(targets[i]) != s.registered.end())
		{
			s.registered.at(targets[i])->AddToWriteBuffer(":" + c.nickname + " PRIVMSG " + targets[i] + " :"+ args[1] + "\r\n");
			continue;
		}
		std::string temp = targets[i];
		if (temp.find(":localhost") != std::string::npos)
			temp.erase(temp.find(":localhost"));
		if (s.channels.find(temp) != s.channels.end()) {
			if (s.channels.at(temp).isOperator.find(c.nickname) != s.channels.at(temp).isOperator.end())
				s.channels.at(temp).broadcast(":" + c.nickname + " PRIVMSG " + targets[i] + " :" + args[1] + "\r\n", c.nickname);
		} else {
			c.AddToWriteBuffer(ERR_NOSUCHNICK(c.nickname, targets[i]));
		}
	}
}

void Task::part(Client &c, Server &s)
{
	#if DEBUG
		std::cout << "PART: started" << std::endl;
	#endif
	if (args.size() < 1) {
		c.AddToWriteBuffer(ERR_NEEDMOREPARAMS(c.nickname, "PART"));
		return;
	}
	std::vector<std::string> channels = string_split(args[0], ',', STRSPL_BUNDLE);
	for (std::vector<std::string>::iterator i = channels.begin(), c_end = channels.end();
		i != c_end; ++i)
	{
		#if DEBUG
			std::cout << "PART: trying channel ";
			std::cout << *i  << std::endl;
		#endif
		std::map<std::string, Channel>::iterator ch_search = s.channels.find(*i);
		if (ch_search == s.channels.end()) {
			c.AddToWriteBuffer(ERR_NOSUCHCHANNEL(c.nickname, *i));
			continue;
		}
		#if DEBUG
			std::cout << "PART: confirmed channel existance" << std::endl;
		#endif
		std::map<std::string, bool>::iterator user_search = ch_search->second.isOperator.find(c.nickname);
		if (user_search == ch_search->second.isOperator.end()) {
			c.AddToWriteBuffer(ERR_NOTONCHANNEL(c.nickname, ch_search->first));
			continue;
		}
		#if DEBUG
			std::cout << "PART: confirmed parter is in channel" << std::endl;
			std::cout << "PART: parting" << std::endl;
		#endif
		std::string message = ':' + c.nickname + '!' + c.username + "@localhost PART " + ch_search->first;
		if (1 < args.size())
			message += " :" + args[1];
		#if DEBUG
			std::cout << "PART: sending: " << message << std::endl;
		#endif
		message += "\r\n";
		ch_search->second.broadcast(message);
		ch_search->second.removeUser(c.nickname);
		#if DEBUG
			std::cout <<  "PART: done with channel" << std::endl;
		#endif
		if (ch_search->second.isOperator.empty())
			s.channels.erase(ch_search);
	}
	#if DEBUG
		std::cout << "PART: done" << std::endl;
	#endif
}

void Task::kick(Client &c, Server &s)
{ 
	#if DEBUG
		std::cout << "KICK: started" << std::endl;
	#endif
	if (args.size() < 2) {
		c.AddToWriteBuffer(ERR_NEEDMOREPARAMS(c.nickname, "KICK"));
		return;
	}
	std::vector<std::string> channels = string_split(args[0], ',', STRSPL_BUNDLE);
	std::vector<std::string> targets = string_split(args[1], ',', STRSPL_BUNDLE);
	for (std::vector<std::string>::iterator i = channels.begin(), c_end = channels.end();
		i != c_end; ++i)
	{
		#if DEBUG
			std::cout << "KICK: trying channel ";
			std::cout << *i  << std::endl;
		#endif
		std::map<std::string, Channel>::iterator ch_search = s.channels.find(*i);
		if (ch_search == s.channels.end()) {
			c.AddToWriteBuffer(ERR_NOSUCHCHANNEL(c.nickname, *i));
			continue;
		}
		#if DEBUG
			std::cout << "KICK: confirmed channel existance" << std::endl;
		#endif
		std::map<std::string, bool>::iterator user_search = ch_search->second.isOperator.find(c.nickname);
		if (user_search == ch_search->second.isOperator.end()) {
			c.AddToWriteBuffer(ERR_NOTONCHANNEL(c.nickname, ch_search->first));
			continue;
		}
		#if DEBUG
			std::cout << "KICK: confirmed kicker is in channel" << std::endl;
		#endif
		if (!user_search->second) {
			c.AddToWriteBuffer(ERR_CHANOPRIVSNEEDED(c.nickname, ch_search->first));
			continue;
		}
		#if DEBUG
			std::cout << "KICK: confirmed privileges" << std::endl;
			std::cout << "KICK: there are " << targets.size() << " targets" << std::endl;
		#endif
		for (std::vector<std::string>::iterator j = targets.begin(), t_end = targets.end();
				j != t_end; ++j)
		{
			#if DEBUG
				std::cout << "KICK: checking presence of ";
				std::cout << *j << std::endl;
			#endif
			std::map<std::string, bool>::iterator target_search = ch_search->second.isOperator.find(*j);
			if (target_search == ch_search->second.isOperator.end()) {
				c.AddToWriteBuffer(ERR_USERNOTINCHANNEL(c.nickname, *j, *i));
				continue;
			}
			#if DEBUG
				std::cout << "KICK: kicking user " << *j << std::endl;
			#endif
			std::string message = ':' + c.nickname + '!' + c.username + "@localhost KICK " + ch_search->first + ' ' + *j;
			if (2 < args.size())
				message += " :" + args[2];
			#if DEBUG
				std::cout << "PART: sending: " << message << std::endl;
			#endif
			message += "\r\n";
			ch_search->second.broadcast(message);
			ch_search->second.removeUser(*j);
		}
		if (ch_search->second.isOperator.empty())
			s.channels.erase(ch_search);
		#if DEBUG
			std::cout <<  "KICK: done with channel" << std::endl;
		#endif
	}
	#if DEBUG
		std::cout << "KICK: done" << std::endl;
	#endif
}

static void _update_flag(bool& flag, char kind)
{
	switch (kind)
	{
		case '+':
			flag = true;
			break;
		case '-':
			flag = false;
			break;
		default:
			flag = !flag;
	}
}

void Task::mode(Client &c, Server &s)
{
	if (args.size() < 2 || args[1].empty()) {
		c.AddToWriteBuffer(ERR_NEEDMOREPARAMS(c.nickname, cmd));
		return;
	}
	std::map<std::string, Channel>::iterator ch_search = s.channels.find(args[0]);
	if (ch_search == s.channels.end()) {
		c.AddToWriteBuffer(ERR_NOSUCHCHANNEL(c.nickname, args[0]));
		return;
	}
	Channel& chan = ch_search->second;
	std::map<std::string, bool>::iterator user_search = chan.isOperator.find(c.nickname);
	if (user_search == chan.isOperator.end()) {
		c.AddToWriteBuffer(ERR_NOTONCHANNEL(c.nickname, args[0]));
		return; }
	if (!user_search->second) {
		c.AddToWriteBuffer(ERR_CHANOPRIVSNEEDED(c.nickname, args[0]));
		return;
	}
	char operation;
	if (args[1][0] == '+' || args[1][0] == '-')
	{//if +-[x]
		if (args[1].size() < 2) {
			c.AddToWriteBuffer(ERR_NEEDMOREPARAMS(c.nickname, cmd));
			return; }
		operation = args[1][1];
	}
	else
		operation = args[1][0];
	switch (operation)
	{
		case 'i':
			_update_flag(chan.isInviteOnly, args[1][0]);
			c.AddToWriteBuffer(RPL_CHANNELMODEIS(c.nickname, chan.name, 'i', (chan.isInviteOnly ? "true" : "false")));
			break;
		case 't':
			_update_flag(chan.isTopicCommandOpOnly, args[1][0]);
			c.AddToWriteBuffer(RPL_CHANNELMODEIS(c.nickname, chan.name, 't', (chan.isTopicCommandOpOnly ? "true" : "false")));
			break;
		case 'o':
			if (args.size() < 3) { 
				c.AddToWriteBuffer(ERR_NEEDMOREPARAMS(c.nickname, cmd));
				return; }
			{
				std::map<std::string, bool>::iterator target_search = chan.isOperator.find(args[2]);
				if (target_search == chan.isOperator.end()) {
					c.AddToWriteBuffer(ERR_NOSUCHNICK(c.nickname, args[2]));
					return; }
				_update_flag(target_search->second, args[1][0]);
			}
			//c.AddToWriteBuffer(RPL_CHANNELMODEIS(c.nickname, chan.name, '0', chan.isTopicCommandOpOnly);
			//Tengo que aclarar el caso donde la info es del usuario
				//RPL_UMODEIS existe, pero no tengo del todo claro el mensaje
			break;
		case 'k':
			if ((args[1][0] == '-') || (args[1][0] != '+' && args.size() < 3)) {
				chan.isPasswordNeeded = false;
				chan.password.clear();
				c.AddToWriteBuffer(RPL_CHANNELMODEIS(c.nickname, chan.name, 'k',  "\"\"(none)"));
			}
			else
			{
				if (args.size() < 3) {
					c.AddToWriteBuffer(ERR_NEEDMOREPARAMS(c.nickname, cmd));
					return; }
				if (chan.isPasswordNeeded)
					c.AddToWriteBuffer(ERR_KEYSET(c.nickname, chan.name));
				chan.isPasswordNeeded = true;
				chan.password = args[2];
				c.AddToWriteBuffer(RPL_CHANNELMODEIS(c.nickname, chan.name, 'k',  chan.password));
			}
			break;
		case 'l':
			if (args[1][0] == '-' || (args[1][0] != '+' && args.size() < 3))
				chan.userLimit = 0;
			else
			{
				if (args.size() < 3) {
					c.AddToWriteBuffer(ERR_NEEDMOREPARAMS(c.nickname, cmd));
					return; }
				chan.userLimit = std::atol(args[2].c_str());
			}
			c.AddToWriteBuffer(RPL_CHANNELMODEIS(c.nickname, chan.name, 'l', (chan.userLimit ? "to be implemented"/*have our own t_string*/:"(none)"));
			break;
		default:
			c.AddToWriteBuffer(ERR_UNKNOWNMODE(c.nickname, operation));
			return;
	}
}

//Returns TRUE when command caused the client to be deleted, false otherwise
bool Task::run(Client &c, Server &s)
{
	#if DEBUG
		std::cout << "Task client: " << c.nickname << std::endl;
	#endif
	if (cmd == "CAP")
		return (false);
	if (cmd == "QUIT") {
		quit(c, s);
		return (true);
	}
	else if (cmd == "PASS") {
		pass(c, s);
	}//! Ain't we missing a goto?
	else if (cmd == "NICK") {
		nick(c, s);
		goto validate; }
	else if (cmd == "USER") {
		user(c);
		goto validate; }
	else if (!c.registered)
	{ 
		#if DEBUG
				std::cout << "ANY: not registered" << std::endl;
		#endif
		c.AddToWriteBuffer(ERR_NOTREGISTERED(c.nickname));
	}
    else if (cmd == "PING")
        ping(c);
	else if (cmd == "JOIN")
		join(c, s);
	else if (cmd == "PRIVMSG")
		privmsg(c, s);
	else if (cmd == "KICK")
		kick(c, s);
	else if (cmd == "PART")
		part(c, s);
	else if (cmd == "MODE")
		mode(c, s);
	else if (cmd == "USERS")
		c.AddToWriteBuffer(ERR_USERSDISABLED(c.nickname));
	else if (cmd == "SUMMON")
		c.AddToWriteBuffer(ERR_SUMMONDISABLED(c.nickname));
	else if (c.registered)
		c.AddToWriteBuffer(ERR_UNKNOWNCOMMAND(c.nickname, cmd));
	return (false);

	//tbh this could be a method of Client
	validate:
	if (!c.registered && c.passed && c.nicked && c.usernamed)
	{
		#if DEBUG
			std::cout << "Server: debug: client registered" << std::endl;
		#endif
		c.registered= true;
		c.AddToWriteBuffer(":localhost 001 " + c.nickname + " :Welcome to the Internet Relay Network :" + c.nickname + "!" + c.username + "@localhost\r\n");
		c.AddToWriteBuffer(":localhost 376 " + c.nickname + " :End of /MOTD command.\r\n");
	}
	#if DEBUG
	else
		std::cout << "Server: debug: client cannot be registered yet ("<< c.passed << c.nicked << c.usernamed  << ')' << std::endl;
	#endif
	return (false);
}

//This will make me cry -G
bool Task::run(std::string fullCmd, Client &c, Server &s)
{
    return (Task(fullCmd).run(c, s));
}
