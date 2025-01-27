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
        if (std::isspace(fullCmd[0]))
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
    std::cout << "COMMAND: " << cmd << std::endl;
    std::cout << "ARGS: " << std::endl;
    for (unsigned int i = 0; i < args.size(); i++)
        std::cout << "- " << args[i] << std::endl;
}
