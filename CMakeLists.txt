add_subdirectory(Test)
add_subdirectory(tests)

set(irstd_sources
	Exception/Exception.cpp
	Logger/Logger.cpp
	Memory/Memory.cpp
	Bootstrap/Bootstrap.cpp
)

add_library(irstd STATIC ${irstd_sources})
