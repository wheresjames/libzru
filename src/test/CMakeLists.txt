
#====================================================================

# Output
set(BINARY ${PROJECT_NAME}-test)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true "cpp/*.cpp")

set(SOURCES ${SOURCES})

add_executable(${BINARY} ${SOURCES})

target_link_libraries(${BINARY} PRIVATE ${PROJECT_NAME})

target_link_libraries(${BINARY} PRIVATE "-lpthread -lrt")

enable_testing()
add_test(NAME ${BINARY} COMMAND ${BINARY})
add_custom_command(TARGET ${BINARY} POST_BUILD
                   COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --build-target  ${BINARY})
