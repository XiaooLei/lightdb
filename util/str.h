#include <vector>
#include <string>
#include <cstring>
#include <regex>

namespace lightdb{
inline std::vector<std::string> splitWithStl(const std::string &str,const std::string &pattern)
{
    std::vector<std::string> resVec;

	if (str.empty())
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

inline int stringToInt(const std::string& str, bool& suc){
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

inline double stringToDouble(const std::string& str, bool& suc){
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
inline void splitStrBySpace(const std::string& text, std::vector<std::string>& words){
        std::string word;
        for(auto c : text){
            std::string res;
            if(c == ' '){
                if(!word.empty()){
                    words.push_back(word);
                }
                word = "";
                continue;
            }else{
                word += c;
            }
        }
        if(!word.empty()) {
            words.push_back(word);
        }
}

}
