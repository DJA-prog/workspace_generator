#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <string>

namespace fs = boost::filesystem;

class ProjectGenerator
{
public:
    ProjectGenerator(const std::string &templateFile, const std::string &projectName)
        : templateFile_(templateFile), projectName_(projectName) {}

    void createDirectory(const std::string &dirname)
    {
        fs::create_directory(dirname);
    }

    void createFile(const std::string &filename)
    {
        std::ofstream file(filename);
        if (file)
        {
            std::cout << "File Created: " << filename << std::endl;
        }
        else
        {
            std::cerr << "Failed to create file: " << filename << std::endl;
        }
    }

    std::string replaceSubstring(const std::string &original, const std::string &substring, const std::string &replacement)
    {
        std::string result = original;
        size_t pos = 0;

        while ((pos = result.find(substring, pos)) != std::string::npos)
        {
            result.replace(pos, substring.length(), replacement);
            pos += replacement.length();
        }

        return result;
    }

    std::string toLowercase(const std::string &str)
    {
        std::string result;
        for (char c : str)
        {
            result += std::tolower(c);
        }
        return result;
    }

    int checkLevel(const std::string &line)
    {
        int level = 0;
        for (int i = 0; i < line.length(); i++)
        {
            if (line[i] == '\t')
                level++;
            else
                return level;
        }
        return level;
    }

    std::string whatEntryType(const std::string &line)
    {
        bool flag_level_section = true;
        std::string type = "";
        for (int i = 0; i < line.length(); i++)
        {
            if ((line[i] != '\t' || line[i] == ' ') && flag_level_section)
                flag_level_section = false;
            
            if (flag_level_section == false)
            {
                if (line[i] == ' ')
                    return "";
                if (line[i] == ':')
                    return type;

                type += line[i];
            }
        }

        return "";
    }

    std::string entryLineContent(const std::string &line)
    {
        bool flag_level_section = true;
        bool flag_skip_next = false;
        std::string lineContent = "";
        for (int i = 0; i < line.length(); i++)
        {
            if ((line[i] != '\t' || line[i] == ' ') && flag_level_section)
                flag_level_section = false;

            if (flag_level_section == false && !flag_skip_next)
            {
                lineContent += line[i];

                if (line[i] == ':')
                {
                    lineContent = "";
                    flag_skip_next = true;
                }
            }
            else
            {
                flag_skip_next = false;
            }
        }

        return replaceSubstring(lineContent, "<project_name>", projectName_);
    }

    std::string dropLastLevel(const std::string &path)
    {
        return path.substr(0, path.rfind('/'));
    }

    std::string removeSubstringFromFront(const std::string &original, const std::string &substring)
    {
        std::string result = original;

        if (result.compare(0, substring.length(), substring) == 0)
        {
            result.erase(0, substring.length());
        }

        return result;
    }

    void writeContentToLastFile(const std::string &lastFilePath, const int lineNumber)
    {
        std::ifstream template_file(templateFile_);
        std::string line;
        std::string content = "";
        int currentLine = 0;
        bool flag_level_checked = false;
        int level = 0;
        std::string trimContent = "";

        while (std::getline(template_file, line))
        {
            if (flag_level_checked == false)
            {
                level = checkLevel(line);
                for (int i = 0; i < level; i++)
                {
                    trimContent += '\t';
                }
                flag_level_checked = true;
            }
            

            if (currentLine > lineNumber)
            {
                if (line.find("endContent:") != std::string::npos)
                {
                    template_file.close();
                    break;
                }
                if (line.find("<project_name>") != std::string::npos)
                {
                    content += removeSubstringFromFront(replaceSubstring(line, "<project_name>", projectName_), trimContent) + '\n';
                }
                else
                {

                    content += removeSubstringFromFront(line, trimContent) + '\n';
                }
            }
            currentLine++;
        }

        std::ofstream file(lastFilePath);

        if (file.is_open())
        {
            file << content;
            file.close();
            std::cout << "File written successfully." << std::endl;
        }
        else
        {
            std::cout << "Unable to open the file." << std::endl;
        }
    }

    void createStructure()
    {
        std::ifstream template_file(templateFile_);
        std::string line;
        std::string currentDir;
        int lineNumber = 0;
        int level = 0;
        int currentLevel = 0;
        std::string pwd = ".";
        std::string entryType = "";
        std::string lastEntryType = "";
        bool flag_content_section = false;
        std::string filename = "";

        while (std::getline(template_file, line))
        {
            entryType = whatEntryType(toLowercase(line));
            currentLevel = checkLevel(line);

            if(flag_content_section == false)
            {
                if (entryType == "dir")
                {
                    std::string dirName = entryLineContent(line);
                    if (currentLevel == 0)
                        pwd = "./" + dirName;

                    else if (currentLevel < level)
                        pwd += '/' + dirName;

                    else if (currentLevel == level && level > 0)
                        pwd = dropLastLevel(pwd)  + '/' + dirName;

                    else if (currentLevel == level)
                        pwd += '/' + dirName;
                    
                    else if (currentLevel > level)
                        pwd = dropLastLevel(dropLastLevel(pwd)) + '/' + dirName;
                    
                    std::cout << "PWD: " <<  pwd << " LEVELS: "<< level << ":" << currentLevel << std::endl;
                    createDirectory(pwd);
                }
                else if (entryType == "file")
                {
                    filename = entryLineContent(line);
                    createFile(pwd + "/" + filename);
                    lastEntryType = entryType;
                }
                else if (entryType == "content")
                {
                    flag_content_section = true;
                    writeContentToLastFile(pwd + "/" + filename, lineNumber);
                }
            }
            else if (entryType == "endcontent" && flag_content_section == true)
            {
                flag_content_section = false;
            }
            lineNumber++;
            level = currentLevel;
        }
    }

private:
    std::string templateFile_;
    std::string projectName_;
};

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <template_file> <project_name>" << std::endl;
        return 1;
    }

    std::string templateFile = argv[1];
    std::string projectName = argv[2];

    ProjectGenerator generator(templateFile, projectName);
    generator.createStructure();

    return 0;
}
