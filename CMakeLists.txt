cmake_minimum_required(VERSION 3.21)
project(tinyNetworking)

set(CMAKE_CXX_STANDARD 23)

add_library(tinyNetworking SHARED net/clientThread.cpp net/clientThread.h net/packet.h net/packet.cpp net/networkDispatcher.h net/networkDispatcher.cpp net/AbstractPacketFactory.h)

set_target_properties(tinyNetworking PROPERTIES PUBLIC_HEADER "net/clientThread.h;net/packet.h;net/networkDispatcher.h;net/AbstractPacketFactory.h")
install(TARGETS tinyNetworking
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}net)