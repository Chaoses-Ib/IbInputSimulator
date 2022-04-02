cmake_minimum_required(VERSION 3.14)

project(IbInputSimulator)

# IbWinCppLib
include(FetchContent)
FetchContent_Declare(IbWinCpp
    GIT_REPOSITORY https://github.com/Chaoses-Ib/IbWinCppLib.git
    GIT_TAG        a29ac95537f403ac5ae221cb744d3e82076efbf7
)
FetchContent_MakeAvailable(IbWinCpp)

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
add_library(IbInputSimulator SHARED ${sourceFiles})
target_compile_features(IbInputSimulator PUBLIC cxx_std_20)
target_compile_definitions(IbInputSimulator PRIVATE IB_INPUT_DLLEXPORT)

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