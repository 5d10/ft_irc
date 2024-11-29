#include "ft_irc.hpp"

// PORT and PASSWORD ar just placeholders to be replaced with actual variables

int initialize_listener(const int port)
{
	int listener;
	struct protoent* protocol = getprotobyname("TCP");
	struct sockaddr_in test;//where in the std is this defined? don't ask me

	if (!protocol)
	{
		std::cerr << "Bad ini of protocol" << std::endl;//cerr used
		return (-1);
	}
	listener = socket(AF_INET /* IPv4 */, SOCK_STREAM | SOCK_NONBLOCK /* TCP and Non-Blocking */, protocol->p_proto /* TCP Protocolgit stat */);
	if (-1 == listener)
		return (-1);

	bzero(&test, sizeof(struct sockaddr_in)); // are- are we allowed to use bzero and other funcs??? is there no cpp std98 equivalent?
	test.sin_family = AF_INET;
	test.sin_addr.s_addr = htonl(2130706433);// presumably 127.0.0.1 whic iirc is localhost
	test.sin_port = htons(port);//why htonl and htons? // https://linux.die.net/man/3/htonl "htonl, htons, ntohl, ntohs - convert values between host and network byte order" tl;dr: network shit
	//strncpy(test.sin_path, path, sizeof(test.sin_path) -1);//please check for overflow
	if (-1 == bind(listener, reinterpret_cast<const struct sockaddr*>(&test), sizeof(test)))
	{
		std::cerr << "Failed to bind" << std::endl;//cerr used
		return (-1);
	}

	//mas posible configuracion;

	if (-1 == listen(listener, MAX_CONN_QUEUE))
		return (-1);
	return (listener);	
}
int cycle(struct pollfd* monitored, const char *const password)
{
	int pollret;
	(void)password;
	while (1)
	{
		pollret = poll(monitored, 2 /*length of monitored (array of fds), var is of type nfds_t*/, 0 /*Timeout in ms, set to 0 so it's non-blocking ("If timeout is zero, then poll() will return without blocking.", Source: https://man.freebsd.org/cgi/man.cgi?poll)*/);
		if (pollret > 0)
		{
			for	(int i = 0; i < 2; i++)
			{
				struct pollfd current = monitored[i];
				if (current.events || current.revents)
				{
					std::cout << "--------------------------------------------------------------------" << std::endl;
					// std::cout << "POLLIN: " << POLLIN << std::endl; // 1
					// std::cout << "POLLRDNORM: " << POLLRDNORM << std::endl; // 64
					// std::cout << "POLLRDBAND: " << POLLRDBAND << std::endl; // 128
					// std::cout << "POLLPRI: " << POLLPRI << std::endl; // 2
					// std::cout << "POLLOUT: " << POLLOUT << std::endl; // 4
					// std::cout << "POLLWRNORM: " << POLLWRNORM << std::endl; // 256
					// std::cout << "POLLWRBAND: " << POLLWRBAND << std::endl; // 512
					// std::cout << "POLLERR: " << POLLERR << std::endl; // 8
					// std::cout << "POLLHUP: " << POLLHUP << std::endl; // 16
					// std::cout << "POLLRDHUP: " << POLLRDHUP << std::endl; // 8192
					// std::cout << "POLLNVAL: " << POLLNVAL << std::endl; // 32
					std::cout << "Potential activity on monitored[" << i <<"], fd " << current.fd << std::endl;
					if (current.events)
					{
						std::cout << "[EVENTS] (Raw Value: " << current.events << ")" << std::endl;
						if (current.events & POLLIN)
							std::cout << "- POLLIN" << std::endl;
						if (current.events & POLLOUT)
							std::cout << "- POLLOUT" << std::endl;
						if (current.events & POLLNVAL)
							std::cout << "- POLLNVAL" << std::endl;
					}
					else
						std::cout << "No Events"<< std::endl;
					std::cout << std::endl;
					if (current.revents)
					{
						std::cout << "[REVENTS] (Raw Value: " << current.revents << ")" << std::endl;
						if (current.revents & POLLIN)
							std::cout << "- POLLIN" << std::endl;
						if (current.revents & POLLOUT)
							std::cout << "- POLLOUT" << std::endl;
						if (current.revents & POLLNVAL)
							std::cout << "- POLLNVAL" << std::endl;
					}
					else
						std::cout << "No Revents"<< std::endl;
					std::cout << "--------------------------------------------------------------------" << std::endl;
				}
			}
			/*
				identify which fds we want to do something with
				DO IT
			*/
		}
		else if (pollret)
		{
			std::cerr << "Errno: " << errno << std::endl; // cerr used
			return (-1);
		}
	}
	return (0);//in case we want to return errors
}

int main (int argc, char** argv)
{
	int listener;
	struct pollfd* monitored;

	if (argc != 3)
	{
		std::cout << "Incorrect amount of arguments. Expected 2, have " << argc - 1 << std::endl << "Usage: ./ircserv <port> <password>" << std::endl;
		return (1);
	}
	listener = initialize_listener(std::atoi(argv[1])); // potencial error de parsing con los out-of-range y/o cosas que no sean numeros mi rey
	if (-1 == listener)
	{
		//possibly use my pterror from minishell _glopez-m
		return (1);//good that no malloc was done yet
	}
	monitored = static_cast<struct pollfd*>(malloc(2 * sizeof(struct pollfd))); // erm... what the malloc? (-42 social credit) (can / should we use a vector?) // not even a calloc? damn dude ok // why do we have 2 sockets??
	if (!monitored)
	{
		close(listener);
		return (ENOMEM);
	}
	std::cout << "Sockets ready!" << std::endl;
	cycle(monitored, argv[2]);
	return (0);
}
