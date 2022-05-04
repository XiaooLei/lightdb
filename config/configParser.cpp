//
// Created by xiaolei on 2022/3/19.
//
#include <fstream>
#include "configParser.h"
#include "../util/str.h"

namespace lightdb{

    ConfigParser::ConfigParser() = default;

    ConfigParser::~ConfigParser() = default;

    string ConfigParser::GetConfig(const string &Key, bool& exist) {
        if(this->_configData.find(Key) != _configData.end()){
            exist = true;
            return _configData[Key];
        }else{
            exist = false;
            return "";
        }
    }

    string ConfigParser::GetDefConfig(const string &Key, const string &Def) {
        if(this->_configData.find(Key) != _configData.end()){
            return _configData[Key];
        }else{
            return Def;
        }
    }

    bool ConfigParser::Parser(const string &filePath) {
        // 以写模式打开文件
        ifstream infile;
        infile.open(filePath);

        for(;!infile.eof();) {
            char buf[100];
            infile.getline(buf, sizeof(buf));
            std::string line;
            line.assign(buf);
            if(line.find('#') == 0){
                continue;
            }
            std::vector<std::string> keyAndValue;
            splitStrBySpace(line, keyAndValue);
            if (keyAndValue.size() > 2) {
                printf("invalid config file \n");
                exit(0);
            }else if(keyAndValue.size() < 2){
                continue;
            }
            this->_configData.insert(std::make_pair(keyAndValue[0], keyAndValue[1]));
        }
    }
}
