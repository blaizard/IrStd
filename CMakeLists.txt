add_subdirectory(test)

set(irstd_sources
	Exception/Exception.cpp
	Logger/Logger.cpp
	Memory/Memory.cpp
)

add_library(irstd STATIC ${irstd_sources})
