
#====================================================================

# Output
set(BINARY ${PROJECT_NAME}-memmon)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true "cpp/*.cpp")

set(SOURCES ${SOURCES})

add_executable(${BINARY} ${SOURCES})

target_link_libraries(${BINARY} PRIVATE ${PROJECT_NAME})
target_link_libraries(${BINARY} PRIVATE "-lpthread -lrt")


