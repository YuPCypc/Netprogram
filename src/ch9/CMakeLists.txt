add_executable(streamserver streamserver.c)
target_link_libraries(streamserver Netprogram)

add_executable(streamclient streamclient.c)
target_link_libraries(streamclient Netprogram)