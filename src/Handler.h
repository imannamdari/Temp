//
// Created by iman on 10/27/17.
//

#ifndef __HANDLER_H__
#define __HANDLER_H__


#include "Container.h"
#include "Transmitter.h"

class Handler {
public:
    /**
     * @param readAddress The address of flows.
     * @param resAddress The address that wired flows should write to.
     * @param size The mesh size.
     * @param percent The percentage of flows that are rt.
     * @warning percent must be between 0 and 100.
     */
    Handler(const std::string &readAddress, const std::string &resAddress,
            int size, int percent);

    void handle();
    ~Handler();

private:
    Container *_container;
    Transmitter *_transmitter;

    std::string _resAddress;
    int _size;

    void writeFlowToFile(Flow *flow, int index);
    void writeDelays();

    void clear();
};


#endif //__HANDLER_H__
