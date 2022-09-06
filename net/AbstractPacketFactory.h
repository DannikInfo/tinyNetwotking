#ifndef ABSTRACTPACKETFACTORY_H
#define ABSTRACTPACKETFACTORY_H

#include <iostream>
#include "packet.h"

class AbstractPacketFactory {
public:
    virtual std::unique_ptr<packet> create(int packetID, nlohmann::json *data = NULL) = 0;
};

#endif //ABSTRACTPACKETFACTORY_H
