#ifndef TINYNETWORKING_UNIVERSALPACKET_H
#define TINYNETWORKING_UNIVERSALPACKET_H

#include "iostream"
#include "packet.h"

class universalPacket : public packet {
private:
    bool answer;
public:
    universalPacket();
    void serialize() override;
    bool isAnswer();
    void setIsAnswer(bool ans);
};


#endif //TINYNETWORKING_UNIVERSALPACKET_H
