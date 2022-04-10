#include <vector>
#include <string>
#include <string.h>
#include <regex>

namespace lightdb{
inline std::vector<std::string> splitWithStl(const std::string &str,const std::string &pattern)
{
    std::vector<std::string> resVec;

	if ("" == str)
    {
        return resVec;
    }
    //方便截取最后一段数据
    std::string strs = str + pattern;
    
    size_t pos = strs.find(pattern);
    size_t size = strs.size();

    while (pos != std::string::npos)
    {
        std::string x = strs.substr(0,pos);
        resVec.push_back(x);
        strs = strs.substr(pos+1,size);
        pos = strs.find(pattern);
    }
    
    return resVec;
}

inline int stringToInt(std::string str, bool& suc){
    int res;
    try{
        res = std::stoi(str);
    } catch (...) {
        suc = false;
        return res;
    }
    suc = true;
    return res;
}

inline double stringToDouble(std::string str, bool& suc){
    double res;
    try{
        res = std::stod(str);
    } catch (...) {
        suc = false;
        return res;
    }
    suc = true;
    return res;
}

//用空格分割字符串
inline void splitStrBySpace(std::string text, std::vector<std::string>& words){
        std::string word = "";
        for(auto c : text){
            std::string res;
            if(c == ' '){
                if(word.size() != 0){
                    words.push_back(word);
                }
                word = "";
                continue;
            }else{
                word += c;
            }
        }
        if(word.size() != 0) {
            words.push_back(word);
        }
}

// 去掉s中首尾的del子字符串
inline std::string& Strim(std::string &s, const std::string & del)
{
    if (s.empty())
    {
        return s;
    }

    s.erase(0,s.find_first_not_of(del));
    s.erase(s.find_last_not_of(del) + 1);
    return s;
}

inline std::vector<std::string> Split(const std::string & input, const std::string& regex)
{
    // passing -1 as the submatch index parameter performs splitting
    std::string key;
    std::string value;
    int count = 0;
    for(auto c : input){
        if(c == '='){
            count ++;
        }
    }
    if(count != 1){
        return {};
    }
    bool flag = false;
    for(auto c : input){
        if(c == '='){
            flag = true;
            continue;
        }
        if(c == ' '){
            continue;
        }
        if(flag){
            key += c;
        }else{
            value += c;
        }
    }
    return {key, value};
}


}
