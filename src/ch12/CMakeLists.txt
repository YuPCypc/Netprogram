add_executable(nioclient nioclient.c)
target_link_libraries(nioclient Netprogram)

add_executable(nioserver nioserver.c)
target_link_libraries(nioserver Netprogram)