set(mineziper_STATIC_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(mineziper_STATIC_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# mineziper sources
set(mineziper_SRC
	${mineziper_STATIC_SRC}/mineziper.c
)

set(mineziper_INCLUDE
)

add_executable(mineziper ${mineziper_SRC} ${mineziper_INCLUDE})
target_include_directories(mineziper PUBLIC ${mineziper_STATIC_INCLUDE})
target_link_libraries(mineziper PUBLIC libmineziper)
