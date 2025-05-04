#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>
# include <sstream>
# include <limits>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <string>
#include <map>


int ft_stoi(const std::string &input);
unsigned long long ft_stoll(const std::string &input);
std::string toString(int value);

//checkers

bool directoryExists(const std::string &path);
bool fileExists(const std::string &path);

bool createDirectory(const std::string& path);

std::string sanitizeFileName(const std::string& fileName);
std::string extractFileNameFromMultipart(const std::string& rawData);

bool isFileTransfer(const std::string& method, const std::map<std::string, std::string>& headers, const std::string& body);

std::string getStatusCodeMessage(int statusCode);


#endif