//
// Created by iman on 10/27/17.
//

#ifndef __STRUCTS_H__
#define __STRUCTS_H__


#include <cmath>
#include "Setting.h"

class Node {
public:
    explicit Node(int x, int y, int number);

    int getNumber() const;
    int getX() const;
    int getY() const;

private:
    /// The number of the node from 0 to (n * n - 1).
    int _number;
    int _x, _y;
};
inline Node::Node(int x, int y, int number) :
        _x(x), _y(y), _number(number) {
}
inline int Node::getNumber() const {
    return _number;
}
inline int Node::getX() const {
    return _x;
}
inline int Node::getY() const {
    return _y;
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

    void setPassedByWire(bool byWire);
    bool getPassedByWire() const;

    int getLength() const;

    void clear();

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

    bool _passedByWire;
};
inline Flow::Flow(Node *start, Node *end, int sendTime, FlowType type) :
        _start(start), _end(end), _sendCycle(sendTime), _type(type),
        _delay(0), _neededCycles(flitCount * flitDuration), _passedByWire(false) {
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

inline void Flow::setPassedByWire(bool byWire) {
    _passedByWire = byWire;
}
inline bool Flow::getPassedByWire() const {
    return _passedByWire;
}

inline int Flow::getLength() const {
    return static_cast<int>(std::abs(_start->getX() - _end->getX()) +
                            std::abs(_start->getY() - _end->getY()));
}

inline void Flow::clear() {
    _delay = 0;
    _neededCycles = flitCount * flitDuration;
    _passedByWire = false;
}

#endif //__STRUCTS_H__
