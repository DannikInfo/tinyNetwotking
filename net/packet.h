#ifndef PACKET_H
#define PACKET_H

#include <iostream>
#include "json.hpp"

class packet {
protected:
    std::string from;
    std::string to;
    long timeSend;
    int id;
    nlohmann::json data;
public:
    //getters
    std::string& getFrom();
    std::string& getTo();
    long& getTimeSend();
    int getID();
    nlohmann::json& getData();

    //setters
    void setFrom(const std::string& from);
    void setTo(const std::string& to);
    void setTimeSend(const long& timeSend);
    void setID(int id);

    virtual void deserialize(std::string& data) =0;
    virtual void serialize() =0;

    virtual ~packet() {};
};


#endif //PACKET_H
