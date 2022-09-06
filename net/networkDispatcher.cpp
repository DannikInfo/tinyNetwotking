#include "networkDispatcher.h"
#include "AbstractPacketFactory.h"

bool error(int n, const std::string& msg){
    if (n < 0){
        logger::error(msg);
        return true;
    }
    return false;
}

void networkDispatcher::setPort(int portG) {
    port = portG;
}
void networkDispatcher::setHost(std::string &hostnameG){
    hostname = hostnameG;
}

void networkDispatcher::setFactory(AbstractPacketFactory *factoryG) {
    factory = factoryG;
}

void networkDispatcher::startListen(){
    int sockfd, newsockfd;
    while(true){
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                logger::error("ERROR opening socket");

        bzero((char *) &serv_addr, sizeof(serv_addr));

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(port);

        if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
            logger::error("ERROR on binding");

        listen(sockfd, 5);

        clilen = sizeof(cli_addr);

        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            logger::error("ERROR on accept");

        logger::info("server: got connection from "+std::string(inet_ntoa(cli_addr.sin_addr))+" port "+std::to_string(ntohs(cli_addr.sin_port)));
        std::thread clientT(clientThread::run);
        std::mutex mtx;
        mtx.lock();
            clientThread::setSockfd(sockfd);
        mtx.unlock();
    }
}

int networkDispatcher::con(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        logger::error("ERROR opening socket");

    server = gethostbyname(hostname.c_str());
    if (server == nullptr) {
        logger::error("ERROR, no such host");
        return -1;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        logger::error("ERROR connecting");

    return sockfd;
}

void networkDispatcher::closeCon(int sockfd, int newsockfd) {
    if(newsockfd != -1)
        close(newsockfd);
    close(sockfd);
}

void networkDispatcher::send(std::unique_ptr<packet>& p, std::string &to, int sockfd) {
    p->setFrom(hostname);
    p->setTo(to);
    p->setTimeSend(std::time(nullptr));
    p->serialize();
    std::string data = p->getData().dump();
    int dataPieces;
    if(data.length() > 1024)
        dataPieces = data.length()/4096+1;
    else
        dataPieces = data.length();

    std::string strDataPieces = std::to_string(dataPieces);

    //data size
    n = ::send(sockfd, std::to_string(data.length()).c_str(), std::to_string(data.length()).length(), MSG_NOSIGNAL);
    if(error(n, "ERROR writing to socket")) return;
    if(!receiveSM(SM_OK, sockfd)) return;

    //pieces count
    n = ::send(sockfd, strDataPieces.c_str(), strDataPieces.length(), MSG_NOSIGNAL);
    if(error(n, "ERROR writing to socket")) return;
    if(!receiveSM(SM_OK, sockfd)) return;

    std::string dataPiece;
    //data send
    for(int i = 0; i < dataPieces; i++) {
        dataPiece = data.substr(i*4096, (i+1)*4096);
        n = ::send(sockfd, dataPiece.c_str(), dataPiece.length(), MSG_NOSIGNAL);
        if (error(n, "ERROR writing to socket")) return;

        logger::info(dataPiece);
        if(!receiveSM(SM_OK, sockfd)) return;
    }
    sendSM(SM_END, sockfd);
}

void networkDispatcher::receive(int sockfd) {
    //data size
    char *sizeB[1024];
    n = recv(sockfd, sizeB, 1024, MSG_NOSIGNAL);
    if (error(n, "ERROR reading from socket")) return;
    int size = atoi(reinterpret_cast<const char *>(sizeB));
    sendSM(SM_OK, sockfd);

    //pieces count
    char *piecesB[1024];
    n = recv(sockfd, piecesB, 1024, MSG_NOSIGNAL);
    if (error(n, "ERROR reading from socket")) return;
    sendSM(SM_OK, sockfd);

    //data receive
    int pieces = atoi(reinterpret_cast<const char *>(piecesB));
    std::string data, received;
    char *buffer[4096];
    int readSize;

    for(int i = 0; i < pieces; i++) {
        if(i+1 == pieces)
            readSize = size - 4096*i;
        else
            readSize = 4096;

        bzero(buffer, 4096);

        do{
            n = recv(sockfd, buffer, readSize, MSG_NOSIGNAL);
            if (error(n, "ERROR reading from socket")) return;
            received = reinterpret_cast<char *const>(buffer);
            sendSM(SM_OK, sockfd);
        }while(strstr(data.c_str(), received.c_str()));

        data += received;
        sendSM(SM_OK, sockfd);
    }

    if(!receiveSM(SM_END, sockfd)) return;

    nlohmann::json j = nlohmann::json::parse(data);
    int id = j["packetName"].get<int>();
    std::unique_ptr<packet> packet = factory->create(id);
    packet->deserialize(data);
}

void networkDispatcher::sendSM(char* sm, int sockfd){
    n = ::send(sockfd, sm, strlen(sm), MSG_NOSIGNAL);
    if(error(n, "ERROR writing to socket")) return;
}

bool networkDispatcher::receiveSM(char* sm, int sockfd) {
    char *sizeB[1024];
    n = read(sockfd, sizeB, 1024);
    if (error(n, "ERROR reading from socket")) return false;
    if (!strstr(reinterpret_cast<const char *>(sizeB), sm)) {
        remove(reinterpret_cast<const char *>(sizeB));
        return true;
    }else {
        logger::error("Expected '"+std::string(sm)+"', but received otherwise");
        remove(reinterpret_cast<const char *>(sizeB));
        return false;
    }
}