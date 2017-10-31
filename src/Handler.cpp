//
// Created by iman on 10/27/17.
//

#include "Handler.h"

#include <fstream>
#include <iostream>
#include <sstream>

Handler::Handler(const std::string &readAddress, const std::string &resAddress,
                 int size, int percent) : _resAddress(resAddress) {
    _container = new Container(size);
    _container->readFlows(readAddress);
    float ratio = percent / 100.0f;
    auto rtCount = static_cast<int>(_container->getFlowsCount() * ratio);
    _transmitter = new Transmitter(size * size);
    _size = size;
}

void Handler::writeFlowToFile(Flow *flow, int index) {
    std::ofstream out;
    std::string indexStr;
    std::stringstream ss;
    ss << index;
    ss >> indexStr;
    out.open(_resAddress + "_" + indexStr + ".txt",
             std::ofstream::out | std::ofstream::app);
    out << flow->getStart()->getNumber() << " " << flow->getEnd()->getNumber() <<
        " " << flow->getSendCycle() << std::endl;
    out.close();
}
void Handler::writeDelays() {
    double rtSum = 0.0, nrtSum = 0.0;
    int rtCount = 0, nrtCount = 0;
    for (auto flow : _container->getFlows()) {
        if (flow->getType() == FlowType::RT) {
            ++rtCount;
            rtSum += flow->getDelay();
        }
        else if (!flow->getPassedByWire()) {
            ++nrtCount;
            nrtSum += flow->getDelay();
        }
    }
    std::cout << "rt mean delay : " << rtSum / rtCount << std::endl;
    std::cout << "nrt mean delay : " << nrtSum / nrtCount << std::endl;
}

void Handler::clear() {
    for (auto flow : _container->getFlows())
        flow->clear();
    _transmitter->clear();
}

void Handler::handle() {
    _container->sortFlows();
    for (int i = 0; i <= 2 * (_size - 1); ++i) {
        for (auto flow : _container->getFlows()) {
            if (flow->getType() == FlowType::NRT && flow->getLength() < i) {
                flow->setPassedByWire(true);
                writeFlowToFile(flow, i);
            }
            else
                _transmitter->sendFlow(flow);
        }
        _transmitter->finalUpdate();
        std::cout << ">= " << i << " passed by wireless" << std::endl;
        writeDelays();
        std::cout << std::endl;
        clear();
        if (i == 0)
            i = _size - 2;
    }
}

Handler::~Handler() {
    delete _container;
    delete _transmitter;
}

