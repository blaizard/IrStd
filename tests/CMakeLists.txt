# Build the test executable
set(test_sources
	TestCompiler.cpp
	TestEvent.cpp
	TestFileCsv.cpp
	TestFlag.cpp
	TestMacro.cpp
	TestLogger.cpp
	TestMemory.cpp
	TestJson.cpp
	TestException.cpp
	TestMain.cpp
	TestRWLock.cpp
	TestServer.cpp
	TestStreambuf.cpp
	TestThread.cpp
	TestTopic.cpp
	TestType.cpp
	TestTypeRingBuffer.cpp
	TestTypeStreamDB.cpp
)

include_directories(..)

add_executable(tests ${test_sources})
target_link_libraries(tests irstdgtest irstd pthread)

enable_testing()
add_test(tests tests)
