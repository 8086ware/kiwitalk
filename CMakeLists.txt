CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(kiwitalk)

set(KIWITALK_CLIENT_SOURCE src/client/main.c)
set(KIWITALK_SERVER_SOURCE src/server/main.c)

add_executable(kiwitalk ${KIWITALK_CLIENT_SOURCE})
add_executable(kiwitalk_server ${KIWITALK_SERVER_SOURCE})

target_include_directories(kiwitalk PRIVATE include)
target_include_directories(kiwitalk_server PRIVATE include)

target_link_libraries(kiwitalk PRIVATE termmanip)
target_link_libraries(kiwitalk PRIVATE sockmanip)

target_link_libraries(kiwitalk_server PRIVATE termmanip)
target_link_libraries(kiwitalk_server PRIVATE sockmanip)

install(TARGETS sockmanip DESTINATION lib)
install(FILES include/sockmanip.h DESTINATION include)

if(WIN32) 
	target_link_libraries(kiwitalk PRIVATE ws2_32)
	target_link_libraries(kiwitalk_server PRIVATE ws2_32)
endif()
