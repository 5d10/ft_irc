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
	listener = socket(AF_INET, SOCK_STREAM, 0);//protocol->p_proto);
	if (-1 == listener)//(IPv4, necesario para TCP, TCP en si)
		return (-1);

	bzero(&test, sizeof(struct sockaddr_in));
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
		pollret = poll(monitored, 0/*array of file descriptors, currently NULL*/, POLL_TIMEOUT); // WARNING: Poll may be a blocking function (Source: https://man7.org/linux/man-pages/man2/poll.2.html, "The timeout argument specifies the number of milliseconds that poll() should block waiting for a file descriptor to become ready.", keyword BLOCK)
		/*
			identify which fds we want to do something with
			DO IT
		*/
		(void)pollret; // cound't compile without this
	}
	return (0);//in case we want to return errors
}

int main (int argc, char** argv)
{
	int listener;
	struct pollfd* monitored;

	if (argc !=3)
	{
		std::cout << "Incorrect amount of arguments. Expected 2, have " << argc-1 << std::endl;
		return (1);
	}


	listener = initialize_listener(std::atoi(argv[1]));
	if (-1 == listener)
	{
		//possibly use my pterror from minishell _glopez-m
		return (1);//good that no malloc was done yet
	}
	monitored = static_cast<struct pollfd*>(malloc(2 * sizeof(struct pollfd)));
	if (!monitored)
	{
		close(listener);
		return (ENOMEM);
	}
	cycle(monitored, argv[2]);
	return (0);
}
