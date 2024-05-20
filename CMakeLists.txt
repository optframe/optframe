cmake_minimum_required(VERSION 3.27)

# detect if OptFrame is being used,
# disable testsuite, demos and C library in that case
if(NOT DEFINED PROJECT_NAME)
  set(IS_SUBPROJECT OFF)
else()
  set(IS_SUBPROJECT ON)
endif()

project(OptFrame LANGUAGES CXX VERSION 0.0.1)
# OptFrame default should be C++17 here, unless for testing
set (CMAKE_CXX_STANDARD 17)
# set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
Include(FetchContent)

option(OPTFRAME_DEVELOPMENT_BUILD "Build tests, demos, etc" OFF)
if (NOT IS_SUBPROJECT)
   set(OPTFRAME_DEVELOPMENT_BUILD ON)
   # force C++20 for local development
   set (CMAKE_CXX_STANDARD 20)
endif()
include(CMakeDependentOption)
cmake_dependent_option(OPTFRAME_BUILD_TESTING  "Build the tests"  ON  "OPTFRAME_DEVELOPMENT_BUILD" OFF)
cmake_dependent_option(OPTFRAME_BUILD_DEMOS    "Build code demos" ON "OPTFRAME_DEVELOPMENT_BUILD" OFF)
cmake_dependent_option(OPTFRAME_BUILD_CLIBRARY "Build C library"  ON "OPTFRAME_DEVELOPMENT_BUILD" OFF)

# ====== configure project with header only library ======
add_library(OptFrameAll INTERFACE)
add_library(OptFrame::OptFrameAll ALIAS OptFrameAll)
target_include_directories(OptFrameAll INTERFACE include/)

if (OPTFRAME_BUILD_DEMOS AND NOT IS_SUBPROJECT)
add_executable(app_demo_opt demo/00_SimpleBuild/src/OptFCore_load.cpp)
target_link_libraries(app_demo_opt PRIVATE OptFrameAll)
endif()

if (OPTFRAME_BUILD_CLIBRARY AND NOT IS_SUBPROJECT)
add_library(optframe_lib MODULE src/OptFrameLib/OptFrameLib.cpp)
add_library(OptFrame::optframe_lib ALIAS optframe_lib)
target_link_libraries(optframe_lib PRIVATE OptFrameAll)
#
set_target_properties(optframe_lib PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build
    LIBRARY_OUTPUT_NAME optframe_lib
)
set_target_properties(optframe_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)
# https://stackoverflow.com/questions/58059048/ambiguous-byte-definition-in-rpcndr-and-cstddef#58060367
target_compile_definitions(optframe_lib PRIVATE _HAS_STD_BYTE=0)
endif()

if (OPTFRAME_BUILD_TESTING AND NOT IS_SUBPROJECT)

#add_library(catch2 INTERFACE)
#target_include_directories(catch2 INTERFACE tests/thirdparty/)

FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.3.1)
FetchContent_MakeAvailable(Catch2)
#
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
#
#add_executable(my_test tests/all.Test.cpp  tests/thirdparty/catch2/catch_amalgamated.cpp)
add_executable(my_test tests/all.Test.cpp)
target_link_libraries(my_test PRIVATE OptFrameAll Catch2::Catch2WithMain) #catch2)

# https://stackoverflow.com/questions/58059048/ambiguous-byte-definition-in-rpcndr-and-cstddef#58060367
target_compile_definitions(my_test PRIVATE _HAS_STD_BYTE=0)

#catch_discover_tests(my_test)
#enable_testing()
endif()
