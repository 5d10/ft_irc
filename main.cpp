#include "ft_irc.hpp"
#include "Server.hpp"

bool signal_shutdown = false;

void signalTriggered(int signal)
{
	std::cout << std::endl << "Signal: " << 128 + signal << std::endl;
	signal_shutdown = true;
}

int main (int argc, char** argv)
{
	if (argc != 3)
	{
		std::cout << "Incorrect amount of arguments (expected 2, given " << argc - 1 << ")" << std::endl;
		std::cout << "Usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}

	int port = std::atoi(argv[1]);
	Server server(argv[2]);
	if (-1 == server.init(port)) return (1);
	std::cout << "Server ready! Listening on port " << port << "..." << std::endl;

	struct sigaction sigact;
	sigact.sa_handler = &signalTriggered;
  	sigemptyset(&sigact.sa_mask);
  	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);

	server.cycle();
	return (0);
}
