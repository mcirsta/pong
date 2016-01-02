set(PLIB_SOURCES
        interface.cpp
        libConfig.cpp
        database.cpp
        utils.cpp
        dbQueries.cpp
)

set(PLIB_HEADERS
        interface.hpp
        libConfig.hpp
        database.hpp
        utils.hpp
)

find_package(LibArchive REQUIRED)
find_package(Sqlite3 REQUIRED)

include_directories(${LibArchive_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIR})

add_library(lib${CMAKE_PROJECT_NAME} SHARED ${PLIB_SOURCES} ${PLIB_HEADERS})
target_link_libraries(lib${CMAKE_PROJECT_NAME} ${LibArchive_LIBRARIES} ${SQLITE3_LIBRARIES})
