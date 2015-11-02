set(PLIB_SOURCES
        interface.cpp
        libConfig.cpp
)

set(PLIB_HEADERS
        interface.hpp
        libConfig.hpp
)

add_library(lib${CMAKE_PROJECT_NAME} SHARED ${PLIB_SOURCES} ${PLIB_HEADERS})
