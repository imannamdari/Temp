//
// Created by iman on 10/27/17.
//

#ifndef __TRANSMITTER_H__
#define __TRANSMITTER_H__

#include "Structs.h"

#include <deque>
#include <vector>

class RR {
public:
    std::deque<Flow *> rt, nrt;
    bool empty();
};

inline bool RR::empty() {
    return rt.empty() && nrt.empty();
}

class Transmitter {
public:
    explicit Transmitter(int nodeCount, int nrtStock);

    void sendFlow(Flow *flow);
    void clear();

    void finalUpdate();

private:
    std::vector<RR> _rrs; ///< Vector of round robins for each node.
    int _clock; ///< Store the prev clock.
    int _nodeCount; ///< Number of nodes.

    int _nrtStock, _nrtNumber;
    int _rtTurn, _nrtTurn;

    void incrementRTTurn();
    void incrementNRTTurn();

    std::pair<bool, int> updateFrontPacket(std::deque<Flow *> &rr,
                                           int clockCount,
                                           FlowType type);
    bool iterateRTs();
    bool iterateNRTs();
    bool updateRR(int clock);
    void update(int clock);

    void incrementFlowsDelay(int count);
    void incrementFlowsDelay(RR &rr, int count);

    bool rrsEmpty();
    bool nrtsEmpty();
};

inline bool Transmitter::rrsEmpty() {
    for (auto &rr : _rrs)
        if (!rr.empty())
            return false;
    return true;
}
inline bool Transmitter::nrtsEmpty() {
    for (auto &rr : _rrs)
        if (!rr.nrt.empty())
            return false;
    return true;
}
inline void Transmitter::incrementRTTurn() {
    ++_rtTurn;
    _rtTurn = _rtTurn % _nodeCount;
}
inline void Transmitter::incrementNRTTurn() {
    ++_nrtTurn;
    _nrtTurn = _nrtTurn % _nodeCount;
}


#endif //__TRANSMITTER_H__
