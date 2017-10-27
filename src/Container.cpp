//
// Created by iman on 10/27/17.
//

#include "Container.h"

#include <fstream>
#include <cassert>
#include <algorithm>

Container::Container(int size) : _size(size) {
    for (int i = 0; i < _size; ++i)
        for (int j = 0; j < _size; ++j)
            _grid.push_back(new Node(i * _size + j));
}

void Container::readFlows(const std::string &address) {
    std::ifstream fin;
    fin.open(address);
    for (auto start : _grid) {
        int flowCount;
        fin >> flowCount;
        for (int j = 0; j < flowCount; ++j) {
            int end, sendTime;
            std::string typeStr;
            fin >> end >> sendTime >> typeStr;
            FlowType type;
            if (typeStr == "RT")
                type = FlowType::RT;
            else if (typeStr == "NRT")
                type = FlowType::NRT;
            else
                assert(false);
            _flows.push_back(new Flow(start, _grid[end], sendTime, type));
        }
    }
    fin.close();
}

void Container::sortFlows() {
    std::sort(_flows.begin(), _flows.end(), Compare());
}
