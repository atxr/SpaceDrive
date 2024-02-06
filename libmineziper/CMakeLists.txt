set(libmineziper_STATIC_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(libmineziper_STATIC_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# mineziper sources
set(libmineziper_SRC
	${libmineziper_STATIC_SRC}/libmineziper.c
	${libmineziper_STATIC_SRC}/libmineziper_zip.c
)

set(libmineziper_INCLUDE
	${libmineziper_STATIC_INCLUDE}/libmineziper.h
	${libmineziper_STATIC_INCLUDE}/libmineziper_zip.h
)

# Create static library
add_library(libmineziper STATIC ${libmineziper_SRC} ${libmineziper_INCLUDE})

# Include directories management
target_include_directories(
	libmineziper 
	PUBLIC $<BUILD_INTERFACE:${libmineziper_STATIC_INCLUDE}> $<INSTALL_INTERFACE:include>
)

install(TARGETS libmineziper EXPORT mineziper LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
install(DIRECTORY ${libmineziper_SRC_PATH}/include DESTINATION include FILES_MATCHING PATTERN "*.h*")

include(GNUInstallDirs)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mineziper.pc DESTINATION lib/pkgconfig)
install(EXPORT mineziper DESTINATION cmake)