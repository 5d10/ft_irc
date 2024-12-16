#include "ft_irc.hpp"


// PORT and PASSWORD ar just placeholders to be replaced with actual variables

int main (int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Incorrect amount of arguments. Expected 2, have " << argc - 1 << std::endl << "Usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}
	int port = std::atoi(argv[1]);
	Server server(argv[2]);
	if (server.init(port)) { return 1; }
	/*
	monitored = static_cast<struct pollfd*>(malloc(2 * sizeof(struct pollfd))); // erm... what the malloc? (-42 social credit) (can / should we use a vector?) // not even a calloc? damn dude ok // why do we have 2 sockets??
	if (!monitored)
	{
		close(listener);
		return (ENOMEM);
	}
	*/
	std::cout << "Server ready! Listening on port " << port << "..." << std::endl;

	server.cycle();
	return (0);
}
