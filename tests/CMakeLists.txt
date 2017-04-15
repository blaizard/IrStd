# Build the test executable
set(test_sources
	TestMacro.cpp
	TestLogger.cpp
)

include_directories(${IRSTD_INCLUDE})

add_executable(tests ${test_sources})
target_link_libraries(tests irstdgtest irstd pthread)

enable_testing()
add_test(tests tests)
