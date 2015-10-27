set(PLIB_SOURCES
        interface.cpp
)

set(PLIB_HEADERS
        interface.hpp
)

add_library(lib${CMAKE_PROJECT_NAME} SHARED ${PLIB_SOURCES} ${PLIB_HEADERS})
