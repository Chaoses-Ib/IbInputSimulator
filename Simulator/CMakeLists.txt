cmake_minimum_required(VERSION 3.15)

project(IbInputSimulator)

option(BUILD_SHARED_LIBS "Build as shared library (DLL) instead of static library" ON)

# Default: MultiThreaded$<$<CONFIG:Debug>:Debug>DLL
# BUILD_SHARED_LIBS is mainly for AHK, which uses MT. So we follow it.
if(BUILD_SHARED_LIBS)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreadedDLL)
endif()

# IbWinCppLib
include(FetchContent)
FetchContent_Declare(IbWinCpp
    GIT_REPOSITORY https://github.com/Chaoses-Ib/IbWinCppLib.git
    GIT_TAG        f630f0e3a2c757652298c626fcd52469e5d5bc81
)

# Save and restore BUILD_SHARED_LIBS to avoid affecting other dependencies
set(_BUILD_SHARED_LIBS_SAVED ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF)
FetchContent_MakeAvailable(IbWinCpp)
set(BUILD_SHARED_LIBS ${_BUILD_SHARED_LIBS_SAVED})

# Detours
find_path(DETOURS_INCLUDE_DIR detours/detours.h)
find_library(DETOURS_LIBRARY detours)

# RapidJSON
find_package(RapidJSON CONFIG REQUIRED)


set(sourceFiles
    dllmain.cpp
    "API 2.cpp"
    "API 3.cpp"
)
list(TRANSFORM sourceFiles PREPEND source/)

if(BUILD_SHARED_LIBS)
    add_library(IbInputSimulator SHARED ${sourceFiles})
    target_compile_definitions(IbInputSimulator PRIVATE IB_INPUT_DLLEXPORT)
else()
    add_library(IbInputSimulator STATIC ${sourceFiles})
    target_compile_definitions(IbInputSimulator PRIVATE IB_INPUT_STATIC)
endif()

target_compile_features(IbInputSimulator PUBLIC cxx_std_20)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 4)
set(PROJECT_VERSION_PATCH 1)
configure_file(source/resource.rc.cmake.in resource.rc @ONLY)
target_sources(IbInputSimulator PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")

target_include_directories(IbInputSimulator
    PUBLIC include
    PRIVATE ${DETOURS_INCLUDE_DIR}
)
target_link_libraries(IbInputSimulator
    PRIVATE IbWinCpp    
    PRIVATE ${DETOURS_LIBRARY}
    PRIVATE rapidjson
)

# Test
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(BUILD_TESTING "Build the testing tree." OFF)
    include(CTest)
    if(BUILD_TESTING)
        find_package(Boost REQUIRED COMPONENTS unit_test_framework)
        find_package(fmt CONFIG REQUIRED)

        set(testFiles
            test.cpp
        )
        list(TRANSFORM testFiles PREPEND test/)

        add_executable(IbInputSimulator_Test ${testFiles})
        target_link_libraries(IbInputSimulator_Test
            PRIVATE IbInputSimulator
            PRIVATE Boost::unit_test_framework
            PRIVATE IbWinCpp
            PRIVATE fmt::fmt
        )
    endif()
endif()