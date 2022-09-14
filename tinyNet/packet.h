#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include "json.hpp"
#include "packetDirection.h"

class packet {
protected:
    long timeSend;
    int id;
    nlohmann::json data;
    packetDirection direction;
    bool processed;
public:
    //getters;
    long& getTimeSend();
    int getID();
    nlohmann::json& getData();
    packetDirection& getDirection();
    bool isProcessed();

    //setters
    void setTimeSend(const long& timeSend);
    void setID(int id);
    void setDirection(packetDirection &direct);

    virtual void serialize();
    virtual void deserialize(std::string& data) = 0;


    virtual ~packet() {};
};


#endif //PACKET_H
