cmake_minimum_required(VERSION 3.21)
project(tinyNetworking)

set(CMAKE_CXX_STANDARD 23)

add_library(tinyNetworking SHARED
        tinyNet/packet.h
        tinyNet/abstractPacketFactory.h
        tinyNet/packetDirection.h
        tinyNet/serverDispatcher.h
        tinyNet/clientDispatcher.h
        tinyNet/dispatcher.h
        tinyNet/tcp/client.h
        tinyNet/tcp/server.h
        tinyNet/tcp/general.h
        tinyNet/packet.cpp
        tinyNet/tcp/client.cpp
        tinyNet/tcp/server.cpp
        tinyNet/tcp/serverClient.cpp
        tinyNet/serverDispatcher.cpp
        tinyNet/dispatcher.cpp
        tinyNet/clientDispatcher.cpp
        tinyNet/universalPacket.cpp
        tinyNet/universalPacket.h
        tinyNet/abstractNetworkHandler.h tinyNet/tcp/baseClient.h tinyNet/tcp/socketType.h tinyNet/tcp/threadPool.h tinyNet/tcp/socketStatus.h tinyNet/tcp/serverClient.h tinyNet/tcp/threadManageType.h tinyNet/tcp/serverStatus.h)


install(TARGETS tinyNetworking
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(DIRECTORY "${CMAKE_SOURCE_DIR}/" # source directory
        DESTINATION "include" # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.h" # select header files
        PATTERN ".git" EXCLUDE
        PATTERN "build" EXCLUDE
        PATTERN "cmake-build-debug" EXCLUDE
        PATTERN ".idea" EXCLUDE
        )