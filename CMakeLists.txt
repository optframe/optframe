# cmake_minimum_required(VERSION 3.27)
cmake_minimum_required(VERSION 4.0)

# set(CMAKE_CXX_COMPILER /usr/bin/g++-15)
set(CMAKE_CXX_COMPILER /usr/bin/clang++-19)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

option(USE_CXX_MODULES "Use modules (default OFF unless -DUSE_CXX_MODULES=ON)" ON)
option(USE_CXX_CONCEPTS "Use concepts (default OFF unless -DUSE_CXX_CONCEPTS=ON)" OFF)

# detect if OptFrame is being used,
# disable testsuite, demos and C library in that case
if(NOT DEFINED PROJECT_NAME)
  set(IS_SUBPROJECT OFF)
else()
  set(IS_SUBPROJECT ON)
endif()

# OptFrame default should be C++17 here, unless for advanced features
if(USE_CXX_MODULES)
set (CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
set(CMAKE_CXX_MODULE_STD 1)
elseif(USE_CXX_CONCEPTS)
set (CMAKE_CXX_STANDARD 20)
else()
set (CMAKE_CXX_STANDARD 17)
endif()

set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(OptFrame LANGUAGES CXX VERSION 0.0.1)

Include(FetchContent)

option(OPTFRAME_DEVELOPMENT_BUILD "Build tests, demos, etc" OFF)
if (NOT IS_SUBPROJECT)
   set(OPTFRAME_DEVELOPMENT_BUILD ON)
   # force C++23 for local development
   set (CMAKE_CXX_STANDARD 23)
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

#if(USE_CXX_MODULES)

add_library(optframe_nnptr)
target_link_libraries(optframe_nnptr PUBLIC OptFrameAll) # too much...
target_sources(optframe_nnptr PUBLIC  FILE_SET CXX_MODULES FILES src/optframe/nnptr.cppm)

add_library(optframe_opview)
target_link_libraries(optframe_opview PUBLIC OptFrameAll) # too much...
target_sources(optframe_opview  PUBLIC  FILE_SET CXX_MODULES FILES src/optframe/opview.cppm)

add_library(optframe_semstream)
target_link_libraries(optframe_semstream PUBLIC OptFrameAll)
target_sources(optframe_semstream  PUBLIC  FILE_SET CXX_MODULES FILES src/optframe/semstream.cppm)

add_library(optframe_concepts_my)
target_link_libraries(optframe_concepts_my PUBLIC OptFrameAll)
target_sources(optframe_concepts_my  PUBLIC  FILE_SET CXX_MODULES FILES src/optframe/concepts_my.cppm)
target_link_libraries(optframe_concepts_my PUBLIC optframe_nnptr)
target_link_libraries(optframe_concepts_my PUBLIC optframe_opview)

add_library(optframe_concepts)
target_link_libraries(optframe_concepts PUBLIC OptFrameAll)
target_sources(optframe_concepts  PUBLIC  FILE_SET CXX_MODULES FILES src/optframe/concepts.cppm)
target_link_libraries(optframe_concepts PUBLIC optframe_concepts_my)

add_library(optframe_values)
target_link_libraries(optframe_values PUBLIC OptFrameAll)
target_sources(optframe_values  PUBLIC  FILE_SET CXX_MODULES FILES src/optframe/values.cppm)
target_link_libraries(optframe_values PUBLIC optframe_concepts)

add_library(optframe_component)
target_link_libraries(optframe_component PUBLIC OptFrameAll)
target_sources(optframe_component  PUBLIC  FILE_SET CXX_MODULES FILES src/optframe/component.cppm)
target_link_libraries(optframe_component PUBLIC optframe_semstream)

add_library(optframe_timer)
target_link_libraries(optframe_timer PUBLIC OptFrameAll)
target_sources(optframe_timer  PUBLIC  FILE_SET CXX_MODULES FILES src/optframe/timer.cppm)
target_link_libraries(optframe_timer PUBLIC optframe_component)

add_library(optframe_core)
target_link_libraries(optframe_core PUBLIC OptFrameAll)
target_sources(optframe_core  PUBLIC  FILE_SET CXX_MODULES FILES src/optframe/core.cppm)
target_link_libraries(optframe_core PUBLIC optframe_component)
target_link_libraries(optframe_core PUBLIC optframe_concepts)
target_link_libraries(optframe_core PUBLIC optframe_values)
target_link_libraries(optframe_core PUBLIC optframe_timer)

add_library(optframe_pareto)
target_link_libraries(optframe_pareto PUBLIC OptFrameAll)
target_sources(optframe_pareto  PUBLIC  FILE_SET CXX_MODULES FILES src/optframe/pareto.cppm)
target_link_libraries(optframe_pareto PUBLIC optframe_core)

add_library(optframe_search)
target_link_libraries(optframe_search PUBLIC OptFrameAll)
target_sources(optframe_search  PUBLIC  FILE_SET CXX_MODULES FILES src/optframe/search.cppm)
target_link_libraries(optframe_search PUBLIC optframe_core)
target_link_libraries(optframe_search PUBLIC optframe_pareto)


add_library(optframe_heuristics)
target_link_libraries(optframe_heuristics PUBLIC OptFrameAll)
target_sources(optframe_heuristics  PUBLIC  FILE_SET CXX_MODULES FILES src/optframe/heuristics.cppm)
target_link_libraries(optframe_heuristics PUBLIC optframe_core)
target_link_libraries(optframe_heuristics PUBLIC optframe_search)


add_executable(trymod demo/07_Try_Modules/trymod.cpp)

target_link_libraries(trymod PRIVATE optframe_core)
target_link_libraries(trymod PRIVATE optframe_heuristics)
#endif()

if (OPTFRAME_BUILD_TESTING AND NOT IS_SUBPROJECT)

#add_library(catch2 INTERFACE)
#target_include_directories(catch2 INTERFACE tests/thirdparty/)

FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.8.0) # 3.3.1
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
