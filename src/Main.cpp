#include <ctime>
#include <iostream>
#include <sys/stat.h>
#include "dirent.h"
#include <sstream>
#include <cassert>

#include "Handler.h"

int getSize(const std::string &fileName) {
    std::string size;
    std::stringstream ss;
    for (char i : fileName) {
        if (i == '_')
            break;
        size += i;
    }
    ss << size;
    int intSize;
    ss >> intSize;
    return intSize;
}
std::string removeTxt(std::string str) {
    std::string res = str;
    for (auto i = static_cast<int>(res.size() - 1); i >= 0; --i) {
        if (res[i] == '.') {
            res.pop_back();
            break;
        }
        res.pop_back();
    }
    return res;
}

int main(int argc, char **argv) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    DIR *dir;
    struct dirent *ent;
    assert(argc == 2);
    std::string dirName = argv[1];
    if ((dir = opendir(dirName.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string name = ent->d_name;
            if (name != "." && name != "..") {
                std::cout << name << " start" << std::endl;
                std::string temp = removeTxt(name);
                mkdir(("../res/out/" + removeTxt(name)).c_str(), 0777);
                Handler *handler = new Handler(dirName + "/" + name,
                                               "out/" + removeTxt(name) +
                                               "/" + removeTxt(name),
                                               getSize(name), 5);
                handler->handle();
                delete handler;
            }
        }
    }
    else
        std::cout << "can't open" << std::endl;
    return 0;
}