# mineziper(d) sources
add_executable(mineziper ${CMAKE_CURRENT_SOURCE_DIR}/mineziper.c)
target_link_libraries(mineziper PUBLIC libmineziper)

add_executable(mineziperd ${CMAKE_CURRENT_SOURCE_DIR}/mineziperd.c)
target_link_libraries(mineziperd PUBLIC libmineziper)
