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
	(void)c;
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
		c.AddToWriteBuffer(ERR_PASSWDMISMATCH(c.nickname));//PLEASE replace with an attempt to get the nonexisting name
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
	
	if (s.registered.find(args[0]) != s.registered.end())
	{
		#if DEBUG
			std::cout << "NICK: nickname collision" << std::endl;
		#endif
		c.AddToWriteBuffer(ERR_NICKCOLLISION(c.nickname, args[0]));
		return;
	}
	c.nickname = args[0];
	#if DEBUG
		std::cout << "NICK: success" << std::endl;
	#endif

	/*
	std::list<Client>::iterator i = s.clients.begin();
	const std::list<Client>::iterator end = s.clients.end();
	while (i != end)
	{
		if (i->nickname == args[0])
		{
			#if DEBUG
				std::cout << "NICK: nickname collision" << std::endl;
			#endif
			c.AddToWriteBuffer(ERR_NICKCOLLISION(c.nickname, args[0]));
			return;
		}
		++i;
	}
	c.nickname = args[0];
	#if DEBUG
		std::cout << "NICK: success" << std::endl;
	#endif*/
}

void Task::user(Client &c, Server &s)
{
	if (args.size() < 4)
		c.AddToWriteBuffer(ERR_NEEDMOREPARAMS(c.nickname, "PASS"));
	//TODO else if () check for ERR_ALREADYREGISTERED
}

void Task::run(Client &c, Server &s)
{
    if (cmd == "PING")
        ping(c);
	else if (cmd == "NICK")
		nick(c, s);
	else if (cmd == "PASS")
		pass(c, s);
	else if (cmd == "PASS")
		pass(c, s);
}

void Task::run(std::string fullCmd, Client &c, Server &s)
{
    Task(fullCmd).run(c, s);
}
