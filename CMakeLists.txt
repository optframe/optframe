cmake_minimum_required(VERSION 3.27)
project(optframe-project LANGUAGES CXX VERSION 0.0.1)
#
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
Include(FetchContent)

# ====== configure project with header only library ======
add_library(OptFrameAll INTERFACE)
target_include_directories(OptFrameAll INTERFACE include/)
add_executable(app_demo_opt demo/00_SimpleBuild/OptFCore_load.cpp)
target_link_libraries(app_demo_opt PRIVATE OptFrameAll)

# ===== check if external catch2 library is used ===== 
option(DISABLE_OPTFRAME_TESTING "Build tests")
if(NOT DISABLE_OPTFRAME_TESTING)

add_library(catch2 INTERFACE)
target_include_directories(catch2 INTERFACE tests/thirdparty/)

#FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.3.1)
#FetchContent_MakeAvailable(Catch2)
#
#list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
#include(CTest)
#include(Catch)
#
add_executable(my_test tests/all.Test.cpp  tests/thirdparty/catch2/catch_amalgamated.cpp)
target_link_libraries(my_test PRIVATE OptFrameAll catch2)#Catch2::Catch2WithMain)
#catch_discover_tests(my_test)
#enable_testing()
endif()
