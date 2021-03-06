//
// Created by iman on 10/27/17.
//

#include "Transmitter.h"

#include <limits>
#include <iostream>
#include <cassert>

Transmitter::Transmitter(int nodeCount, int nrtStock) :
        _nodeCount(nodeCount), _clock(0), _rtTurn(0), _nrtTurn(0),
        _nrtNumber(0), _nrtStock(nrtStock) {
    _rrs.resize(static_cast<unsigned long>(_nodeCount));
}

void Transmitter::sendFlow(Flow *flow, bool forceRT) {
    update(flow->getSendCycle());
    if (forceRT || flow->getType() == FlowType::RT)
        _rrs[flow->getStart()->getNumber()].rt.push_back(flow);
    else
        _rrs[flow->getStart()->getNumber()].nrt.push_back(flow);
}

void Transmitter::finalUpdate() {
    while (!rrsEmpty())
        while (updateRR(std::numeric_limits<int>::max()));
}

std::pair<bool, int> Transmitter::updateFrontPacket(std::deque<Flow *> &rr,
                                                    int clockCount,
                                                    FlowType type) {
    int delayCount = std::min(clockCount, rr.front()->getNeededCycles());
    bool packetEnded = false;
    rr.front()->decrementNeededCycles(delayCount);
    incrementFlowsDelay(delayCount);
    if (!rr.front()->getNeededCycles()) {
        if (type == FlowType::RT)
            incrementRTTurn();
        else
            incrementNRTTurn();
        rr.pop_front();
        packetEnded = true;
    }
    return std::make_pair(packetEnded, delayCount);
}
bool Transmitter::iterateRTs() {
    if (_nrtNumber && _nrtNumber < _nrtStock)
        return true;
    while (_rrs[_rtTurn].rt.empty()) {
        incrementRTTurn();
        if (_rtTurn == 0)
            return true;
    }
    return false;
}
bool Transmitter::iterateNRTs() {
    int count = 0;
    while (_rrs[_nrtTurn].nrt.empty()) {
        incrementNRTTurn();
        ++count;
        /*if (_nrtTurn == 0) {
            _nrtNumber = 0;
            return true;
        }*/
        if (count == _nodeCount) {
            _nrtNumber = 0;
            return true;
        }
    }
    ++_nrtNumber;
    if (_nrtNumber == _nrtStock)
        _nrtNumber = 0;
    return false;
}
bool Transmitter::updateRR(int clock) {
    bool turnFinished = false;
    FlowType turn;
    if (!rrsEmpty()) {
        if (iterateRTs()) {
            if (iterateNRTs()) {
                if (iterateRTs()) {
                    turn = FlowType::NRT;
                    iterateNRTs();
                }
                else
                    turn = FlowType::RT;
            }
            else
                turn = FlowType::NRT;
        }
        else
            turn = FlowType::RT;
    }
    else
        return false;
    int clockCount = clock - _clock;
    while (clockCount > 0) {
        // First we should check non-complete packets and if all were complete
        // we should check them based on the priority.
        if (!_rrs[_rtTurn].rt.empty() &&
            !_rrs[_rtTurn].rt.front()->isCompletePacket()) {
            auto pair = updateFrontPacket(_rrs[_rtTurn].rt, clockCount,
                                          FlowType::RT);
            clockCount -= pair.second;
            if (pair.first) {
                turnFinished = true;
                break;
            }
        }
        else if (!_rrs[_nrtTurn].nrt.empty() &&
                !_rrs[_nrtTurn].nrt.front()->isCompletePacket()) {
            auto pair = updateFrontPacket(_rrs[_nrtTurn].nrt, clockCount,
                                          FlowType::NRT);
            clockCount -= pair.second;
            if (pair.first) {
                turnFinished = true;
                break;
            }
        }
        else if (turn == FlowType::RT) {
            assert(!_rrs[_rtTurn].rt.empty());
            auto pair = updateFrontPacket(_rrs[_rtTurn].rt, clockCount,
                                          FlowType::RT);
            clockCount -= pair.second;
            if (pair.first) {
                turnFinished = true;
                break;
            }
        }
        else {
            assert(!_rrs[_nrtTurn].nrt.empty());
            auto pair = updateFrontPacket(_rrs[_nrtTurn].nrt, clockCount,
                                          FlowType::NRT);
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
    if (_clock)
        while (updateRR(clock));
    _clock = clock;
}

void Transmitter::incrementFlowsDelay(RR &rr, int count) {
    for (auto next : rr.rt)
        next->incrementDelay(count);
    for (auto next : rr.nrt)
        next->incrementDelay(count);
}
void Transmitter::incrementFlowsDelay(int count) {
    for (auto &rr : _rrs)
        incrementFlowsDelay(rr, count);
}

void Transmitter::clear() {
    for (auto &rr : _rrs) {
        rr.rt.clear();
        rr.nrt.clear();
    }
    _clock = _rtTurn = _nrtTurn = 0;
}
