//
// Created by iman on 10/27/17.
//

#include "Handler.h"

#include <fstream>
#include <iostream>

Handler::Handler(const std::string &readAddress, const std::string &resAddress,
                 int size, int percent) : _resAddress(resAddress) {
    _container = new Container(size);
    _container->readFlows(readAddress, percent);
    float ratio = percent / 100.0f;
    auto rtCount = static_cast<int>(_container->getFlowsCount() * ratio);
    _transmitter = new Transmitter(rtCount);
}

void Handler::writeFlowToFile(Flow *flow) {
    std::ofstream out;
    out.open(_resAddress, std::ofstream::out | std::ofstream::app);
    out << flow->getStart()->getNumber() << " " << flow->getEnd()->getNumber() <<
        std::endl;
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

void Handler::handle() {
    _container->sortFlows();
    for (auto flow : _container->getFlows()) {
        if (_transmitter->ifShouldSendByWire(flow)) {
            writeFlowToFile(flow);
            flow->setPassedByWire(true);
        }
        else
            _transmitter->sendFlow(flow);
    }
    _transmitter->finalUpdate();
    writeDelays();
}
