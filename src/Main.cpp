#include <ctime>
#include <iostream>
#include <sys/stat.h>
#include "dirent.h"
#include <sstream>
#include <cassert>
#include <algorithm>
#include <map>
#include <fstream>

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
std::string removeTxt(const std::string &str) {
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
    res;
    return res;
}

int uniqueSize;

int main(int argc, char **argv) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    DIR *dir;
    struct dirent *ent;
    assert(argc == 3);
    std::string dirName = argv[1];
    int farNumber;
    {
        std::string farString = argv[2];
        std::stringstream ss;
        ss << farString;
        ss >> farNumber;
    }
    int nrtStock = 1;
    std::vector<std::string> dirs;
    std::map<std::string, std::map<std::string, double> > rtDelay, nrtDelay;
    if ((dir = opendir(dirName.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string name = ent->d_name;
            if (name != "." && name != "..")
                dirs.push_back(name);
        }
    }
    else
        std::cout << "can't open" << std::endl;
    for (const auto &name : dirs)
        mkdir((dirName + "/" + name + "/out").c_str(), 0777);
    for (const auto &name : dirs)
        mkdir(("BookSim/" + name).c_str(), 0777);
    for (const auto &name : dirs) {
        std::vector<std::string> files;
        if ((dir = opendir((dirName + "/" + name).c_str())) != nullptr) {
            while ((ent = readdir(dir)) != nullptr) {
                std::string fileName = ent->d_name;
                if (fileName != "." && fileName != ".." && fileName != "out")
                    files.push_back(fileName);
            }
        }
        std::sort(files.begin(), files.end());
        for (const auto &file : files) {
            std::string fileName = removeTxt(file);
            uniqueSize = getSize(file);
            Handler *handler = new Handler(dirName + "/" + name + "/" + fileName,
                                           dirName + "/" + name + "/out/" +
                                           getOutFileName(fileName),
                                           dirName + "/" + name + "/../../BookSim/" +
                                           name + "/" + dirName, fileName,
                                           uniqueSize, 5, nrtStock);
            handler->sort();
            if (!handler->handleI(farNumber))
                continue;
            auto delay = handler->getDelay();
            std::string fileNameI = getOutFileName(fileName) + "_" +
                    std::to_string(farNumber);
            rtDelay[fileNameI][fileName] += delay.first;
            nrtDelay[fileNameI][fileName] += delay.second;
            handler->clear();
            delete handler;
        }
    }
    /*for (const auto &next : rtDelay) {
        for (const auto &next2 : next.second) {
            std::string rtName = dirName + "/" + next.first + "_RT.txt";
            std::ofstream out;
            out.open(rtName, std::ofstream::out | std::ofstream::app);
            out << next2.second / dirs.size() << " ";
        }
    }
    for (const auto &next : nrtDelay) {
        for (const auto &next2 : next.second) {
            std::string nrtName = dirName + "/" + next.first + "_NRT.txt";
            std::ofstream out;
            out.open(nrtName, std::ofstream::out | std::ofstream::app);
            out << next2.second / dirs.size() << " ";
        }
    }*/
    return 0;
}