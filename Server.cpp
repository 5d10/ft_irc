#include "Server.hpp"
#include <cstdio> // std::perror (no lo meto en el header porque no se si puede usarse o no el perror)

int Server::init(int port)
{
	int listener;
	listener = initialize_listener(port); // potencial error de parsing con los out-of-range y/o cosas que no sean numeros mi rey
	if (-1 == listener)
		//possibly use my pterror from minishell _glopez-m
		return (-1);
	AddClient(listener, POLLIN | POLLHUP); //I think POLLHUP is doing nothing here
	return (0);
}

int Server::initialize_listener(const int port)
{
	int listener;
	struct protoent* protocol = getprotobyname("TCP");
	struct sockaddr_in test;//where in the std is this defined? don't ask me

	if (!protocol) {
		std::cerr << "Bad ini of protocol" << std::endl;//cerr used
		return (-1); }
	listener = socket(AF_INET /* IPv4 */,
				SOCK_STREAM | SOCK_NONBLOCK /* TCP and Non-Blocking */,
				protocol->p_proto /* TCP Protocolgit stat */);
	if (-1 == listener)
		return (-1);

	std::memset(&test, 0, sizeof(struct sockaddr_in));
	test.sin_family = AF_INET;
	test.sin_addr.s_addr = htonl(2130706433);// presumably 127.0.0.1 whic iirc is localhost
	test.sin_port = htons(port);//why htonl and htons? // https://linux.die.net/man/3/htonl "htonl, htons, ntohl, ntohs - convert values between host and network byte order" tl;dr: network shit
	//strncpy(test.sin_path, path, sizeof(test.sin_path) -1);//please check for overflow

	// Copied From Another ft_irc, Fixes Some "Failed To Bind" Issues
	int flag = 1;
	if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)))
	{
		std::cerr << "Failed to configure server socket" << std::endl; // cerr used
		return (-1);
	}


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

void Server::AddClient(int fd, short flags)
{
	struct pollfd poll_data;
	poll_data.fd = fd;
	poll_data.events = flags;
	poll_data.revents = 0;//0 since this will get filled when poll() gets called
	pollfds.push_back(poll_data);
	clients.push_back(Client(fd));
}

void Server::DisconnectClient(size_t index)
{
	close(pollfds[index].fd);
	pollfds.erase(pollfds.begin() + index);
	std::list<Client>::iterator it = clients.begin();
	while (index--)
		it++;
	clients.erase(it);
}

void Server::AcceptClient()
{
	// https://reactive.so/post/42-a-comprehensive-guide-to-ft_irc/
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int client_fd = -1;
	while (client_fd < 0)//this should not be needed
		client_fd = accept(pollfds[0].fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len); // returns -1 on failure, usually EAGAIN due to non-block//if properly programmed, we never get to see EAGAIN
	AddClient(client_fd, POLLIN | POLLHUP); // we might want some global precompiler thing for these, in case its not just POLLIN walways
	//do we want to do anything else with the newcomer? like putting there recent messages or something
	std::cout << "WORLD WIDE NOISE 🗣 🗣 🗣" << std::endl;//DEBUG
}

bool debug_is_printable_str(std::string str)
{
	for (unsigned int i = 0; i < str.size(); i++)
		if (!isprint(str[i]) && !isspace(str[i]))
			return false;
	return true;
}

void debug_print_hex(std::string str)
{
	std::string hexChars = "0123456789ABCDEF";
	for (unsigned int i = 0; i < str.size(); i++)
	{
		unsigned char c = str[i];
		std::cout << hexChars[c / 16];
		std::cout << hexChars[c % 16];
		if (i < str.size() - 1)
			std::cout << " ";
	}
}

// move this elsewhere please???
bool IsValidChannelName(const std::string &name)
{
	if (name.size() > 200)
		return false;
	if (name[0] != '#' || name[0] != '&')
		return false;
	for (unsigned int i = 1; i < name.size(); i++)
		if (name[i] == ' ' || name[i] == ',' || name[i] == 7)
			return false;
	return true;
}

