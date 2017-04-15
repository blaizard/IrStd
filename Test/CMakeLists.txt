# GTest
add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(IRSTD_INCLUDE "${IRSTD_INCLUDE} ${gtest_SOURCE_DIR}/include")

# Sources
set(gtest_sources
	Test.cpp
	TestMain.cpp
)

# Build the gtest static library
add_library(irstdgtest STATIC ${gtest_sources})
target_link_libraries(irstdgtest gtest)
