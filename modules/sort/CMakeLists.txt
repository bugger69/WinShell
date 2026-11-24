add_executable(sort minHeap.cpp main.cpp)

set_target_properties(sort PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/modules")