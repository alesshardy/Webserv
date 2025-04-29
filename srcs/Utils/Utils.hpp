#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <sstream>
# include <limits>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <string>


int ft_stoi(const std::string &input);
unsigned long long ft_stoll(const std::string &input);
std::string toString(int value);

//checkers

bool directoryExists(const std::string &path);
bool fileExists(const std::string &path);



#endif