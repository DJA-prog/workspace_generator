#include <iostream>
#include <string>

std::string dropLastLevel(const std::string &path)
{
   return path.substr(0, path.rfind('/'));
}

int main()
{
	std::string path = "/home/save/path/main.cpp";
	std::cout << dropLastLevel(path) << std::endl;
	return 0;
}
