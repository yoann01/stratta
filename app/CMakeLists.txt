set(SOURCE_FILES
    main/main.cpp
    mainwindow/mainwindow.cpp
    mainwindow/viewport/UsdViewport.cpp

)

set(HEADER_FILES 
    mainwindow/mainwindow.h
    mainwindow/viewport/UsdViewport.h
)


set(QRC_SOURCE_FILES
    showcase/showcase.qrc
)



set(CMAKE_INCLUDE_CURRENT_DIR ON)


qt_add_resources(APP_RESOURCES
    ${QRC_SOURCE_FILES}
)

qt_add_executable(${EXECUTABLE_NAME}
    ${SOURCE_FILES}
    ${HEADER_FILES}
    ${APP_RESOURCES}
)

find_package(pxr REQUIRED)
find_package(MaterialX)



target_compile_definitions(${EXECUTABLE_NAME} PRIVATE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
)

if(MSVC)
    target_compile_options(${EXECUTABLE_NAME} PRIVATE 
        /permissive-
        /Zc:__cplusplus
        /utf-8
    )
endif()


target_include_directories(${EXECUTABLE_NAME} PRIVATE 
    ${PXR_INCLUDE_DIRS}
)

# === Link libraries ===
target_link_libraries(
    ${EXECUTABLE_NAME}
    PRIVATE
        
        # Libraries from vcpkg
        nlohmann_json::nlohmann_json
        spdlog::spdlog
        
        # Qt libraries
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets
        Qt6::Svg

        TBB::tbb

        qlementine
        qlementine-icons

        ${PXR_LIBRARIES}
)


# === POST_BUILD ===
add_custom_command(TARGET ${EXECUTABLE_NAME}
    POST_BUILD

    COMMAND ${CMAKE_COMMAND} -E echo "=== Copying extras ==="
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/extras"
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different 
        "${CMAKE_HOME_DIRECTORY}/extras" 
        "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/extras"

    # Copy USD plugins to the correct location
    COMMAND ${CMAKE_COMMAND} -E echo "=== Setting up USD plugins ==="
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/lib/usd"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/bin/usd"
    


     COMMAND ${CMAKE_COMMAND} -E echo "Path to lib: ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/lib/usd"
     COMMAND ${CMAKE_COMMAND} -E echo "Path to bin: ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin/usd"
    COMMAND ${CMAKE_COMMAND} -E echo "Path to plugin: ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/plugin"



    # Copy plugin definitions
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/lib/usd"
        "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/lib/usd"
    
    # Copy plugin implementations  
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin/usd"
        "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/bin/usd"


    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin"
        "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/"

    # Also copy any plugin libraries from the main lib directory
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/plugin"
        "$<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/plugin"
        
    # Set environment variable for debugging
    COMMAND ${CMAKE_COMMAND} -E echo "Set PXR_PLUGINPATH_NAME to: $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/lib/usd"

)


# === Windows Qt Deployment ===
if(WIN32)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${Qt6_DIR}/../../../bin")

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}" "$<TARGET_FILE:${EXECUTABLE_NAME}>"
            COMMENT "Running windeployqt to copy Qt DLLs and plugins."
        )
    else()
        message(WARNING "windeployqt not found. DLLs will not be deployed automatically.")
    endif()
endif()