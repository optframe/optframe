cmake_minimum_required(VERSION 3.27)
project(my-project LANGUAGES CXX VERSION 0.0.1)
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
Include(FetchContent)
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
add_executable(mainTSP src/mainTSP.cpp ${SOURCES})
add_executable(Example.Test tests/Example.Test.cpp ${SOURCES})
# begin dependencies from cxxdeps.txt
# cxxdeps dependency OptFrame
FetchContent_Declare(OptFrame GIT_REPOSITORY https://github.com/optframe/optframe.git GIT_TAG master)
FetchContent_MakeAvailable(OptFrame)
target_link_libraries(mainTSP PRIVATE OptFrameAll)
target_link_libraries(Example.Test PRIVATE OptFrameAll)
# cxxdeps dependency Catch2
FetchContent_Declare(Catch2 GIT_REPOSITORY https://github.com/catchorg/Catch2.git GIT_TAG v3.5.1)
FetchContent_MakeAvailable(Catch2)
target_link_libraries(Example.Test PRIVATE Catch2::Catch2WithMain)