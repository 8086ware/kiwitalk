CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(kiwitalk)

set(KIWITALK_CLIENT_SOURCE src/client/main.c src/client/tab/tab.c src/client/tab/process.c src/client/tab/command.c src/client/tab/getinput.c src/client/tab/receive.c src/parse_command.c)
set(KIWITALK_SERVER_SOURCE src/server/main.c src/parse_command.c src/server/config.c)

add_executable(kiwitalk ${KIWITALK_CLIENT_SOURCE})
add_executable(kiwitalk_server ${KIWITALK_SERVER_SOURCE})

target_include_directories(kiwitalk PRIVATE include)
target_include_directories(kiwitalk_server PRIVATE include)

target_link_libraries(kiwitalk PRIVATE termmanip)
target_link_libraries(kiwitalk_server PRIVATE termmanip)

if(WIN32) 
	target_link_libraries(kiwitalk PRIVATE ws2_32)
	target_link_libraries(kiwitalk_server PRIVATE ws2_32)
endif()
