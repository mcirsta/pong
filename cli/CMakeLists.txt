set(CLI_SOURCES
	main.cpp
)

find_package(Boost 1.58.0 REQUIRED COMPONENTS program_options)

include_directories(${Boost_INCLUDE_DIRS})

add_executable(${CMAKE_PROJECT_NAME} ${CLI_SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_LIBRARIES})
