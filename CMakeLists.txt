# Minimal version for CMake
cmake_minimum_required(VERSION 3.1)

# Project's name
project(irstd)

# Set c++11 standard
set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

# Check the compiler
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(IRSTD_COMPILER "CLANG")
	set(IRSTD_COMPILER_CLANG 1)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(IRSTD_COMPILER "GNU")
	set(IRSTD_COMPILER_GNU 1)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	set(IRSTD_COMPILER "INTEL")
	set(IRSTD_COMPILER_INTEL 1)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(IRSTD_COMPILER "MSVC")
	set(IRSTD_COMPILER_MSVC 1)
else()
	message(FATAL_ERROR "Unsupported compiler")
endif()

# Display information about the build
message(STATUS "Compiler: ${IRSTD_COMPILER}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Add custom target to force recompiling the compiler.cpp file in order
# to generate a new version for the build each time
message(STATUS "Touching file 'Compiler/Compiler.cpp' to force regeneration of version number")
add_custom_target(invalidate_files ALL
		COMMAND touch ${CMAKE_CURRENT_LIST_DIR}/Compiler/Compiler.cpp)

# Do not use specific compile options
add_subdirectory(Websocket/pushcpp)

if (IRSTD_COMPILER_GNU)
	add_compile_options(
		-Wall
		-Wextra
		-Wfloat-equal
		-Wshadow
		-Wpointer-arith
		-Wcast-align
		-Wstrict-overflow=2
		-Wwrite-strings
		-Wcast-qual
		-Wswitch-default
		-Wswitch-enum
		-Wconversion
		-Wunreachable-code
		-Werror)
	set(CMAKE_CXX_FLAGS_DEBUG "-DIRSTD_DEBUG -O0 -rdynamic -g")
	set(CMAKE_C_FLAGS_DEBUG "-DIRSTD_DEBUG -O0 -rdynamic -g")
	set(CMAKE_CXX_FLAGS_RELEASE "-DIRSTD_RELEASE -O3")
	set(CMAKE_C_FLAGS_RELEASE "-DIRSTD_RELEASE -O3")
endif()

add_subdirectory(Crypto)
add_subdirectory(Fetch)
add_subdirectory(Json)
add_subdirectory(Test)
add_subdirectory(tests)

set(irstd_sources
	Compiler/Compiler.cpp
	Event/Event.cpp
	Exception/Exception.cpp
	Exception/ExceptionPtr.cpp
	FileSystem/FileSystem.cpp
	FileSystem/FileStream.cpp
	FileSystem/FileCsv.cpp
	Logger/Logger.cpp
	Main/Main.cpp
	Memory/Memory.cpp
	Rand/Rand.cpp
	Bootstrap/Bootstrap.cpp
	Thread/Thread.cpp
	Thread/Threads.cpp
	Topic/Topic.cpp
	Type/Type.cpp
	Type/Timestamp.cpp
	Type/Memory.cpp
	Type/ShortString.cpp
	Type/Gson.cpp
	Type/Decimal.cpp
	Type/Numeric.cpp
	Type/Buffer.cpp
	Type/Stopwatch.cpp
	Server/Server.cpp
	Server/ServerHTTP.cpp
	Server/ServerREST.cpp
	Server/MimeType.cpp
	Json/Json.cpp
	Utils/Utils.cpp
	Websocket/Pusher.cpp
)

add_library(irstd ${irstd_sources})
target_link_libraries(irstd irstdfetch irstdwebsocket irstdcrypto)
