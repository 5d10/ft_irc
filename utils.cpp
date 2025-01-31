#include "utils.hpp"

//it would be nice if you added the capacity for it to treat things between quotes
//	as a solid block IF told to do so i.e. bool argument
//it would be nice if you added the capacity for it to treat many consecutive
//	separators as one IF told to do so i.e. bool argument
std::vector<std::string> string_split(const std::string &string, const char &separator)
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
		from = until + 1;
		until = string.find(separator, from);
	}
	result.push_back(string.substr(from, -1));
	return (result);
}

std::list<std::string>::iterator list_find(std::list<std::string> list, std::string searched)
{
	std::list<std::string>::iterator i = list.begin();
	const std::list<std::string>::iterator end = list.end();
	while (i != end && *i != searched) ++i;
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
