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

class CConfigParser
{
public:
    CConfigParser();
    ~CConfigParser();
public:
    bool Parser(const string & cFilePath);
    bool HasSection(const string & cSection);
    int GetSections(vector<string> & vecSections);
    int GetKeys(const string & strSection, vector<string> & vec);
    const map<string, string> * GetSectionConfig(const string & cSection);
    string GetConfig(const string & cSection, const string & cKey);
    string GetDefConfig(const string & cSection, const string & cKey, const string & cDef);
private:
    map<string, map<string, string> *> m_mpConfigData;
};

}

#endif //MYPROJECT_CONFIG_PARSER_H
