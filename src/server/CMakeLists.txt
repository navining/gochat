# Include source files
aux_source_directory(. SRC_LIST)

# Generate executable file
add_executable(gochat-server ${SRC_LIST})

# Link libraries
target_link_libraries(gochat-server muduo_net muduo_base pthread)