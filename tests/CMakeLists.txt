# Build the test executable
set(test_sources
	TestMacro.cpp
	TestLogger.cpp
)
add_executable(tests ${test_sources})
target_link_libraries(tests irstdgtest irstd)

#add_test(runUnitTests runUnitTests)
