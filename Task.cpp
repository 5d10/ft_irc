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
    while (fullCmd.size() > 0)
    {
        if (std::isspace(fullCmd[0]))//my brother in christ, we only want to care about ' '
        {
            unsigned int newStart = 0;
            while (newStart < fullCmd.size() && std::isspace(fullCmd[newStart]))
                newStart++;
            fullCmd = newStart < fullCmd.size() ? fullCmd.substr(newStart, fullCmd.size()) : "";
        }
        if (fullCmd.size() > 0)
        {
            unsigned int newStart = 0;
            while (newStart < fullCmd.size() && !std::isspace(fullCmd[newStart]))
                newStart++;
            split.push_back(fullCmd.substr(0, newStart));
            fullCmd = fullCmd.substr(newStart, fullCmd.size());
        }
    }
    cmd = split[0];
    args.reserve(split.size());
    args.insert(args.begin(), split.begin() + 1, split.end());
	#if DEBUG
    std::cout << "COMMAND: " << cmd << std::endl;
    std::cout << "ARGS: " << std::endl;
	#endif
    for (unsigned int i = 0; i < args.size(); i++)
        std::cout << "- " << args[i] << std::endl;
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
        // struct timeval time;
	    // gettimeofday(&time, NULL);
        // unsigned long msec = (time.tv_sec * 1000) + (time.tv_usec / 1000);
        std::string pongMsg = ":nick1!user@localhost PONG :";
        if (hasLagPrefix)
            pongMsg += "LAG";
        // unsigned long div = 1;
        // while (msec / div >= 10)
        //     div *= 10;
        // while (div > 0)
        // {
        //     pongMsg += '0' + (msec / div % 10);
        //     div /= 10;
        // }
        pongMsg += args[0].substr(hasLagPrefix ? 3 : 0, args[0].size());
        pongMsg += "\r\n";
        std::cout << pongMsg;
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
		c.AddToWriteBuffer(ERR_PASSWDMISMATCH(c.nickname));//!PLEASE replace with an attempt to get the nonexisting name
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
		passwords = string_split(args[1], ',');//consucutive ',' generate many entries, which we want
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
			s.channels.insert(std::pair<std::string, Channel>(joining[i], Channel(joining[i], c.nickname, s.registered)));//! make sure the default constructor initializaes everything to 0
			//s.channels[joining[i]].isOperator[c.nickname] = true;
			//? RPL_NOTOPIC is not said to be a possible reply of JOIN, yet it exists for other commands.
				//? Is it possible for complete servers to unset an hypothetical default topic to achive a non-topic?
			c.AddToWriteBuffer(RPL_NOTOPIC(c.nickname, joining[i]));
			c.AddToWriteBuffer(RPL_NAMREPLY(c.nickname, joining[i], s.channels.at(joining[i]).getUserList()));
		}
		else
		{//the channel does exist
			//? we should think whether Channel::addUser is called only if we know we want to add it or let it
				//? check that itself AND send the numeric replies if needed
			#if DEBUG
				std::cout << "JOIN: trying to join channel " << joining[i] << std::endl;
			#endif
			Channel* attempting = &(s.channels.at(joining[i]));
			std::list<std::string>::iterator invitation = list_find(attempting->invitedUsers, c.nickname);

			#if DEBUG
				std::cout << "JOIN: isInviteOnly = " << attempting->isInviteOnly << std::endl;
			#endif
			if (attempting->userLimit && attempting->userLimit <= static_cast<ssize_t>(attempting->isOperator.size()))
				c.AddToWriteBuffer(ERR_CHANNELISFULLL(c.nickname, joining[i]));
			else if (attempting->isInviteOnly && invitation == attempting->invitedUsers.end())
				c.AddToWriteBuffer(ERR_INVITEONLYCHAN(c.nickname, joining[i]));
			//subject does not require us for bans
			else if (attempting->isPasswordNeeded && (passwords.size() < i || passwords[i] != attempting->password))
				c.AddToWriteBuffer(ERR_BADCHANNELKEY(c.nickname, joining[i]));
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
				c.AddToWriteBuffer(RPL_TOPIC(c.nickname, joining[i], s.channels.at(joining[i]).topic));
				c.AddToWriteBuffer(RPL_NAMREPLY(c.nickname, joining[i], s.channels.at(joining[i]).getUserList()));
			}
		}
		#if DEBUG
			std::cout << "JOIN: done"  << std::endl;
		#endif
	}
}

//! we crash when hexchat is closed with stablished connections, I don't know where tho
void Task::quit(Client &c, Server &s)
{
	std::string quit_message;
	if (args.size() < 1)
		quit_message = "[username] has left the chat"; //* We need to agree on a default message
	else
		quit_message = args[0];

	#if DEBUG
		std::cout << "QUIT: message is `" << "QUIT " << quit_message << '\'' << std::endl;
	#endif
	s.EraseClient(c, "QUIT " + quit_message);
	#if DEBUG
		std::cout << "QUIT: success" << std::endl;
	#endif
}

void Task::run(Client &c, Server &s)
{
	if (cmd == "CAP")
		return;
	if (cmd == "QUIT")
		quit(c, s);
	else if (cmd == "PASS") {
		pass(c, s);
		goto validate; } // Enjoy the goto -glopez-m
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
	else if (cmd == "USERS")
		c.AddToWriteBuffer(ERR_USERSDISABLED(c.nickname));
	else if (cmd == "SUMMON")
		c.AddToWriteBuffer(ERR_SUMMONDISABLED(c.nickname));
	else if (c.registered)
		c.AddToWriteBuffer(ERR_UNKNOWNCOMMAND(c.nickname, cmd));
	return;

	validate:
	if (!c.registered && c.passed && c.nicked && c.usernamed)
	{
		#if DEBUG
			std::cout << "Server: debug: client registered" << std::endl;
		#endif
		c.registered= true;
	}
	#if DEBUG
	else
		std::cout << "Server: debug: client cannot be registered yet ("<< c.passed << c.nicked << c.usernamed  << ')' << std::endl;
	#endif
}

void Task::run(std::string fullCmd, Client &c, Server &s)
{
    Task(fullCmd).run(c, s);
}
