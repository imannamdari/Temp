//
// Created by iman on 10/27/17.
//

#include "Container.h"

#include <fstream>
#include <cassert>
#include <algorithm>
#include <iostream>

Container::Container(int size) : _size(size) {
    for (int i = 0; i < _size; ++i)
        for (int j = 0; j < _size; ++j)
            _mesh.push_back(new Node(i * _size + j));
}

void Container::readFlows(const std::string &address) {
    std::ifstream fin;
    fin.open(address);
    int start, end, sendTime;
    std::string typeStr;
    while (fin >> start >> end >> sendTime >> typeStr) {
        FlowType type;
        if (typeStr == "RT")
            type = FlowType::RT;
        else if (typeStr == "NRT")
            type = FlowType::NRT;
        else
            assert(false);
        if (start != end)
            _flows.push_back(new Flow(_mesh[start], _mesh[end], sendTime, type));
        else
            std::cout << "warning : start and end of the flow is equal!"
                      << std::endl;
    }
    fin.close();
}

void Container::sortFlows() {
    std::sort(_flows.begin(), _flows.end(), Compare());
}
