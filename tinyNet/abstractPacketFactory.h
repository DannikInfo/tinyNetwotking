#ifndef ABSTRACTPACKETFACTORY_H
#define ABSTRACTPACKETFACTORY_H

#include <iostream>
#include "packet.h"

class abstractPacketFactory {
public:
    virtual std::unique_ptr<packet> create(int packetID, nlohmann::json &data) = 0;
};

#endif //ABSTRACTPACKETFACTORY_H
