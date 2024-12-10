#include "ft_irc.hpp"

#include <cstdio> // std::perror (no lo meto en el header porque no se si puede usarse o no el perror)

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
int cycle(std::vector<struct pollfd>& monitored, const char *const password)
{
	int pollret;
	int monit_size;
	(void)password;
	std::string rd_buff[250]; // we don't have a user class / struct yet so the message buffer is here
	std::string wr_buff[250]; // we don't have a user class / struct yet so the message buffer is here
	while (1)
	{
		monit_size = monitored.size();
		pollret = poll(monitored.data(), monitored.size() /*length of monitored (array of fds), var is of type nfds_t*/, 0 /*Timeout in ms, set to 0 so it's non-blocking ("If timeout is zero, then poll() will return without blocking.", Source: https://man.freebsd.org/cgi/man.cgi?poll)*/);
		if (pollret > 0)
		{
			for	(int i = 0; i < monit_size && pollret; i++) // we're potentially adding / removing clients from the vector, careful about the index and where it ends
			{
				struct pollfd current = monitored[i];
				if (DEBUG)
					std::cout << "Pollret: " << pollret << " |fd: " << current.fd << " |revents " << current.revents <<std::endl;//DEBUG
				if (!(current.revents & current.events)) { continue; }
				if (DEBUG)
				{
					std::cout << "--------------------------------------------------------------------" << std::endl;
					std::cout << "Potential activity on monitored[" << i << "], fd " << current.fd << std::endl;
				}
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
				// if (current.events)
				// {//events are the ones we will be checking with, so we are the ones setting the values. Do we need to DEBUG print them?
				// 	std::cout << "[EVENTS] (Raw Value: " << current.events << ")" << std::endl;
				// 	if (current.events & POLLIN)
				// 		std::cout << "- POLLIN" << std::endl;
				// 	if (current.events & POLLOUT)
				// 		std::cout << "- POLLOUT" << std::endl;
				// 	if (current.events & POLLNVAL)
				// 		std::cout << "- POLLNVAL" << std::endl;
				// }
				// else
				// 	std::cout << "No Events"<< std::endl;
				// std::cout << std::endl;
				if (!current.revents)
				{
					if (DEBUG)
						std::cout << "No Revents"<< std::endl;//DEBUG
					continue;
				}
				if (DEBUG)
					std::cout << "[REVENTS] (Raw Value: " << current.revents << ")" << std::endl;
				if (i == 0 && (current.revents & current.events)) //listener will always be [0]
				{
					// https://reactive.so/post/42-a-comprehensive-guide-to-ft_irc/
					struct sockaddr_in client_addr;
				    socklen_t client_addr_len = sizeof(client_addr);
					int client_fd = -1;
					struct pollfd newcomer;
				    while (client_fd < 0)//this should not be needed
						client_fd = accept(monitored[0].fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len); // returns -1 on failure, usually EAGAIN due to non-block//if properly programmed, we never get to see EAGAIN
					newcomer.fd = client_fd;
					newcomer.events = POLLIN | POLLOUT | POLLHUP /*we might want some global precompiler thing for these, in case its not just POLLIN walways*/;
					newcomer.revents = 0;
					monitored.push_back(newcomer);
					//do we want to do anything else with the newcomer? like putting there recent messages or something
					std::cout << "WORLD WIDE NOISE 🗣 🗣 🗣" << std::endl;//DEBUG
				}
				else if (current.revents & current.events)
				{
					if (current.revents & POLLIN) // might be redundant later on
					{
						if (DEBUG)
							std::cout << "- POLLIN" << std::endl;
						char buffer[1];
						ssize_t bytes_read = recv(current.fd, buffer, sizeof(buffer), MSG_DONTWAIT);
						while (bytes_read > 0 && buffer[bytes_read - 1] != '\n')
						{
							for (size_t j = 0; j < sizeof(buffer); j++)
								rd_buff[i] += buffer[j];
							// std::cout  << "msg: " << msg << std::endl;
							bytes_read = recv(current.fd, buffer, sizeof(buffer), MSG_DONTWAIT); // this can block? (nc -C + Ctrl-D) // is client socket non-blocking?
						}
						if (!bytes_read)//consider checking for POLLHUP instead // https://stackoverflow.com/questions/74627334/no-pollhup-event-when-poll-on-tcp-socket-and-remote-closed
						{
							std::cout << "Client Disconnected" << std::endl;//DEBUG?
							close(current.fd);
							monitored.erase(monitored.begin() + i); // should we i-- after this? might be skipping over a client
						}
						else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK) // can we use errno?
						{
							perror("read"); // perror
							close(current.fd);
							monitored.erase(monitored.begin() + i); // should we i-- after this? might be skipping over a client
							// close(server_fd);
							return 1;//don't return, try to handle the error here and "continue;" only return if we are truly fucked
						}
						else if (bytes_read > 0)
						{
							std::cout << "Received message: " << rd_buff[i] << std::endl;
							for (int j = 0; j < 250; j++)
								if (j != i)
									wr_buff[j] = "Message From Client: " + rd_buff[i] + '\n';
							rd_buff[i] = "";
						}
					}
					// Write (cutrisimo y asqueroso)
					if (current.events & POLLOUT && wr_buff[i].length() > 0)
					{
						char buffer[1];
						buffer[0] = wr_buff[i].c_str()[0];
						ssize_t out = send(current.fd, buffer, sizeof(buffer), MSG_DONTWAIT);
						while (out > 0 && wr_buff[i].length() > 0)
						{
							if (wr_buff[i].length() > 1)
							{
								wr_buff[i] = wr_buff[i].substr(1);
								buffer[0] = wr_buff[i].c_str()[0];
								send(current.fd, buffer, sizeof(buffer), MSG_DONTWAIT);
							}
							else { wr_buff[i] = ""; }
						}
						if (out < 0 && errno != EAGAIN && errno != EWOULDBLOCK) // can we use errno?
						{
							perror("send"); // perror
							close(current.fd);
							monitored.erase(monitored.begin() + i); // should we i-- after this? might be skipping over a client
							// close(server_fd);
							return 1;//don't return, try to handle the error here and "continue;" only return if we are truly fucked
						}
					}
				}
				if (DEBUG)
				{
					if (current.revents & POLLOUT)
						std::cout << "- POLLOUT" << std::endl;
					if (current.revents & POLLNVAL)
						std::cout << "- POLLNVAL" << std::endl;
					if (current.revents & POLLHUP)
						std::cout << "- POLLHUP" << std::endl;
				}
				current.revents = 0; // do we need to do this?
				pollret--;
				if (DEBUG)
					std::cout << "--------------------------------------------------------------------" << std::endl;
			}
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
	std::vector<struct pollfd> monitored;
	struct pollfd name_this;

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
	name_this.fd = listener;
	name_this.events = POLLIN | POLLHUP;
	name_this.revents = 0;//0 since this will get filled when poll() gets called
	monitored.push_back(name_this);
	/*
	monitored = static_cast<struct pollfd*>(malloc(2 * sizeof(struct pollfd))); // erm... what the malloc? (-42 social credit) (can / should we use a vector?) // not even a calloc? damn dude ok // why do we have 2 sockets??
	if (!monitored)
	{
		close(listener);
		return (ENOMEM);
	}
	*/
	std::cout << "Socket ready! Listening on port " << std::atoi(argv[1]) << "..." << std::endl;

	cycle(monitored, argv[2]);
	return (0);
}
