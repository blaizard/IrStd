# Build the test executable
set(test_sources
	TestMacro.cpp
	TestLogger.cpp
	TestMemory.cpp
)

include_directories(..)

add_executable(tests ${test_sources})
target_link_libraries(tests irstdgtest irstd pthread)

enable_testing()
add_test(tests tests)
