//
// Created by iman on 10/27/17.
//

#ifndef __TRANSMITTER_H__
#define __TRANSMITTER_H__

#include "Structs.h"

#include <deque>
#include <vector>

struct RR {
    std::deque<Flow *> rt, nrt, buffer;
};

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

    int _prevTurn, _turn;

    int updateFrontPacket(std::deque<Flow *> &rr, int clockCount);
    void updateRR(RR &rr, int clock);
    void update(int clock);

    void incrementFlowsDelay(int count);
    void incrementFlowsDelay(RR &rr, int count);
};

inline bool Transmitter::ifShouldSendByWire(Flow *flow) const {
    return flow->getType() == FlowType::NRT &&
           _rrs[flow->getStart()->getNumber()].nrt.size() == _maxSize;
}


#endif //__TRANSMITTER_H__
