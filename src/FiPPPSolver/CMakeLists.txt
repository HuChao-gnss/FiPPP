# apps/filetools/CMakeLists.txt
file(GLOB_RECURSE FiPPPSolver  FiPPPSolver.hpp)
source_group("Header Files" FILES ${FiPPPSolver})

add_executable(FiPPPSolver FiPPPSolver.cpp ${FiPPPSolver})

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_directories(FiPPPSolver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/CLAPACKlib/lapacklib)
    target_link_libraries(FiPPPSolver blas lapack)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    target_link_directories(FiPPPSolver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/CLAPACKlib/debug)
    target_link_directories(FiPPPSolver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../third_party/CLAPACKlib/release)
    target_link_libraries(FiPPPSolver blasd lapackd libf2cd)
endif ()

target_link_libraries(FiPPPSolver FiPPP)
install (TARGETS FiPPPSolver DESTINATION bin)
