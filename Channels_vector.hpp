//THIS IS A BACKUP PLAN
//refer to "Classes_2" instead

//These two are aligned by index
std::vector<Client> clients;
std::vector<pollfds> fds;

//clients[i]

//We can't know for sure each time where in memmory our client will be
// so searching is needed
unsigned int find_client(std::string name, std::vector<Client> clients);
//Used every time we send anything to it

class Channel
{
	std::string name;
	std::vector<std::string> clients;
	//other stuff
	//.
	//.
	//.
}

class Channels
{
	std::vector<Channel> channels;
	public:
		void removeFromAll();
};
