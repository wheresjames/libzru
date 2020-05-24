
#====================================================================

# Output
set(BINARY ${PROJECT_NAME})

# Source files
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true "cpp/*.cpp")

set(SOURCES ${SOURCES})

add_library(${BINARY} SHARED ${SOURCES})

# Link options
target_link_libraries(${BINARY} ${CONAN_LIBS})

# No prefix
set_target_properties(${BINARY} PROPERTIES PREFIX "")

