add_executable(graceserver graceserver.c)
target_link_libraries(graceserver Netprogram)

add_executable(graceclient graceclient.c)
target_link_libraries(graceclient Netprogram)