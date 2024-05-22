cmake_minimum_required(VERSION 3.27)
project(my-project LANGUAGES CXX VERSION 0.0.1)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
Include(FetchContent)
add_definitions(-DOPTFRAME_AC)
# add all executables
add_executable(mainTSP src/mainTSP.cpp )
add_executable(Example.Test tests/Example.Test.cpp )
# begin dependencies from cxxdeps.txt
# cxxdeps dependency OptFrame
set(OptFrame_DIR "${CMAKE_SOURCE_DIR}/../../cmake")
find_package(OptFrame REQUIRED)
include_directories("${OptFrame_INCLUDE_DIRS}")
# cxxdeps dependency Catch2
FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.5.1)
FetchContent_MakeAvailable(Catch2)
target_link_libraries(Example.Test PRIVATE Catch2::Catch2WithMain)
# finally, add all sources
set(SOURCES
	src/NSEnumShift.cpp
	src/ProblemInstance.cpp
	src/DeltaMoveTSP2Opt.cpp
	src/RandomInitialSolution.cpp
	src/NSEnumSwap.cpp
	src/NearestNeighborConstructive.cpp
	src/ConstructiveBestInsertion.cpp
	src/ProblemCommand.cpp
	src/Evaluator.cpp
	src/DeltaMoveTSPOrOptk.cpp
)
target_sources(mainTSP PRIVATE ${SOURCES})
target_sources(Example.Test PRIVATE ${SOURCES})