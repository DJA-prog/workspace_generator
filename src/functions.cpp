#include <string>
#include <fstream>
#include <yaml/yaml.hpp>

int getLevel(const std::string &line)
{

}

std::string getTemplate(const std::string &templatePath)
{
    std::ifstream templateFile(templatePath);
    std::string line;
    bool flagInContent = false;

    while (std::getline(templateFile, line))
    {

    }

}