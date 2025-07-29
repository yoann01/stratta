function(add_git_submodule relative_dir)
    cmake_minimum_required(VERSION 3.14) # pour GIT_SUBMODULES
    find_package(Git REQUIRED)

    set(FULL_DIR "${CMAKE_SOURCE_DIR}/${relative_dir}")
    set(SUBMODULE_PATH "${relative_dir}")

    # Vérifie si le dossier existe
    if (NOT EXISTS "${FULL_DIR}")
        message(STATUS "Fetching Git submodule '${SUBMODULE_PATH}'...")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- "${SUBMODULE_PATH}"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT
        )
        if (NOT GIT_SUBMOD_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to initialize Git submodule '${SUBMODULE_PATH}'")
        endif()
    else()
        # On vérifie s'il est à jour
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule status "${SUBMODULE_PATH}"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE SUBMOD_STATUS
        )

        string(FIND "${SUBMOD_STATUS}" "-" IS_OUTDATED)
        if (NOT IS_OUTDATED EQUAL -1)
            message(STATUS "Updating outdated submodule '${SUBMODULE_PATH}'...")
            execute_process(
                COMMAND ${GIT_EXECUTABLE} submodule update --recursive --remote "${SUBMODULE_PATH}"
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                RESULT_VARIABLE GIT_UPDATE_RESULT
            )
            if (NOT GIT_UPDATE_RESULT EQUAL 0)
                message(FATAL_ERROR "Failed to update submodule '${SUBMODULE_PATH}'")
            endif()
        endif()
    endif()

    # Si le sous-module contient un CMakeLists.txt, on l'ajoute
    if (EXISTS "${FULL_DIR}/CMakeLists.txt")
        message(STATUS "Submodule is a CMake project: '${SUBMODULE_PATH}'")
        add_subdirectory("${FULL_DIR}")
    else()
        message(STATUS "Submodule is not a CMake project: '${SUBMODULE_PATH}' (no CMakeLists.txt)")
    endif()
endfunction()
