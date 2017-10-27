//
// Created by iman on 10/27/17.
//

#ifndef __CONTAINER_H__
#define __CONTAINER_H__


#include "Structs.h"

#include <vector>
#include <string>

class Container {
public:
    explicit Container(int size);

    void readFlows(const std::string &address);
    /// In case when rt and nrt type is not specified and should select randomly.
    void readFlows(const std::string &address, int percent);
    void sortFlows();

    int getFlowsCount() const;
    const std::vector<Flow *> &getFlows() const;

private:
    struct Compare {
        bool operator() (const Flow *flow0,
                         const Flow *flow1) {
            return flow0->getSendCycle() < flow1->getSendCycle();
        }
    };
    int _size; ///< Size of mesh.
    std::vector<Node *> _mesh;
    std::vector<Flow *> _flows;
};

inline int Container::getFlowsCount() const {
    return static_cast<int>(_flows.size());
}
inline const std::vector<Flow *> &Container::getFlows() const {
    return _flows;
}


#endif //__CONTAINER_H__
