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



# === Qt6 setup ===
if(Qt6_VERSION VERSION_GREATER_EQUAL "6.9.0")
    qt_standard_project_setup()
endif()

set(EXECUTABLE_NAME "stratta")

# === CMake Modules ===
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")


include(AddGitSubmodule)

if(ENABLE_WARNINGS)
    include(Warnings)
endif()


# === PYTHON ===
# -- Manually define Python3 if using NVIDIA USD's internal Python distro --
set(Python3_ROOT_DIR "${CMAKE_SOURCE_DIR}/../USD/python")
set(Python3_EXECUTABLE "${Python3_ROOT_DIR}/python.exe")
set(Python3_LIBRARY "${Python3_ROOT_DIR}/libs/python311.lib")
set(Python3_INCLUDE_DIR "${Python3_ROOT_DIR}/include")

# Force CMake to use the manually specified Python
set(Python3_FIND_REGISTRY NEVER)
set(Python3_FIND_STRATEGY LOCATION)


message(STATUS "Using Python from: ${Python3_EXECUTABLE}")
message(STATUS "Python include dir: ${Python3_INCLUDE_DIR}")
message(STATUS "Python library: ${Python3_LIBRARY}")


find_package(Python3 REQUIRED COMPONENTS Interpreter Development)


# === TBB ===
set(TBB_ROOT "${CMAKE_SOURCE_DIR}/../USD")
set(TBB_INCLUDE_DIR "${TBB_ROOT}/include")
set(TBB_LIBRARY "${TBB_ROOT}/lib/tbb.lib")

# Affichage
message(STATUS "TBB root install dir: ${TBB_ROOT}")
message(STATUS "TBB include dirs: ${TBB_INCLUDE_DIR}")
message(STATUS "TBB lib dirs: ${TBB_LIBRARY}")

# Déclaration manuelle d'une target IMPORTED
add_library(TBB::tbb UNKNOWN IMPORTED)
set_target_properties(TBB::tbb PROPERTIES
    IMPORTED_LOCATION "${TBB_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${TBB_INCLUDE_DIR}"
)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/../USD/lib/cmake/TBB")

# === USD ===
# IMPORTANT: set correct path to pxrConfig.cmake from NVIDIA USD distro
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${CMAKE_SOURCE_DIR}/../USD")

find_package(pxr CONFIG REQUIRED)

message(STATUS "USD include dirs: ${PXR_INCLUDE_DIRS}")
message(STATUS "USD libraries: ${PXR_LIBRARIES}")


# === External dependencies ===

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