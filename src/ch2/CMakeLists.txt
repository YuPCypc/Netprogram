add_executable(tcpclient tcpclient.c)
target_link_libraries(tcpclient Netprogram)
add_executable(tcpserver tcpserver.c)
target_link_libraries(tcpserver Netprogram)