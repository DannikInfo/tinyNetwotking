#include "packet.h"

std::string& packet::getFrom() {
    return this->from;
}

std::string& packet::getTo() {
    return this->to;
}

long& packet::getTimeSend() {
    return this->timeSend;
}

int packet::getID() {
    return this->id;
}

nlohmann::json& packet::getData() {
    return this->data;
}


void packet::setFrom(const std::string &from) {
    this->from = from;
}

void packet::setTo(const std::string &to) {
    this->to = to;
}

void packet::setTimeSend(const long &timeSend) {
    this->timeSend = timeSend;
}

void packet::setID(int id) {
    this->id = id;
}