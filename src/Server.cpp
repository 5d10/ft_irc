#include "Server.hpp"

int Server::init(int port)
{
	int listener;
	listener = initialize_listener(port);
	if (-1 == listener)
		return (-1);
	AddClient(listener, POLLIN);
	return (0);
}

int Server::initialize_listener(const int port)
{
	int listener;
	struct protoent* protocol = getprotobyname("TCP");
	struct sockaddr_in test;

	if (!protocol) {
		std::cerr << "Bad ini of protocol" << std::endl;
		return (-1); }
	listener = socket(AF_INET /* IPv4 */,
				SOCK_STREAM | SOCK_NONBLOCK /* TCP and Non-Blocking */,
				protocol->p_proto /* TCP Protocolgit stat */);
	if (-1 == listener)
	{
		std::cerr << "Failed to create server socket" << std::endl;
		return (-1);
	}

	std::memset(&test, 0, sizeof(struct sockaddr_in));
	test.sin_family = AF_INET;
	test.sin_addr.s_addr = htonl(2130706433);// 127.0.0.1
	test.sin_port = htons(port);

	int flag = 1;
	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)))
	{
		std::cerr << "Failed to configure server socket" << std::endl;
		return (-1);
	}


	if (-1 == bind(listener, reinterpret_cast<const struct sockaddr*>(&test), sizeof(test)))
	{
		std::cerr << "Failed to bind" << std::endl;
		return (-1);
	}

	if (-1 == listen(listener, MAX_CONN_QUEUE))
		return (-1);
	return (listener);	
}

void Server::AddClient(int fd, short flags)
{
	struct pollfd poll_data;
	poll_data.fd = fd;
	poll_data.events = flags;
	poll_data.revents = 0;
	pollfds.push_back(poll_data);
	clients.push_back(Client(fd));
}

void Server::EraseClient(Client &client, std::string quit_message)
{
	//remove from Channels
	{
		std::map<std::string, Channel>::iterator i = channels.begin();
		std::map<std::string, Channel>::iterator end = channels.end();
		std::map<std::string, Channel>::iterator temp;
		while (i != end)
		{
			i->second.removeUser(client.nickname);
			i->second.broadcast(quit_message);
			if (i->second.isOperator.empty())
			{
				temp = i;
				++i;
				channels.erase(temp);
			}
			else
				++i;
		}
	}
	//remove from Server
	unsigned int i = 0;
	std::list<Client>::iterator it = clients.begin();
	std::list<Client>::iterator end = clients.end();

	while (it != end && it->fd != client.fd)
	{//search the client first
		i++;
		it++;
	}
	if (it == end)
	{
		#if DEBUG
			std::cout << "YOU HAVE REACHED END OF CLIENTS LIST ON ERASE CLIENT\n"
			<< "WHAT IN THE WOOOOOOOOOOOOOOOOOOOOOOOOOOOOOORLD" << std::endl;
		#endif
		return;
	}
	//then delete it
	#if DEBUG
		std::cout << "Erase: closing fd " << pollfds[i].fd << std::endl;
	#endif
	close(it->fd);
	pollfds.erase(pollfds.begin() + i);
	registered.erase(client.nickname);
	clients.erase(it);

	#if DEBUG
		std::cout << "SERVER: finished removing a client" << std::endl;
	#endif
}

void Server::AcceptClient()
{
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = -1;
	client_fd = accept(pollfds[0].fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);
	AddClient(client_fd, POLLIN);
	#if DEBUG
		std::cout << "WORLD WIDE NOISE 🗣 🗣 🗣" << std::endl;
	#endif
}

int Server::OnClientRead(size_t index)
{
	Client &client = getClientAtIndex(index);
	ssize_t bytes_read = client.Read();
	if (!bytes_read)
	{
		#if DEBUG
			std::cout << "Client Disconnected" << std::endl;
		#endif
		EraseClient(client, "");
	}
	else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
	{
		std::cerr << "error when reading from [" << index << "]" << std::endl;
		perror("read" );
		EraseClient(client, "");
		return (1);
	}
	else if (bytes_read > 0)
	{
		bool deleted = Task::run(client.GetNextCmd(), client, *this);
		if (deleted)
			return(2);
	}
	return (0);
}

