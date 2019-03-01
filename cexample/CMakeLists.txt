PROJECT(cserver)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
MESSAGE(STATUS "#########################")
SET(CMAKE_BUILD_TYPE Release)
ADD_DEFINITIONS(-std=c++11 -g -Wall )

INCLUDE_DIRECTORIES(
${PROJECT_SOURCE_DIR}
)
file(GLOB ALL_CXX_SRCS
        ./*.cpp
        )
		
add_executable(${PROJECT_NAME} ${ALL_CXX_SRCS} )
set(ALL_LIB  
		pthread
		libcepoll_proactor.a
        )

target_link_libraries(${PROJECT_NAME} ${ALL_LIB})