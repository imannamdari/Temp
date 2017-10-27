//
// Created by iman on 10/27/17.
//

#ifndef __TRANSMITTER_H__
#define __TRANSMITTER_H__

#include "Structs.h"

#include <deque>

class Transmitter {
public:
    explicit Transmitter(int maxSize);

    void sendFlow(Flow *flow);
    bool ifShouldSendByWire(Flow *flow) const;

    void finalUpdate();

private:
    std::deque<Flow *> _rtRR, _nrtRR, _rtBuffer;
    int _clock; ///< Store the prev clock.
    int _maxSize; ///< Max size of the rt and nrt round robins.

    int updateFrontPacket(std::deque<Flow *> &rr, int clockCount);
    void update(int clock);

    void incrementFlowsDelay(int count);
};

inline bool Transmitter::ifShouldSendByWire(Flow *flow) const {
    return flow->getType() == FlowType::NRT && _nrtRR.size() == _maxSize;
}


#endif //__TRANSMITTER_H__
