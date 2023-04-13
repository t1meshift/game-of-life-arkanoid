include(FetchContent)

if (DISABLE_VCPKG)
    set(_FIND_PKG_ARGS QUIET)
else()
    set(_FIND_PKG_ARGS CONFIG REQUIRED)
endif()

# Dependencies

## fmtlib
find_package(fmt ${_FIND_PKG_ARGS})
if(NOT fmt_FOUND)
    FetchContent_Declare(
            fmt
            URL https://github.com/fmtlib/fmt/releases/download/9.1.0/fmt-9.1.0.zip
    )
FetchContent_MakeAvailable(fmt)
endif()

## raylib
find_package(raylib ${_FIND_PKG_ARGS})
if (NOT raylib_FOUND)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/4.5.0.zip
    )
    FetchContent_MakeAvailable(raylib)
endif()

### raygui
#if (DISABLE_VCPKG)
#    FetchContent_Declare(
#        raygui
#        URL https://github.com/raysan5/raygui/archive/refs/tags/3.2.zip
#    )
#    FetchContent_MakeAvailable(raygui)
#endif()
#find_path(RAYGUI_INCLUDE_DIRS "raygui.h")

## raylib-cpp
find_package(raylib_cpp QUIET)
if (NOT raylib_cpp_FOUND)
    FetchContent_Declare(
        raylib_cpp
        GIT_REPOSITORY https://github.com/RobLoach/raylib-cpp.git
        GIT_TAG v4.5.0
    )
    FetchContent_MakeAvailable(raylib_cpp)
endif()


target_include_directories("${EXE_NAME}" PRIVATE ${RAYGUI_INCLUDE_DIRS})
target_link_libraries("${EXE_NAME}" PRIVATE raylib raylib_cpp fmt::fmt)