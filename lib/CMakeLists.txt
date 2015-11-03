set(PLIB_SOURCES
        interface.cpp
        libConfig.cpp
        database.cpp
        utils.cpp
)

set(PLIB_HEADERS
        interface.hpp
        libConfig.hpp
        database.hpp
        utils.hpp
)

add_library(lib${CMAKE_PROJECT_NAME} SHARED ${PLIB_SOURCES} ${PLIB_HEADERS})
