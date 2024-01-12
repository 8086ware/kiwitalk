CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(kiwitalk)

set(KIWITALK_CLIENT_SOURCE src/client/main.c src/client/tab/tab.c src/client/tab/process.c src/client/tab/command.c src/client/tab/getinput.c src/client/tab/receive.c src/parse_command.c)
set(KIWITALK_SERVER_SOURCE src/server/main.c src/parse_command.c src/server/config.c)

add_executable(kiwitalk ${KIWITALK_CLIENT_SOURCE})
add_executable(kiwitalk_server ${KIWITALK_SERVER_SOURCE})

target_include_directories(kiwitalk PRIVATE include)
target_include_directories(kiwitalk_server PRIVATE include)

if(WIN32) 
	target_link_libraries(kiwitalk PRIVATE ws2_32)
	target_link_libraries(kiwitalk_server PRIVATE ws2_32)

	target_link_directories(kiwitalk PUBLIC "C:/Program Files (x86)/termmanip/lib")
	target_include_directories(kiwitalk PUBLIC "C:/Program Files (x86)/termmanip/include")

	target_link_directories(kiwitalk_server PUBLIC "C:/Program Files (x86)/termmanip/lib")
	target_include_directories(kiwitalk_server PUBLIC "C:/Program Files (x86)/termmanip/include")
endif()

target_link_libraries(kiwitalk PRIVATE termmanip)
target_link_libraries(kiwitalk_server PRIVATE termmanip)

if(CMAKE_BUILD_TYPE MATCHES Debug)
	target_compile_options(kiwitalk PRIVATE -Wall -fsanitize=address)
	target_link_options(kiwitalk PRIVATE -Wall -fsanitize=address)
	target_compile_options(kiwitalk_server PRIVATE -fsanitize=address)
	target_link_options(kiwitalk_server PRIVATE -Wall -fsanitize=address)
endif()
