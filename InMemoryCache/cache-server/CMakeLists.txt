set (PROJECT_NAME "SimpleCacheServer" CXX)

project(${PROJECT_NAME})

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/output)
if (UNIX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif (UNIX)

if (MSVC)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D _WIN32_WINNT=0x0501")
endif (MSVC)




#set (BOOST_ROOT "c:/boost/boost_1_59_0")

set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)
set (BOOST_ALL_DYN_LINK OFF)


find_package (Boost 1.59.0 REQUIRED COMPONENTS system filesystem)
if (Boost_FOUND)
  include_directories (SYSTEM ${Boost_INCLUDE_DIR})

  link_directories (${Boost_LIBRARY_DIRS})
endif ()


add_executable (${PROJECT_NAME} Main.cpp Server.cpp Server.h StorageProvider.cpp StorageProvider.h Connection.cpp Connection.h ConnectionManager.cpp ConnectionManager.h OldestInsertionEviction.cpp EvictionStrategy.h )

target_link_libraries (${PROJECT_NAME} ${Boost_LIBRARIES})