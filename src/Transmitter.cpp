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
    while (!rrsEmpty())
        while (updateRR(std::numeric_limits<int>::max()));
}

std::pair<bool, int> Transmitter::updateFrontPacket(std::deque<Flow *> &rr,
                                                    int clockCount) {
    int delayCount = std::min(clockCount, rr.front()->getNeededCycles());
    bool packetEnded = false;
    rr.front()->decrementNeededCycles(delayCount);
    incrementFlowsDelay(delayCount);
    if (!rr.front()->getNeededCycles()) {
        incrementTurn();
        rr.pop_front();
        packetEnded = true;
    }
    return std::make_pair(packetEnded, delayCount);
}
bool Transmitter::updateRR(int clock) {
    bool turnFinished = false;
    if (!rrsEmpty())
        while (_rrs[_turn].empty())
            incrementTurn();
    else
        return false;
    int clockCount = clock - _clock;
    while (clockCount > 0 &&
           (!_rrs[_turn].rt.empty() || !_rrs[_turn].buffer.empty() ||
            !_rrs[_turn].nrt.empty())) {
        // First we should check non-complete packets and if all were complete
        // we should check them based on the priority.
        if (!_rrs[_turn].rt.empty() && !_rrs[_turn].rt.front()->isCompletePacket()) {
            auto pair = updateFrontPacket(_rrs[_turn].rt, clockCount);
            clockCount -= pair.second;
            if (pair.first) {
                turnFinished = true;
                break;
            }
        }
        else if (!_rrs[_turn].nrt.empty() &&
                !_rrs[_turn].nrt.front()->isCompletePacket()) {
            auto pair = updateFrontPacket(_rrs[_turn].nrt, clockCount);
            clockCount -= pair.second;
            if (pair.first) {
                turnFinished = true;
                break;
            }
        }
        else if (!_rrs[_turn].rt.empty()) {
            auto pair = updateFrontPacket(_rrs[_turn].rt, clockCount);
            clockCount -= pair.second;
            if (pair.first) {
                turnFinished = true;
                break;
            }
        }
        else if (!_rrs[_turn].buffer.empty()) {
            Flow *front = _rrs[_turn].buffer.front();
            _rrs[_turn].rt.push_back(front);
            _rrs[_turn].buffer.pop_front();
        }
        else if (!_rrs[_turn].nrt.empty()) {
            auto pair = updateFrontPacket(_rrs[_turn].nrt, clockCount);
            clockCount -= pair.second;
            if (pair.first) {
                turnFinished = true;
                break;
            }
        }
    }
    _clock += (clock - _clock - clockCount);
    return turnFinished;
}
void Transmitter::update(int clock) {
    if (_clock) {
        while (updateRR(clock));
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
