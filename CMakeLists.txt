cmake_minimum_required(VERSION 3.21)
project(tinyNetworking)

set(CMAKE_CXX_STANDARD 23)

add_library(tinyNetworking SHARED packet.cpp TcpClient.cpp TcpServer.cpp TcpServerClient.cpp serverDispatcher.cpp dispatcher.cpp clientDispatcher.cpp)

set_target_properties(tinyNetworking PROPERTIES PUBLIC_HEADER "packet.h;AbstractPacketFactory.h;packetDirection.h;serverDispatcher.h;clientDispatcher.h;dispatcher.h;TcpClient.h;TcpServer.h;general.h;")

install(TARGETS tinyNetworking
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})