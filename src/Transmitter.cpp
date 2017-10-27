//
// Created by iman on 10/27/17.
//

#include "Transmitter.h"

Transmitter::Transmitter(int maxSize) : _maxSize(maxSize), _clock(0) {
}

void Transmitter::sendFlow(Flow *flow) {
    update(flow->getSendCycle());
    if (flow->getType() == FlowType::RT) {
        if (_rtRR.size() < _maxSize)
            _rtRR.push_back(flow);
        else
            _rtBuffer.push_back(flow);
    }
    else
        _nrtRR.push_back(flow);
}

void Transmitter::update(int clock) {
    if (_clock) {
        int clockCount = clock - _clock;
        while (clockCount > 0 &&
               (!_rtRR.empty() || _rtBuffer.empty() || !_nrtRR.empty())) {
            if (!_rtRR.empty()) {
                int delayCount = std::min(clockCount,
                                          _rtRR.front()->getNeededCycles());
                clockCount -= delayCount;
                _rtRR.front()->decrementNeededCycles(delayCount);
                if (!_rtRR.front()->getNeededCycles())
                    _rtRR.pop_front();
                incrementFlowsDelay(delayCount);
            }
            else if (!_rtBuffer.empty()) {
                Flow *front = _rtBuffer.front();
                _rtRR.push_back(front);
                _rtBuffer.pop_front();
            }
            else if (!_nrtRR.empty()) {
                int delayCount = std::min(clockCount,
                                          _nrtRR.front()->getNeededCycles());
                clockCount -= delayCount;
                _nrtRR.front()->decrementNeededCycles(delayCount);
                if (!_nrtRR.front()->getNeededCycles())
                    _nrtRR.pop_front();
                incrementFlowsDelay(delayCount);
            }
        }
    }
    _clock = clock;
}

void Transmitter::incrementFlowsDelay(int count) {
    for (auto flow : _rtRR)
        flow->incrementDelay(count);
    for (auto flow : _rtBuffer)
        flow->incrementDelay(count);
    for (auto flow : _nrtRR)
        flow->incrementDelay(count);
}
