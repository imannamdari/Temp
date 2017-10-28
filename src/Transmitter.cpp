//
// Created by iman on 10/27/17.
//

#include "Transmitter.h"

#include <limits>
#include <iostream>

Transmitter::Transmitter(int nodeCount, int maxSize) :
        _maxSize(maxSize), _nodeCount(nodeCount), _clock(0), _turn(0) {
    _rrs.resize(_nodeCount);
}

void Transmitter::sendFlow(Flow *flow) {
    update(flow->getSendCycle());
    if (flow->getType() == FlowType::RT) {
        if (_rrs[flow->getStart()->getNumber()].rt.size() < _maxSize)
            _rrs[flow->getStart()->getNumber()].rt.push_back(flow);
        else
            _rrs[flow->getStart()->getNumber()].buffer.push_back(flow);
    }
    else
        _rrs[flow->getStart()->getNumber()].nrt.push_back(flow);
}

void Transmitter::finalUpdate() {
    for (auto &rr : _rrs)
        updateRR(rr, std::numeric_limits<int>::max());
}

int Transmitter::updateFrontPacket(std::deque<Flow *> &rr, int clockCount) {
    int delayCount = std::min(clockCount, rr.front()->getNeededCycles());
    rr.front()->decrementNeededCycles(delayCount);
    incrementFlowsDelay(delayCount);
    if (!rr.front()->getNeededCycles()) {
        incrementTurn();
        rr.pop_front();
    }
    return delayCount;
}
void Transmitter::updateRR(RR &rr, int clock) {
    int clockCount = clock - _clock;
    while (clockCount > 0 &&
           (!rr.rt.empty() || !rr.buffer.empty() || !rr.nrt.empty())) {
        // First we should check non-complete packets and if all were complete
        // we should check them based on the priority.
        if (!rr.rt.empty() && !rr.rt.front()->isCompletePacket()) {
            int delayCount = updateFrontPacket(rr.rt, clockCount);
            clockCount -= delayCount;
        }
        else if (!rr.nrt.empty() && !rr.nrt.front()->isCompletePacket()) {
            int delayCount = updateFrontPacket(rr.nrt, clockCount);
            clockCount -= delayCount;
        }
        else if (!rr.rt.empty()) {
            int delayCount = updateFrontPacket(rr.rt, clockCount);
            clockCount -= delayCount;
        }
        else if (!rr.buffer.empty()) {
            Flow *front = rr.buffer.front();
            rr.rt.push_back(front);
            rr.buffer.pop_front();
        }
        else if (!rr.nrt.empty()) {
            int delayCount = updateFrontPacket(rr.nrt, clockCount);
            clockCount -= delayCount;
        }
    }
}
void Transmitter::update(int clock) {
    if (_clock) {
        if (!rrsEmpty())
            while (_rrs[_turn].empty())
                incrementTurn();
        updateRR(_rrs[_turn], clock);
    }
    _clock = clock;
}

void Transmitter::incrementFlowsDelay(RR &rr, int count) {
    for (auto flow : rr.rt)
        flow->incrementDelay(count);
    for (auto flow : rr.buffer)
        flow->incrementDelay(count);
    for (auto flow : rr.nrt)
        flow->incrementDelay(count);
}
void Transmitter::incrementFlowsDelay(int count) {
    for (auto &rr : _rrs)
        incrementFlowsDelay(rr, count);
}
