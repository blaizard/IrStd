# GTest
add_subdirectory(googletest)

# Sources
set(gtest_sources
	Test.cpp
	TestMain.cpp
)

# Build the gtest static library
add_library(irstdgtest STATIC ${gtest_sources})
target_link_libraries(irstdgtest gtest)
