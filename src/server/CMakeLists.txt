# Include source files
aux_source_directory(. SRC_LIST)
aux_source_directory(./database DB_LIST)
aux_source_directory(./model MD_LIST)
aux_source_directory(./redis REDIS_LIST)

# Generate executable file
add_executable(gochat-server ${SRC_LIST} ${DB_LIST} ${MD_LIST} ${REDIS_LIST})

# Link libraries
target_link_libraries(gochat-server muduo_net muduo_base mysqlclient hiredis pthread)