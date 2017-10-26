//
// Created by iman on 10/27/17.
//

#ifndef __STRUCTS_H__
#define __STRUCTS_H__

class Node {
public:
    explicit Node(int number);

private:
    /// The number of the node from 0 to (n * n - 1).
    int _number;
};
inline Node::Node(int number) : _number(number) {
}

enum class FlowType {
    RT, NRT
};
class Flow {
public:
    Flow(Node *start, Node *end, FlowType type);

private:
    Node *_start, *_end;
    /// Delay of the flow that commit through transmission.
    int _delay;
    FlowType _type;
};
Flow::Flow(Node *start, Node *end, FlowType type) :
        _start(start), _end(end), _delay(0), _type(type) {
}

#endif //__STRUCTS_H__
