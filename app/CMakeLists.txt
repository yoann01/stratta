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

# === Include paths ===
target_include_directories(${EXECUTABLE_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../
    ${PXR_INCLUDE_DIRS}
)

# === Compile options ===
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

        # Qlementine
        qlementine
        qlementine-icons

        # USD libraries
        ${PXR_LIBRARIES}
        OpenGL::GL
        GLEW::GLEW
        #tbb
        TBB::tbb



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