# Find curl & openssl
find_package(CURL)
if (CURL_FOUND)
	include_directories(${CURL_INCLUDE_DIR})
else()
	message(FATAL_ERROR "Could not find CURL library")
endif(CURL_FOUND)
find_package(OpenSSL)

# Sources
set(irstdfetch_sources
	Fetch.cpp
	FetchCurl.cpp
)

# Build the gtest static library
add_library(irstdfetch ${irstdfetch_sources})
target_link_libraries(irstdfetch ssl curl)
