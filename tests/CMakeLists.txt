# Build the test executable
set(test_sources
	TestEvent.cpp
	TestFlag.cpp
	TestMacro.cpp
	TestLogger.cpp
	TestMemory.cpp
	TestJson.cpp
	TestException.cpp
	TestServer.cpp
	TestStreambuf.cpp
	TestThread.cpp
	TestType.cpp
)

include_directories(..)

add_executable(tests ${test_sources})
target_link_libraries(tests irstdgtest irstd pthread)

enable_testing()
add_test(tests tests)
