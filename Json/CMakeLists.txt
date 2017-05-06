include_directories("rapidjson/include")

# Disable this as there is nothgin to build with rapidjson
#add_subdirectory(rapidjson)

# Copy the rapidjson include directory
file(COPY rapidjson/include/rapidjson
	DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/..)
