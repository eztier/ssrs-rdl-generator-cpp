cmake_minimum_required(VERSION 3.2)

project(app-ssrs-generator VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "-v")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Threads)
find_package(Boost 1.62.0 COMPONENTS thread date_time program_options filesystem system regex REQUIRED)

set(PLUSTACHE ${PROJECT_SOURCE_DIR}/../../vendor)
set(SPDLOG ${PROJECT_SOURCE_DIR}/../../../spdlog/include)

include_directories(
  /usr/local/include
  "${PROJECT_SOURCE_DIR}/../../"
  "${PLUSTACHE}"
  "${SPDLOG}"
)

link_directories(
  ${PostgreSQL_LIBRARY_DIRS} 
)

file(GLOB_RECURSE CPP_SOURCES "${PROJECT_SOURCE_DIR}/index.cpp")

add_executable(app-ssrs-generator ${CPP_SOURCES})

target_link_libraries(
  app-ssrs-generator
  ${Boost_LIBRARIES} 
  Threads::Threads
  libsybdb.so
)

file(COPY "${PROJECT_SOURCE_DIR}/../../tpl" DESTINATION ${CMAKE_BINARY_DIR}/bin)
