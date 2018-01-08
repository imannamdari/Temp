//
// Created by iman on 10/27/17.
//

#include "Handler.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <set>

Handler::Handler(const std::string &readAddress, const std::string &resAddress,
                 const std::string &flowDir, const std::string &flowFile,
                 int size, int percent, int nrtStock) :
        _flowDir(flowDir), _flowFile(flowFile), _resAddress(resAddress) {
    _container = new Container(size);
    _container->readFlows(readAddress);
    float ratio = percent / 100.0f;
    auto rtCount = static_cast<int>(_container->getFlowsCount() * ratio);
    _transmitter = new Transmitter(size * size, nrtStock);
    _size = size;
}

void Handler::writeFlowToFile(const std::string &fileName, Flow *flow) {
    std::ofstream out;
    out.open(fileName + ".txt",
             std::ofstream::out | std::ofstream::app);
    out << flow->getStart()->getNumber() << " " << flow->getEnd()->getNumber() <<
        " " << flow->getSendCycle() << std::endl;
    out.close();
}
std::pair<double, double> Handler::getDelay() const {
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
    return std::make_pair(rtSum / rtCount, nrtSum / nrtCount);
}
void Handler::writeDelaysToFile(const std::string &fileName) {
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
    std::ofstream out;
    out.open(fileName + "_RT.txt", std::ofstream::out | std::ofstream::app);
    out << rtSum / rtCount << " ";
    out.close();
    out.open(fileName + "_NRT.txt", std::ofstream::out | std::ofstream::app);
    out << nrtSum / nrtCount << " ";
}
void Handler::writeCountToFile(const std::string &fileName, int count) {
    std::ofstream out;
    out.open(fileName + "_WireCount.txt", std::ofstream::out | std::ofstream::app);
    out << count << " ";
    out.close();
}

void Handler::clear() {
    for (auto flow : _container->getFlows())
        flow->clear();
    _transmitter->clear();
}

void Handler::sort() {
    _container->sortFlows();
}
void Handler::handleI(int i) {
    std::string iStr = std::to_string(i);
    mkdir((_flowDir + "_" + iStr).c_str(), 0777);
    int wiredCount = 0;
    for (auto flow : _container->getFlows()) {
        if (flow->getType() == FlowType::NRT && flow->getLength() < i) {
            flow->setPassedByWire(true);
            ++wiredCount;
            writeFlowToFile(_flowDir + "_" + iStr + "/" +
                            _flowFile + "_" + iStr, flow);
        }
        else
            _transmitter->sendFlow(flow, true);
    }
    _transmitter->finalUpdate();
    //std::cout << ">= " << i << " passed by wireless" << std::endl;
    std::string fileName = _resAddress + "_" + std::to_string(i);
    //writeDelaysToFile(fileName);
    writeCountToFile(fileName, wiredCount);
    //std::cout << std::endl;
    //clear();
    /*if (i == 0)
        i = (2 * (_size - 1) - _size / 2);*/
}
void Handler::handle() {
    for (int i = 0; i <= 2 * (_size - 1); ++i) {
        std::string iStr = std::to_string(i);
        mkdir((_flowDir + "_" + iStr).c_str(), 0777);
        int wiredCount = 0;
        for (auto flow : _container->getFlows()) {
            if (flow->getType() == FlowType::NRT && flow->getLength() < i) {
                flow->setPassedByWire(true);
                ++wiredCount;
                writeFlowToFile(_flowDir + "_" + iStr + "/" +
                                _flowFile + "_" + iStr, flow);
            }
            else
                _transmitter->sendFlow(flow, false);
        }
        _transmitter->finalUpdate();
        //std::cout << ">= " << i << " passed by wireless" << std::endl;
        std::string fileName = _resAddress + "_" + std::to_string(i);
        //writeDelaysToFile(fileName);
        writeCountToFile(fileName, wiredCount);
        //std::cout << std::endl;
        //clear();
        /*if (i == 0)
            i = (2 * (_size - 1) - _size / 2);*/
    }
    /*for (auto flow : _container->getFlows()) {
        if (flow->getType() == FlowType::RT)
            _transmitter->sendFlow(flow);
    }
    _transmitter->finalUpdate();
    writeDelays(_resAddress + );*/
    clear();
}

Handler::~Handler() {
    delete _container;
    delete _transmitter;
}

