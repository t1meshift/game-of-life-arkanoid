set(PROJECT_NAME "raylib_cpp_demo")
set(EXE_NAME "demo")
option(DISABLE_VCPKG "Turn off vcpkg support" FALSE)

cmake_minimum_required(VERSION 3.25)
project("${PROJECT_NAME}")

set(CMAKE_CXX_STANDARD 20)

# emcc check
if (EMSCRIPTEN)
    message("vcpkg is OFF because of Emscripten")
    set(DISABLE_VCPKG TRUE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
endif()

# vcpkg check
if (DEFINED VCPKG_MANIFEST_DIR AND NOT DISABLE_VCPKG)
    message("vcpkg is ON")
else()
    message("vcpkg is OFF")
    set(DISABLE_VCPKG TRUE)
endif()

add_executable("${EXE_NAME}"
        main.cpp
        tools/CellAutomata.cpp tools/CellAutomata.h
        scenes/BaseScene.h
        input/KeyboardInputHandler.h
        scenes/ArkanoidScene.cpp scenes/ArkanoidScene.h input/TouchInputHandler.h)

if (EMSCRIPTEN)
    target_link_options("${EXE_NAME}" PRIVATE "--shell-file" "${CMAKE_CURRENT_LIST_DIR}/emshell.html")
endif()

include(cmake/deps.cmake)
