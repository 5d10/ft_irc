#ifndef TASK_HPP
# define TASK_HPP

# include "ft_irc.hpp"
# include "Server.hpp"
# include "Client.hpp"

# define ERR_PASSWDMISMATCH(client) (":localhost 464 " +  client + " :Password incorrect\r\n")
# define ERR_ALREADYREGISTRED(client) (":localhost 462 " + client + " :You may not reregister\r\n")
# define ERR_NEEDMOREPARAMS(cmd, client) (":localhost 461 " + client + ' ' + cmd + " :Not enough parameters\r\n")

class Server;

class Task
{
    private:
        // enum e_types { UNKNOWN, PASS, NICK, USER, PING, JOIN, PRIVMSG, PART, QUIT, KICK, INVITE, TOPIC, MODE }; // might not even be necessary: if/else? map str to function? (one func per command type? no class, cut the middle man)
        std::string cmd;
        std::vector<std::string> args;

        void parse(std::string fullCmd);
        // e_types getType();

        void ping(Client &c);
		void pass(Client &c, Server &s);
    public:
        Task();
        Task(std::string fullCmd);
        Task(const Task &other);
        Task &operator=(const Task &other);
        ~Task();

        void run(Client &c, Server &s);
        static void run(std::string fullCmd, Client &c, Server &s);
};

#endif
