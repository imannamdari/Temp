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
    std::deque<Flow *> rt, nrt, buffer;
    bool empty();
};

inline bool RR::empty() {
    return rt.empty() && nrt.empty() && buffer.empty();
}

class Transmitter {
public:
    explicit Transmitter(int nodeCount, int maxSize);

    void sendFlow(Flow *flow);
    bool ifShouldSendByWire(Flow *flow) const;

    void finalUpdate();

private:
    std::vector<RR> _rrs; ///< Vector of round robins for each node.
    int _clock; ///< Store the prev clock.
    int _maxSize; ///< Max size of the rt and nrt round robins.
    int _nodeCount; ///< Number of nodes.

    int _turn;

    void incrementTurn();

    int updateFrontPacket(std::deque<Flow *> &rr, int clockCount);
    void updateRR(RR &rr, int clock);
    void update(int clock);

    void incrementFlowsDelay(int count);
    void incrementFlowsDelay(RR &rr, int count);

    bool rrsEmpty();
};

inline bool Transmitter::ifShouldSendByWire(Flow *flow) const {
    return flow->getType() == FlowType::NRT &&
           _rrs[flow->getStart()->getNumber()].nrt.size() == _maxSize;
}
inline bool Transmitter::rrsEmpty() {
    for (auto &rr : _rrs)
        if (!rr.empty())
            return false;
    return true;
}
inline void Transmitter::incrementTurn() {
    ++_turn;
    _turn = _turn % _nodeCount;
}


#endif //__TRANSMITTER_H__
