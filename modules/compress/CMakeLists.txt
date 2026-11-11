add_executable(compress main.cpp prefixTree.cpp encoder.cpp decoder.cpp)

set_target_properties(compress PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/modules")