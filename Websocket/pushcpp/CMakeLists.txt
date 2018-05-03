# Sources
set(irstdwebstocket_sources
	easywsclient.cpp
	pushcpp.cpp
	pushcpp_clientevents.cpp
	pushcpp_eventloop.cpp
	pushcpp_events.cpp
	pushcpp_subscriptions.cpp
)

# Build the gtest static library
add_library(irstdwebsocket ${irstdwebstocket_sources})
target_link_libraries(irstdwebsocket irstd pthread)
