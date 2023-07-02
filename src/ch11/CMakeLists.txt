add_executable(pollclient pollclient.c)
target_link_libraries(pollclient Netprogram)

add_executable(pollserver pollserver.c)
target_link_libraries(pollserver Netprogram)
