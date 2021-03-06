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
    out << flow->getSendCycle() << " " << flow->getStart()->getNumber() << " " <<
        flow->getEnd()->getNumber() << " " << flitCount << std::endl;
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
    out.open(fileName + "_RTR.txt", std::ofstream::out | std::ofstream::app);
    out << rtSum / rtCount << " ";
    out.close();
    out.open(fileName + "_NRTWLR.txt", std::ofstream::out | std::ofstream::app);
    out << nrtSum / nrtCount << " ";
    out.close();
}
void Handler::writeNRTCountToFile(const std::string &fileName,
                                  int wireCount, int wirelessCount) {
    std::ofstream out;
    out.open(fileName + "_NRTWC.txt", std::ofstream::out | std::ofstream::app);
    out << wireCount << " ";
    out.close();
    out.open(fileName + "_NRTWLC.txt", std::ofstream::out | std::ofstream::app);
    out << wirelessCount << " ";
    out.close();
}
void Handler::writeRTCountToFile(const std::string &fileName, int count) {
    std::ofstream out;
    out.open(fileName + "_RTC.txt", std::ofstream::out | std::ofstream::app);
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
bool Handler::mustTerminate(const std::string &fileName) {
    std::ifstream fin;
    fin.open(fileName);
    double next;
    while (fin >> next)
        if (next > threshold)
            return true;
    fin.close();
    return false;
}
bool Handler::handleI(int i) {
    std::string iStr = std::to_string(i);
    mkdir((_flowDir + "_" + iStr).c_str(), 0777);
    int wiredCount = 0, wirelessCount = 0;
    int rtCount = 0;
    std::string booksimFile = _resAddress + "_" + std::to_string(i) + "_" +
            _flowFile + "_booksim";
    for (auto flow : _container->getFlows()) {
        if (flow->getType() == FlowType::NRT && flow->getLength() < i) {
            flow->setPassedByWire(true);
            ++wiredCount;
            writeFlowToFile(booksimFile, flow);
        }
        else {
            if (flow->getType() == FlowType::RT)
                ++rtCount;
            else
                ++wirelessCount;
            _transmitter->sendFlow(flow, false);
        }
    }
    _transmitter->finalUpdate();
    //std::cout << ">= " << i << " passed by wireless" << std::endl;
    std::string fileName = _resAddress + "_" + std::to_string(i);
    //writeDelaysToFile(fileName);
    writeNRTCountToFile(fileName, wiredCount, wirelessCount);
    writeRTCountToFile(fileName, rtCount);
    writeDelaysToFile(fileName);
    std::string number;
    number.push_back(_flowFile[0]);
    system(("./booksim configs/con" + number + " " +
            booksimFile + ".txt " + fileName + "_NRTWR.txt").c_str());
    system(("rm " + booksimFile + ".txt").c_str());
    return !mustTerminate(fileName + "_NRTWR.txt");
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
        //writeNRTCountToFile(fileName, wiredCount);
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

