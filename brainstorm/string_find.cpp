#include <iostream>
#include <string>

int main(int argc, char *arg[]) {
    std::string s = " 5 63 456 dfr ";
    std::string s1 = " ";
    s.erase(0, s.find_first_not_of(s1));
    s.erase(s.find_last_not_of(s1) + 1);
    std::cout << s << "end" << std::endl;
    return 0;
}
