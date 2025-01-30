//These two are aligned by index
std::list<Client> clients;
std::vector<pollfds> fds;//MUST be a vector
//We can't know for sure each time where in memmory our client will be
// so searching is needed

std::map<std::string, Client*>;
Client& find_client(unsigned int i, std::list<Client> clients);//already exisst as map::find()

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
