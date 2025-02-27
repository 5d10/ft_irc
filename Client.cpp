#include "Client.hpp"

Client::Client()
{
	this->fd = -1;
	rd_buff = "";
	wr_buff = "";
	registered = false;
}

Client::Client(int fd/*, short *events_ref*/)
{
	this->fd = fd;
	//events = events_ref;
	rd_buff = "";
	wr_buff = "";
	registered = false;
	passed = false;
	nicked = false;
	usernamed = false;
	//events = NULL;
}
Client::Client(const Client &other)
{
	fd = other.fd;
	rd_buff = other.rd_buff;
	wr_buff = other.wr_buff;
	registered = other.registered;
	passed = other.passed;
	nicked = other.nicked;
	usernamed = other.usernamed;
	//events = other.events;
}
Client &Client::operator=(const Client &other)
{
	fd = other.fd;
	rd_buff = other.rd_buff;
	wr_buff = other.wr_buff;
	registered = other.registered;
	passed = other.passed;
	nicked = other.nicked;
	usernamed = other.usernamed;
	//events = other.events;
	return *this;
}

Client::~Client()
{

}

const int &Client::GetFD() const
{
	return fd;
}

const std::string &Client::GetReadBuffer() const
{
	return rd_buff;
}

const std::string &Client::GetWriteBuffer() const
{
	return wr_buff;
}

void Client::ClearReadBuffer()
{
	rd_buff = "";
}

void Client::AddToWriteBuffer(std::string msg)
{
	wr_buff += msg;
}

std::string Client::format_buffer(std::string buffer) const
{
	std::string s = "";
	for (unsigned int i = 0; i < buffer.size(); i++)
	{
		switch(buffer[i])
		{
			case '\r':
				s += COLOR_GRAY;
				s += "\\r";
				s += COLOR_NONE;
				break;
			case '\n':
				s += COLOR_GRAY;
				s += "\\n";
				s += COLOR_NONE;
				break;
			default:
				s += buffer[i];
		}
	}
	return s;
}

void Client::print_buffer(std::string buffer) const
{
	std::cout << COLOR_CYAN << "Buffer data: " << COLOR_NONE << "'" << format_buffer(buffer) << "'" << COLOR_NONE << std::endl;
}

ssize_t Client::Read()
{
	char buffer[1];
	ssize_t bytes_read = recv(fd, buffer, sizeof(buffer), MSG_DONTWAIT);
	while (bytes_read > 0 && buffer[bytes_read - 1] != '\n')
	{
		rd_buff += buffer[0];
		bytes_read = recv(fd, buffer, sizeof(buffer), MSG_DONTWAIT); // this can block? (nc -C + Ctrl-D) // is client socket non-blocking?
	}
	rd_buff += buffer[0];
	std::cout << COLOR_YELLOW << "[SERVER] << Read on FD " << fd << COLOR_NONE << std::endl;
	print_buffer(rd_buff);
	return bytes_read;
}

ssize_t Client::Send()
{
	std::cout << COLOR_PINK << "[SERVER] >> Write on FD " << fd << COLOR_NONE << std::endl;
	print_buffer(wr_buff);
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
	if (wr_buff.length() > 0)
	{
		std::cout << "WARNING: Buffer not empty after write on FD " << fd << "!" << std::endl;
		print_buffer(wr_buff);
	}
	return out;
}
