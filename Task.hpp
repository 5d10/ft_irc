#include "ft_irc.hpp"

class Task
{
    private:
        std::string cmd;
        std::vector<std::string> args;

        void parse(std::string fullCmd);
    public:
        Task();
        Task(std::string fullCmd);
        Task(const Task &other);
        Task &operator=(const Task &other);
        ~Task();
};
