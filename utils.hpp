#ifndef IRC_UTILS_HPP
# define IRC_UTILS_HPP

#include <vector>
#include <string>
#include <list>

std::vector<std::string> string_split(const std::string &string, const char &separator);

//template<class C>
//typename std::list<C>::iterator list_find(std::list<C> list, C searched);
std::list<std::string>::iterator list_find(std::list<std::string> list, std::string searched);

#endif
