//
// Created by iman on 10/27/17.
//

#ifndef __STRUCTS_H__
#define __STRUCTS_H__


#include "Setting.h"

class Node {
public:
    explicit Node(int number);

    int getNumber() const;

private:
    /// The number of the node from 0 to (n * n - 1).
    int _number;
};
inline Node::Node(int number) : _number(number) {
}
inline int Node::getNumber() const {
    return _number;
}

enum class FlowType {
    RT, NRT
};
class Flow {
public:
    Flow(Node *start, Node *end, int sendTime, FlowType type);

    const Node *getStart() const;
    const Node *getEnd() const;
    int getSendCycle() const;
    FlowType getType() const;
    int getNeededCycles() const;
    int getDelay() const;

    void incrementDelay(int count);
    void decrementNeededCycles(int count);

    /// If all it's flits remaining.
    bool isCompletePacket() const;

private:
    Node *_start, *_end;
    int _sendCycle; /// Time that flow should send.
    FlowType _type;
    /// Delay of the flow that commit through transmission.
    int _delay;
    /// Number of cycles that this flow needs to send it's flits.
    int _neededCycles;
};
inline Flow::Flow(Node *start, Node *end, int sendTime, FlowType type) :
        _start(start), _end(end), _sendCycle(sendTime), _type(type),
        _delay(0), _neededCycles(flitCount * flitDuration) {
}
inline const Node* Flow::getStart() const {
    return _start;
}
inline const Node* Flow::getEnd() const {
    return _end;
}
inline int Flow::getSendCycle() const {
    return _sendCycle;
}
inline FlowType Flow::getType() const {
    return _type;
}
inline int Flow::getNeededCycles() const {
    return _neededCycles;
}
inline int Flow::getDelay() const {
    return _delay;
}

inline void Flow::incrementDelay(int count) {
    _delay += count;
}
inline void Flow::decrementNeededCycles(int count) {
    _neededCycles -= count;
}

inline bool Flow::isCompletePacket() const {
    return _neededCycles == flitCount * flitDuration;
}

#endif //__STRUCTS_H__
