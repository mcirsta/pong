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

find_package(LibArchive REQUIRED)

include_directories(${LibArchive_INCLUDE_DIRS})

add_library(lib${CMAKE_PROJECT_NAME} SHARED ${PLIB_SOURCES} ${PLIB_HEADERS})
target_link_libraries(lib${CMAKE_PROJECT_NAME} ${LibArchive_LIBRARIES})
