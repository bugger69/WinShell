add_executable(grep main.cpp)

set_target_properties(grep PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/modules")