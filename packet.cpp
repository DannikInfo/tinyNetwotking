#include "packet.h"

long& packet::getTimeSend() {
    return this->timeSend;
}

int packet::getID() {
    return this->id;
}

nlohmann::json& packet::getData() {
    return this->data;
}

packetDirection& packet::getDirection() {
    return direction;
}

void packet::setTimeSend(const long &timeSend) {
    this->timeSend = timeSend;
}

void packet::setID(int id) {
    this->id = id;
}

void packet::setDirection(packetDirection &direct) {
    this->direction = direct;
}
