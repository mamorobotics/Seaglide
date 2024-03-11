add_executable (Float "Float.cpp")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Float PROPERTY CXX_STANDARD 20)
endif()

set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
message( ${LIB_DIR} )
