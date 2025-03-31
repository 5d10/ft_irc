#ifndef TASK_HPP
# define TASK_HPP

# include "ft_irc.hpp"
# include "Server.hpp"
# include "Client.hpp"
# include "utils.hpp"
# define MSG_NICK(nck) (":localhost NICK " + nck + "\r\n")
# define MSG_USER(usr, realname) (":localhost USER " + usr + " 0 * :" + realname + "\r\n")

# define RPL_LUSERCLIENT(client, nusr) (":localhost 251 " + client + " :There are " + nusr + " users and 0 invisible on 1 servers\r\n")
# define RPL_LUSRME(client, nusr) (":localhost 255 " + client + " :I have " + nusr + " clients and 0 servers\r\n")
# define RPL_CHANNELMODEIS(client, chn, mode, params) (":localhost 324 " + client + ' ' + chn + ' ' + mode + ' ' + params + "\r\n")
# define RPL_UMODEIS(client, mode) (":localhost 221 " + client + ' ' + mode + "\r\n")

# define RPL_NOTOPIC(client, chnl) (":localhost 331 " + client + ' ' + chnl + " :No topic is set\r\n")
# define RPL_TOPIC(client, chnl, topic) (":localhost 332 " + client + ' ' + chnl + " :" + topic + "\r\n")
# define RPL_INVITING(client, nck, chnl) (":localhost 341 " + client + ' ' + nck + ' ' + chnl + "\r\n") //Either hexchat is wrong or the RFC is wrong
# define RPL_NAMREPLY(client, chnl, lst) (":localhost 353 " + client + ' ' + chnl + " :" + lst + "\r\n")
# define RPL_ENDOFNAMES(client, chnl) (":localhost 366 " + client + ' ' + chnl + " :End of /NAMES list\r\n")

# define ERR_NOSUCHNICK(client, bad_nickname) (":localhost 401 " + client + ' ' + bad_nickname + " :No such nick/channel\r\n")
# define ERR_NOSUCHCHANNEL(client, chnl) (":localhost 403 " + client + ' ' + chnl + " :No such channel\r\n")
# define ERR_CANNOTSENDTOCHAN(client, chnl) (":localhost 404 " + client + ' ' + chnl + " :Cannot send to channel\r\n")
# define ERR_NORECIPIENT(client, cmd) (":localhost 411 " + client + " :No recipient given(" + cmd + ")\r\n")
# define ERR_NOTEXTTOSEND(client) (":localhost 412 " + client + " :No text to send\r\n")
# define ERR_UNKNOWNCOMMAND(client, cmd) (":localhost 421 " + client + ' '+ cmd + " :Unknown command\r\n")
# define ERR_NONICKNAMEGIVEN(client) (":localhost 431 " + client + " :No nickname given\r\n")
# define ERR_ERRONEUSNICKNAME(client, nck) (":localhost 432 " + client + ' ' + nck + " :Erroneus nickname\r\n")
# define ERR_NICKNAMEINUSE(client, nck) (":localhost 433 " + client + ' ' + nck + " :Nickname is already in use\r\n")
# define ERR_NICKCOLLISION(client, nck) (":localhost 436 " + client + ' ' + nck + " :Nickname collision KILL\r\n")
# define ERR_USERNOTINCHANNEL(client, chnl, nck) (":localhost 441 " + client + ' ' + nck + ' ' + chnl + " :They aren't on that channel\r\n")
# define ERR_NOTONCHANNEL(client, chnl) (":localhost 442 " + client + ' ' + chnl + " :You're not on that channel\r\n")
# define ERR_USERONCHANNEL(client, nck, chnl) (":localhost 443 " + client + ' ' + nck + ' ' + chnl + " :is already on channel\r\n")
# define ERR_SUMMONDISABLED(client) (":localhost 445 " + client + " :SUMMON has been disabled\r\n")
# define ERR_USERSDISABLED(client) (":localhost 446 " + client + ":USERS has been disabled\r\n")
# define ERR_NOTREGISTERED(client) (":localhost 451 " + client + " :You have not registered\r\n")
# define ERR_NEEDMOREPARAMS(client, cmd) (":localhost 461 " + client + ' ' + cmd + " :Not enough parameters\r\n")
# define ERR_ALREADYREGISTRED(client) (":localhost 462 " + client + " :You may not reregister\r\n")
# define ERR_PASSWDMISMATCH(client) (":localhost 464 " +  client + " :Password incorrect\r\n")
# define ERR_KEYSET(client, chnl) (":localhost 467 " +  client + ' ' + chnl + " :Channel key already set\r\n")
# define ERR_CHANNELISFULLL(client, chnl) (":localhost 471 " + client + ' ' + chnl + " :Cannot join channel (+l)\r\n")
# define ERR_UNKNOWNMODE(client, chr) (":localhost 472 " + client + ' ' + chr +" :is unknown mode char to me\r\n")
# define ERR_INVITEONLYCHAN(client, chnl) (":localhost 473 " + client + ' ' + chnl + " :Cannot join channel (+i)\r\n")
# define ERR_BADCHANNELKEY(client, chnl) (":localhost 475 " + client + ' ' + chnl + " :Cannot join channel (+k)\r\n")
# define ERR_CHANOPRIVSNEEDED(client, chnl) (":localhost 482 " + client + ' ' + chnl + " :You're not channel operator\r\n")
class Server;

class Task
{
    private:
        std::string cmd;
        std::vector<std::string> args;


        void parse(std::string fullCmd);
        bool is_name_valid(std::string name);
        void ping(Client &c);
		void pass(Client &c, Server &s);
		void nick(Client &c, Server &s);
		void part(Client &c, Server &s);
		void kick(Client &c, Server &s);
		void mode(Client &c, Server &s);
		void invite(Client &c, Server &s);
		void user(Client &c);
        void quit(Client &c, Server &s);
		void join(Client &c, Server &s);
		void topic(Client &c, Server &s);
        void privmsg(Client &c, Server &s);
    public:
        Task();
        Task(std::string fullCmd);
        Task(const Task &other);
        Task &operator=(const Task &other);
        ~Task();

        bool run(Client &c, Server &s);
        static bool run(std::string fullCmd, Client &c, Server &s);
};

#endif