int Server::OnClientRead(size_t index)
{
	Client &client = getClientAtIndex(index);
	ssize_t bytes_read = client.Read();
	if (!bytes_read)//consider checking for POLLHUP instead // https://stackoverflow.com/questions/74627334/no-pollhup-event-when-poll-on-tcp-socket-and-remote-closed
	{
		std::cout << "Client Disconnected" << std::endl;//DEBUG?
		DisconnectClient(index); // should we i-- after this? might be skipping over a client
	}
	else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK) // can we use errno?
	{
		std::cerr << "error when reading from [" << index << "]" << std::endl; // std::cerr
		perror("read" ); // perror
		DisconnectClient(index); // should we i-- after this? might be skipping over a client
		// close(server_fd);
		return (1);
	}
	else if (bytes_read > 0)
	{
		if (debug_is_printable_str(client.GetReadBuffer()))
			std::cout << "Received message: " << client.GetReadBuffer() << std::endl;
		else
		{
			std::cout << "Received bytes: ";
			debug_print_hex(client.GetReadBuffer());
			std::cout << std::endl;
		}
		for (size_t i = 0; i < clients.size(); i++)
		{
			if (i != index)
			{
				getClientAtIndex(i).AddToWriteBuffer("Message From Client: " + client.GetReadBuffer() + '\n');
				pollfds[i].events |= POLLOUT;
			}
			else if (client.GetReadBuffer() == "JOIN #chan1\r")
			{
				if (true)
				{
					getClientAtIndex(i).AddToWriteBuffer(":nick1!user@localhost JOIN :#chan1\r\n");
					getClientAtIndex(i).AddToWriteBuffer(":localhost 332 <client> #chan1 <topic>\r\n");
					getClientAtIndex(i).AddToWriteBuffer(":localhost 353 user = #chan1 :@nick1\r\n");
					getClientAtIndex(i).AddToWriteBuffer(":localhost 366 user #chan1 :End of /NAMES list.\r\n");
					std::cout << "TEST: " << Channel("#chan1", "nick1").getUserList() << std::endl; // seems to be fine, nicks are appraently alphabetically ordered
				}
				else
				{
					getClientAtIndex(i).AddToWriteBuffer(":localhost 474 <client> #chan1 :Cannot join channel (+b)\r\n");
					// getClientAtIndex(i).AddToWriteBuffer(":localhost 475 <client> #chan1 :Cannot join channel (+k)\r\n");
					// getClientAtIndex(i).AddToWriteBuffer(":localhost 474 <client> #chan1\r\n");
					// getClientAtIndex(i).AddToWriteBuffer(":localhost 475 #chan1 :gkasgashjg\r\n");
				}
				pollfds[i].events |= POLLOUT;
			}
			else if (client.GetReadBuffer() == "USER user 0 * :realname\r") // https://datatracker.ietf.org/doc/html/rfc1459#section-8.5 read this fucker
			{
				// https://datatracker.ietf.org/doc/html/rfc1459#section-4.1.3 and this
				// username = user
				// hostname = 0
				// servername = *
				// realname = :realname

				/*
				[Client] Message received from client 4 << CAP LS 302

				[Client] Message received from client 4 << PASS server_pass
				NICK nick1
				USER user 0 * :realname
				[Server] Message sent to client 4       >> :localhost 464  :Password incorrect.
				[Server] Message sent to client 4       >> :nick1!@localhost NICK nick1
				*/


				// Copied Over From Another ft_irc
				getClientAtIndex(i).AddToWriteBuffer(":nick1!@localhost NICK nick1\r\n");
				// getClientAtIndex(i).AddToWriteBuffer("localhost 001 nick1 :Welcome to the Internet Relay Network :nick1!user@localhost\r\n");
				// getClientAtIndex(i).AddToWriteBuffer(":localhost 002 nick1 :Your host is 42_Ftirc (localhost), running version 1.1\r\n");
				// getClientAtIndex(i).AddToWriteBuffer(":localhost 003 nick1 :This server was created 15-01-2025 11:44:24\r\n");
				// getClientAtIndex(i).AddToWriteBuffer(":localhost 004 nick1 localhost 1.1 io kost k\r\n");
				// getClientAtIndex(i).AddToWriteBuffer(":localhost 005 nick1 CHANNELLEN=32 NICKLEN=9 TOPICLEN=307 :are supported by this server\r\n");
				
				// Trigger For "Connection Complete!" Pop-Up On HexChat
				getClientAtIndex(i).AddToWriteBuffer(":localhost 376 nick1 :End of /MOTD command.\r\n");
				
				pollfds[i].events |= POLLOUT;
			}
		}
		client.ClearReadBuffer();
	}
	return (0);
}

