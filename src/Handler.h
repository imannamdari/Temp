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
            const std::string &flowDir, const std::string &flowFile,
            int size, int percent, int nrtStock);

    std::pair<double, double> getDelay() const;
    void clear();

    void sort();
    void handle();
    void handleI(int i);
    ~Handler();

private:
    Container *_container;
    Transmitter *_transmitter;

    std::string _flowDir, _flowFile, _resAddress;
    int _size;

    void writeFlowToFile(const std::string &fileName, Flow *flow);
    void writeDelaysToFile(const std::string &fileName);
    void writeNRTCountToFile(const std::string &fileName,
                             int wireCount, int wirelessCount);
    void writeRTCountToFile(const std::string &fileName, int count);
};


#endif //__HANDLER_H__