int Server::OnClientSend(size_t index)
{
	Client &client = getClientAtIndex(index);
	#if DEBUG
		std::cout << "Sending to " << client.nickname << std::endl;
	#endif
	ssize_t out = client.Send();
	if (out < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
	{
		std::cerr << "error when sending from [" << index << "]" << std::endl;
		perror("send");
		EraseClient(client, "");
		return (1);
	}
	pollfds[index].events &= (~POLLOUT); 
	return (0);
}

extern bool signal_shutdown;

int Server::cycle()
{
	std::cout << "Cycling..." << std::endl;

	int pollret;
	size_t monit_size;
	(void)password;
	while (!signal_shutdown)
	{
		monit_size = pollfds.size();
		pollret = poll(pollfds.data(), monit_size, 0); // Timeout in ms, set to 0 so it's non-blocking
		if (pollret < 0 && errno != EINTR)
		{
			std::cerr << "Poll Errno: " << errno << std::endl;
			return (-1);
		}
		#if DEBUG
			if (0 < pollret)
				std::cout << "--------------------------------------------------------------------" << std::endl;
		#endif
		for	(size_t i = 0; i < monit_size && pollret > 0; i++)
		{
			struct pollfd& current = pollfds[i];
			Client &client = getClientAtIndex(i);

			#if DEBUG
				std::cout << "Pollret: " << pollret << " |fd: " << current.fd << " |revents " << current.revents <<std::endl;
			#endif

			if (!(current.revents & current.events)) goto check_pollout;

			#if DEBUG
				std::cout << "Potential activity on monitored[" << i << "], fd " << current.fd << std::endl;
				if (current.revents & POLLNVAL)
					std::cout << "- POLLNVAL" << std::endl;
				if (current.revents & POLLHUP)
					std::cout << "- POLLHUP" << std::endl;
				std::cout << "[REVENTS] (Raw Value: " << current.revents << ")" << std::endl;
			#endif
			if (i == 0 && (current.revents & current.events)) //listener will always be [0]
				AcceptClient();
			else if (current.revents & current.events)
			{
				if (current.revents & POLLIN)
				{
					#if DEBUG
						std::cout << "- POLLIN" << std::endl;
					#endif
					int out = OnClientRead(i);
					if (out == 1)
						return out;
					else if (out == 2) /*bro got deleted*/ {
						--pollret;
						#if DEBUG
							std::cout << ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,," << std::endl;
						#endif
						continue;
					}
				}
				#if DEBUG
					if (current.revents & POLLOUT)
						std::cout << "- POLLOUT" << '\n';
					std::cout << "Size in buffer = " << client.GetWriteBuffer().length() << std::endl;
				#endif
				if (current.revents & POLLOUT && client.GetWriteBuffer().length() > 0)
				{
					int out = OnClientSend(i);
					if (out) {
						--pollret;
						--monit_size;
						#if DEBUG
							std::cout << ",,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,," << std::endl;
						#endif
						continue;
					}
				}
			}
			--pollret;
			check_pollout:
			pollfds[i].events = client.GetWriteBuffer().empty() ? pollfds[i].events & ~POLLOUT : pollfds[i].events | POLLOUT;
			#if DEBUG
				std::cout << "...................................................................." << std::endl;
			#endif
		}
	//	SetClientPolloutFlags();
	}
	std::cout << "Shutting down server..." << std::endl;
	return (0);//in case we want to return errors
}

Server::~Server()
{
	std::cout << pollfds.size() << std::endl;
	for (ssize_t i = pollfds.size() - 1; i >= 0; i--)
	{
		std::cout << "Disconnecting FD " << pollfds[i].fd << std::endl;
		EraseClient(getClientAtIndex(i), "Server has been shut down");
	}
}

void Server::SetClientPolloutFlags()
{//no longer used
	size_t i;
	std::list<Client>::iterator it;
	for (i = 0, it = clients.begin(); i < pollfds.size() && it != clients.end(); i++, it++)
		if (!it->GetWriteBuffer().empty())
			pollfds[i].events |= POLLOUT;
		else
			pollfds[i].events &= ~POLLOUT;
}

Client &Server::getClientAtIndex(size_t index)
{
	std::list<Client>::iterator it = clients.begin();
	while (index--)
		it++;
	return *it;
}

void Server::Rename(Client& client, std::string new_name)
{
	std::string old_name = client.nickname;

	client.nickname = new_name;
	{
		std::map<std::string, Channel>::iterator i = channels.begin();
		std::map<std::string, Channel>::iterator end = channels.end();
		while (i != end)
		{
			if (client.nicked)
				i->second.broadcast(':'+old_name+'!'+ client.username+"@localhost NICK :" + new_name + "\r\n");
			i->second.Rename(old_name, new_name);
			++i;
		}
	}
	registered.erase(old_name);
	registered.insert(std::pair<std::string, Client *>(new_name, &client));
}
