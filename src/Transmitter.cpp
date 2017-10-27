//
// Created by iman on 10/27/17.
//

#include "Transmitter.h"

#include <limits>

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

void Transmitter::finalUpdate() {
    update(std::numeric_limits<int>::max());
}

int Transmitter::updateFrontPacket(std::deque<Flow *> &rr, int clockCount) {
    int delayCount = std::min(clockCount, rr.front()->getNeededCycles());
    rr.front()->decrementNeededCycles(delayCount);
    if (!rr.front()->getNeededCycles())
        rr.pop_front();
    return delayCount;
}
void Transmitter::update(int clock) {
    if (_clock) {
        int clockCount = clock - _clock;
        while (clockCount > 0 &&
               (!_rtRR.empty() || _rtBuffer.empty() || !_nrtRR.empty())) {
            // First we should check non-complete packets and if all were complete
            // we should check them based on the priority.
            if (!_rtRR.empty() && !_rtRR.front()->isCompletePacket()) {
                int delayCount = updateFrontPacket(_rtRR, clockCount);
                clockCount -= delayCount;
                incrementFlowsDelay(delayCount);
            }
            else if (!_nrtRR.empty() && !_nrtRR.front()->isCompletePacket()) {
                int delayCount = updateFrontPacket(_nrtRR, clockCount);
                clockCount -= delayCount;
                incrementFlowsDelay(delayCount);
            }
            if (!_rtRR.empty()) {
                int delayCount = updateFrontPacket(_rtRR, clockCount);
                clockCount -= delayCount;
                incrementFlowsDelay(delayCount);
            }
            else if (!_rtBuffer.empty()) {
                Flow *front = _rtBuffer.front();
                _rtRR.push_back(front);
                _rtBuffer.pop_front();
            }
            else if (!_nrtRR.empty()) {
                int delayCount = updateFrontPacket(_nrtRR, clockCount);
                clockCount -= delayCount;
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
