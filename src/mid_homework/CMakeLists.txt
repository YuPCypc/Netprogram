add_executable(midserver mid_server.c)
target_link_libraries(midserver Netprogram)

add_executable(midclient mid_client.c)
target_link_libraries(midclient Netprogram)