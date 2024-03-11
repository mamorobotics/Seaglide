add_executable (Hoverfish "Hoverfish.cpp")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Hoverfish PROPERTY CXX_STANDARD 20)
endif()

set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

#RF24
find_package(RF24 REQUIRED)

# Include the rf24 headers
include_directories(${RF24_INCLUDE_DIRS})

# Add your executable and link against rf24
target_link_libraries(Hoverfish RF24)