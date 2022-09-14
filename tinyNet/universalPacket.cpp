#include "universalPacket.h"

universalPacket::universalPacket() {
    direction = packetDirection::UNIVERSAL;
    answer = false;
}

void universalPacket::serialize(){
    this->data["answer"] = answer;
    packet::serialize();
}

bool universalPacket::isAnswer() {
    return answer;
}

void universalPacket::setIsAnswer(bool ans) {
    answer = ans;
}
