add_executable(threadserver threadserver.c loop.c)
target_link_libraries(threadserver Netprogram)

add_executable(threadpoolserver threadpoolserver.c loop.c)
target_link_libraries(threadpoolserver Netprogram)