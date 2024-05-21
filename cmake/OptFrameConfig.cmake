# avoid reloading
if(NOT TARGET OptFrame::OptFrameAll)
    # provide path for scripts (if necessary)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
    # load top-level cmakelists (not recommended, but easy!)
    include("${CMAKE_CURRENT_LIST_DIR}/../CMakeLists.txt")
    # exports include dir
    set(OptFrame_FOUND ON)
    set(OptFrame_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/../include/")
    # set(OptFrame_LIBRARIES "")
endif()