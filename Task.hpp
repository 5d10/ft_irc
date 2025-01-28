#include "ft_irc.hpp"

class Task
{
    private:
        // enum e_types { UNKNOWN, PASS, NICK, USER, PING, JOIN, PRIVMSG, PART, QUIT, KICK, INVITE, TOPIC, MODE }; // might not even be necessary: if/else? map str to function? (one func per command type? no class, cut the middle man)
        std::string cmd;
        std::vector<std::string> args;

        void parse(std::string fullCmd);
        e_types getType();
    public:
        Task();
        Task(std::string fullCmd);
        Task(const Task &other);
        Task &operator=(const Task &other);
        ~Task();
};
