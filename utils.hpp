#ifndef IRC_UTILS_HPP
# define IRC_UTILS_HPP

# include <vector>
# include <string>
# include <list>

# define STRSPL_BUNDLE 1 //tells string_split to treat consecutive separators as one
std::vector<std::string> string_split(const std::string &string, const char &separator, unsigned char options = 0);

std::list<std::string>::iterator list_find(std::list<std::string>& list, std::string searched);

#endif
