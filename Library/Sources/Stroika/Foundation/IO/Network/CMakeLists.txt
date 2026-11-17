target_sources (stroika-foundation PRIVATE
    CIDR.cpp
    ConnectionlessSocket.cpp
    ConnectionOrientedMasterSocket.cpp
    ConnectionOrientedStreamSocket.cpp
    DNS.cpp
    Interface.cpp
    InternetAddress.cpp
    LinkMonitor.cpp
    Listener.cpp
    Neighbors.cpp
    Socket.cpp
    SocketAddress.cpp
    SocketStream.cpp
    SystemFirewall.cpp
    UniformResourceIdentification.cpp
    URI.cpp
)

add_subdirectory (HTTP)
add_subdirectory (InternetProtocol)
add_subdirectory (Platform)
add_subdirectory (SOAP)
add_subdirectory (Transfer)
