# Include source files
aux_source_directory(. SRC_LIST)

# Generate executable file
add_executable(gochat-client ${SRC_LIST})

# Link libraries
target_link_libraries(gochat-client pthread)