#include <ctime>
#include <iostream>
#include <sys/stat.h>
#include "dirent.h"
#include <sstream>
#include <cassert>
#include <algorithm>

#include "Handler.h"

std::vector<std::string> _files;

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

std::string getOutFileName(const std::string &name) {
    std::string res = name;
    for (auto i = static_cast<int>(res.size() - 1); i >= 0; --i) {
        if (res[i] == '_') {
            res.pop_back();
            break;
        }
        res.pop_back();
    }
    res += "_RT";
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
            if (name != "." && name != "..")
                _files.push_back(name);
        }
    }
    else
        std::cout << "can't open" << std::endl;
    std::sort(_files.begin(), _files.end());
    for (const auto &file : _files) {
        std::cout << file << " start" << std::endl;
        std::string temp = removeTxt(file);
        //mkdir((dirName + "/" + removeTxt(name)).c_str(), 0777);
        Handler *handler = new Handler(dirName + "/" + file,
                                       dirName + "/" +
                                       getOutFileName(removeTxt(file)),
                                       getSize(file), 5);
        handler->handle();
        delete handler;
    }
    return 0;
}