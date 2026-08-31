add_executable(hello main.cpp)

set_target_properties(hello PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/modules")