int Server::OnClientSend(size_t index)
{
	Client &client = getClientAtIndex(index);
	ssize_t out = client.Send();
	if (out < 0 && errno != EAGAIN && errno != EWOULDBLOCK) // can we use errno?
	{
		std::cerr << "error when sending from [" << index << "]" << std::endl; // std::cerr
		perror("send"); // perror
		DisconnectClient(index); // should we i-- after this? might be skipping over a client
		// close(server_fd);
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
		pollret = poll(pollfds.data(), monit_size, 0 /*Timeout in ms, set to 0 so it's non-blocking ("If timeout is zero, then poll() will return without blocking.", Source: https://man.freebsd.org/cgi/man.cgi?poll)*/);
		if (pollret < 0 && errno != EINTR)
		{
			std::cerr << "Poll Errno: " << errno << std::endl; // cerr used
			return (-1);
		}
		for	(size_t i = 0; i < monit_size && pollret > 0; i++) // we're potentially adding / removing clients from the vector, careful about the index and where it ends
		{
			struct pollfd current = pollfds[i];
			Client &client = getClientAtIndex(i);

			#if DEBUG
				std::cout << "Pollret: " << pollret << " |fd: " << current.fd << " |revents " << current.revents <<std::endl;
			#endif

			if (!(current.revents & current.events)) continue;
			//I'd say this is unnecessary given we will go through the if else if tree

			#if DEBUG
				std::cout << "--------------------------------------------------------------------" << std::endl;
				std::cout << "Potential activity on monitored[" << i << "], fd " << current.fd << std::endl;
			#endif
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
			if (!current.revents)//will we actually ever enter here?
			{
				#if DEBUG
					std::cout << "No Revents"<< std::endl;
				#endif
				continue;
			}
			#if DEBUG
				std::cout << "[REVENTS] (Raw Value: " << current.revents << ")" << std::endl;
			#endif
			if (i == 0 && (current.revents & current.events)) //listener will always be [0]
				AcceptClient();
			else if (current.revents & current.events)
			{
				if (current.revents & POLLIN) // might be redundant later on
				{
					#if DEBUG
						std::cout << "- POLLIN" << std::endl;
					#endif
					int out = OnClientRead(i);
					if (out)
						return out;
				}
				// Write (cutrisimo y asqueroso)
				if (current.events & POLLOUT && client.GetWriteBuffer().length() > 0)
				{
					int out = OnClientSend(i);
					if (out)
						return out;
				}
			}
			#if DEBUG
				if (current.revents & POLLOUT)
					std::cout << "- POLLOUT" << std::endl;
				if (current.revents & POLLNVAL)
					std::cout << "- POLLNVAL" << std::endl;
				if (current.revents & POLLHUP)
					std::cout << "- POLLHUP" << std::endl;
				std::cout << "--------------------------------------------------------------------" << std::endl;
			#endif
			current.revents = 0; // do we need to do this? //we need to reset it, right?
			pollret--;
		}
	}
	std::cout << "Shutting down server..." << std::endl;
	std::cout << pollfds.size() << std::endl;
	for (ssize_t i = pollfds.size() - 1; i >= 0; i--)
	{
		std::cout << "Disconnecting FD " << pollfds[i].fd << std::endl;
		DisconnectClient(i);
	}
	return (0);//in case we want to return errors
}

Client &Server::getClientAtIndex(size_t index)
{
	std::list<Client>::iterator it = clients.begin();
	while (index--)
		it++;
	return *it;
}
