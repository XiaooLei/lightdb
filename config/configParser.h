//
// Created by 9 on 2022/3/19.
//

#ifndef MYPROJECT_CONFIG_PARSER_H
#define MYPROJECT_CONFIG_PARSER_H

#include <map>
#include <string>
#include <vector>

using namespace std;
namespace lightdb
{

class ConfigParser
{
public:
    ConfigParser();
    ~ConfigParser();
public:
    bool Parser(const string & cFilePath);
    string GetConfig(const string & Key, bool& exist);
    string GetDefConfig(const string & Key, const string & Def);
private:
    map<string, string> _configData;
};

}

#endif //MYPROJECT_CONFIG_PARSER_H
