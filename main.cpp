#include "ft_irc.hpp"


// PORT and PASSWORD ar just placeholders to be replaced with actual variables

int main (int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Incorrect amount of arguments. Expected 2, have " << argc-1 << '\n'
			<< "Usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}

	int port = std::atoi(argv[1]);
	Server server(argv[2]);
	if (-1 == server.init(port)) return (1);
	std::cout << "Server ready! Listening on port " << port << "..." << std::endl;

	server.cycle();
	return (0);
}
