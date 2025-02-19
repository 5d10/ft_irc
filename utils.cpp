#include "utils.hpp"
#include "Server.hpp"

//it would be nice if you added the capacity for it to treat things between quotes
//	as a solid block IF told to do so i.e. bool argument
std::vector<std::string> string_split(const std::string &string, const char &separator, const unsigned char options /*default value in hpp*/)
{
	int	from;
	int	until;
	std::vector<std::string> result;

	from = 0;
	until = string.find(separator, 0);
	if (until == -1)
	{
		result.push_back(string);
		return (result);
	}
	while (until != -1)
	{
		result.push_back(string.substr(from, until - from));
		if (options & STRSPL_BUNDLE)
			from = string.find_first_not_of(separator, until);
		else
			from = until + 1;
		until = string.find(separator, from);
	}
	result.push_back(string.substr(from, -1));
	return (result);
}

std::list<std::string>::iterator list_find(std::list<std::string>& list, std::string searched)
{
	std::list<std::string>::iterator i = list.begin();
	std::list<std::string>::iterator end = list.end();
	while (i != end && *i != searched)
		++i;
	#if DEBUG
		std::cout << "list_find: found = " << (i != end) << std::endl;
	#endif
	return (i);
}

/*
template<class C>
typename std::list<C>::iterator list_find(std::list<C> list, C searched)
{
	typename std::list<C>::iterator i = list.begin();
	const typename std::list<C>::iterator end = list.end();
	while (i != end && *i != searched) ++i;
	return (i);
}
*/
