# GTest
if (IRSTD_COMPILER_GNU)
	# Disable some diagnostic
	add_compile_options(-Wno-switch-default)
	add_compile_options(-Wno-switch-enum)
endif()

add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# Sources
set(gtest_sources
	Test.cpp
	TestMain.cpp
)

# Copy the gtest include directory
file(COPY ${gtest_SOURCE_DIR}/include/gtest
	DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Build the gtest static library
add_library(irstdgtest ${gtest_sources})
target_link_libraries(irstdgtest gtest)
