//
// Created by iman on 10/27/17.
//

#include "Handler.h"

#include <fstream>

Handler::Handler(const std::string &readAddress, const std::string &resAddress,
                 int size, int percent) : _resAddress(resAddress) {
    _container = new Container(size);
    _container->readFlows(readAddress);
    float ratio = percent / 100.0f;
    auto rtCount = static_cast<int>(_container->getFlowsCount() * ratio);
    _transmitter = new Transmitter(rtCount);
}

void Handler::writeFlowToFile(Flow *flow) {
    std::ofstream out;
    out.open(_resAddress);
    out << flow->getStart()->getNumber() << " " << flow->getEnd()->getNumber() <<
        std::endl;
    out.close();
}

void Handler::handle() {
    _container->sortFlows();
    for (auto flow : _container->getFlows()) {
        if (_transmitter->ifShouldSendByWire(flow))
            writeFlowToFile(flow);
        else
            _transmitter->sendFlow(flow);
    }
}
