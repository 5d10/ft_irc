#include "Client.hpp"

ssize_t Client::Read()
{
	char buffer[1];
	ssize_t bytes_read = recv(fd, buffer, sizeof(buffer), MSG_DONTWAIT);
	while (bytes_read > 0 && buffer[bytes_read - 1] != '\n')
	{
		for (size_t j = 0; j < sizeof(buffer); j++)
			rd_buff += buffer[j];
		// std::cout  << "msg: " << msg << std::endl;
		bytes_read = recv(fd, buffer, sizeof(buffer), MSG_DONTWAIT); // this can block? (nc -C + Ctrl-D) // is client socket non-blocking?
	}
	return bytes_read;
}

ssize_t Client::Send()
{
	char buffer[1];
	buffer[0] = wr_buff.c_str()[0];
	ssize_t out = send(fd, buffer, sizeof(buffer), MSG_DONTWAIT);
	while (out > 0 && wr_buff.length() > 0)
	{
		if (wr_buff.length() > 1)
		{
			wr_buff = wr_buff.substr(1);
			buffer[0] = wr_buff.c_str()[0];
			send(fd, buffer, sizeof(buffer), MSG_DONTWAIT);
		}
		else { wr_buff = ""; }
	}
	return out;
}
