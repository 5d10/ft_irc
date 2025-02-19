#ifndef TASK_HPP
# define TASK_HPP

# include "ft_irc.hpp"
# include "Server.hpp"
# include "Client.hpp"
# include "utils.hpp"
# define RPL_NOTOPIC(client, chnl) (":localhost 331 " + client + ' ' + chnl + " :No topic is set\r\n")
# define RPL_TOPIC(client, chnl, topic) (":localhost 332 " + client + ' ' + chnl + " :" + topic + "\r\n")
# define ERR_NOSUCHNICK(client, bad_nickname) (":localhost 401 " + client + ' ' + bad_nickname + " :No such nick/channel\r\n")
# define RPL_NAMREPLY(client, chnl, lst) (":localhost 353 " + client + ' ' + chnl + " :" + lst + "\r\n")
# define ERR_NOSUCHCHANNEL(client, chnl) (":localhost 403 " + client + ' ' + chnl + " :No such channel\r\n")
# define ERR_UNKNOWNCOMMAND(client, cmd) (":localhost 421 " + client + ' '+ cmd + " :Unknown command\r\n")
# define ERR_NONICKNAMEGIVEN(client) (":localhost 431 " + client + " :No nickname given\r\n")
# define ERR_NICKNAMEINUSE(client,nck) (":localhost 433 " + client + ' ' + nck + " :Nickname is already in use\r\n")
# define ERR_NICKCOLLISION(client, nck) (":localhost 436 " + client + ' ' + nck + " :Nickname collision KILL\r\n")
# define ERR_SUMMONDISABLED(client) (":localhost 445 " + client + " :SUMMON has been disabled\r\n")
# define ERR_NOTREGISTERED(client) (":localhost 451 " + client + " :You have not registered\r\n")
# define ERR_USERSDISABLED(client) (":localhost 446 " + client + ":USERS has been disabled\r\n")
# define ERR_NEEDMOREPARAMS(client, cmd) (":localhost 461 " + client + ' ' + cmd + " :Not enough parameters\r\n")
# define ERR_ALREADYREGISTRED(client) (":localhost 462 " + client + " :You may not reregister\r\n")
# define ERR_PASSWDMISMATCH(client) (":localhost 464 " +  client + " :Password incorrect\r\n")
# define ERR_CHANNELISFULLL(client, chnl) (":localhost 471 " + client + ' ' + chnl + " :Cannot join channel (+l)\r\n")
# define ERR_INVITEONLYCHAN(client, chnl) (":localhost 473 " + client + ' ' + chnl + " :Cannot join channel (+i)\r\n")
# define ERR_BADCHANNELKEY(client,chnl) (":localhost 475 " + client + ' ' + chnl + " :Cannot join channel (+k)\r\n")

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
		void nick(Client &c, Server &s);
		void user(Client &c);
        void quit(Client &c, Server &s);
		void join(Client &c, Server &s);
        void privmsg(Client &c, Server &s);
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
