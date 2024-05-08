cmake_minimum_required(VERSION 3.27)
project(my-project LANGUAGES CXX VERSION 0.0.1)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
Include(FetchContent)
set(SOURCES
)
add_executable(OptFCore_load src/OptFCore_load.cpp ${SOURCES})
# begin dependencies from cxxdeps.txt
# cxxdeps dependency OptFrame
FetchContent_Declare(OptFrame GIT_REPOSITORY https://github.com/optframe/optframe.git GIT_TAG 5.0.18)
FetchContent_MakeAvailable(OptFrame)
target_link_libraries(OptFCore_load PRIVATE OptFrameAll)
# cxxdeps dependency Catch2
FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.5.1)
FetchContent_MakeAvailable(Catch2)