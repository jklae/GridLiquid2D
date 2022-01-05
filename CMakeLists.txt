CMAKE_MINIMUM_REQUIRED(VERSION 3.19.0)

# Set the project name
SET( PROJECT_NAME GridLiquids )
PROJECT(${PROJECT_NAME})

# Define character set as Unicode
add_definitions(-DUNICODE -D_UNICODE)

# Set the header directory
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/ext/DXViewer/DXViewer-2.0.0/include)

# Collect source files
FILE( GLOB SRC ${CMAKE_SOURCE_DIR}/ext/DXViewer/src/*.cpp
                ${CMAKE_SOURCE_DIR}/src/*.cpp)
FILE( GLOB HDR ${CMAKE_SOURCE_DIR}/ext/DXViewer/DXViewer-2.0.0/include/*.h 
                ${CMAKE_SOURCE_DIR}/src/*.h)
FILE( GLOB SHD ${VS_SHD} ${PS_SHD} )

# Link Source files
ADD_EXECUTABLE( ${PROJECT_NAME} WIN32 ${SRC} ${HDR} ${SHD} )