find_package(OpenSSL)

# Sources
set(irstdcrypto_sources
	Crypto.cpp
)

# Build the gtest static library
add_library(irstdcrypto ${irstdcrypto_sources})
target_link_libraries(irstdcrypto ssl crypto)
