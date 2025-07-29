cmake_minimum_required(VERSION 3.22)

project(    
    "stratta"
    VERSION 0.1.0
    DESCRIPTION "the desktop applcation of storyforge"
    HOMEPAGE_URL "https://github.com/yoann01/storyforge_desktop"
    LANGUAGES C CXX
    )


if(NOT DEFINED CMAKE_BUILD_PARALLEL_LEVEL AND DEFINED ENV{NUMBER_OF_PROCESSORS})
    set(CMAKE_BUILD_PARALLEL_LEVEL $ENV{NUMBER_OF_PROCESSORS})
endif()

# Global flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

message(STATUS "========================Qt6Core DIR: ${Qt6Core_DIR}")

if(Qt6_VERSION VERSION_GREATER_EQUAL "6.9.0")
    qt_standard_project_setup()
endif()

set(EXECUTABLE_NAME "stratta")

# CMAKE MODULES
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
include(AddGitSubmodule)


if(ENABLE_WARNINGS)
    include(Warnings)
endif()

# EXTERNAL Dependencies

# 1. Dependencies from vcpkg
message(STATUS "Using VCPKG")


# 2. Qt6 dependencies
find_package(Qt6 COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets Svg REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

add_subdirectory(external/qlementine)
add_subdirectory(external/qlementine-icons)

add_subdirectory(app)

# INSTALL TARGETS
install(
    TARGETS ${EXECUTABLE_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)