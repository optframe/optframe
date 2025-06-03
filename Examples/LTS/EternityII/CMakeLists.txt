cmake_minimum_required(VERSION 4.0)
project(my-project LANGUAGES CXX VERSION 0.0.1)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
Include(FetchContent)
# add all executables
add_executable(mainEternityII src/mainEternityII.cpp )    # main
add_executable(Example.Test tests/Example.Test.cpp )
add_library(my_headers0 INTERFACE)
target_include_directories(my_headers0 INTERFACE include)
target_link_libraries(mainEternityII PRIVATE my_headers0)
target_link_libraries(Example.Test PRIVATE my_headers0)
# begin dependencies from cxxdeps.txt
# cxxdeps dependency OptFrame
FetchContent_Declare(OptFrame GIT_REPOSITORY https://github.com/optframe/optframe.git GIT_TAG master)
FetchContent_MakeAvailable(OptFrame)
target_link_libraries(my_headers0 INTERFACE OptFrameHeaders)
target_link_libraries(mainEternityII PRIVATE OptFrameHeaders)
target_link_libraries(Example.Test PRIVATE OptFrameHeaders)
# cxxdeps dependency Catch2
FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.5.1)
FetchContent_MakeAvailable(Catch2)
target_link_libraries(Example.Test PRIVATE Catch2::Catch2WithMain)
# finally, add all sources
set(SOURCES
	src/NSSeqSwapCorner.cpp
	src/ProblemInstance.cpp
	src/NSSeqSwapSide.cpp
	src/DeltaMoveRotate.cpp
	src/DeltaMoveSwapCenter.cpp
	src/DeltaMoveSwapCorner.cpp
	src/DeltaMoveSwapSide.cpp
	src/ProblemCommand.cpp
	src/InitialSolutionRandom.cpp
	src/NSSeqSwapCenter.cpp
	src/InitialSolutionGreedy.cpp
	src/Evaluator.cpp
	src/NSSeqRotate.cpp
	src/DeltaMoveSwapRotateCenter.cpp
	src/NSSeqSwapRotateCenter.cpp
)
target_sources(mainEternityII PRIVATE ${SOURCES})
target_sources(Example.Test PRIVATE ${SOURCES})