# CMakeLists.txt project file for SDL3 projects
# 
# NB: SDL_image, SDL_ttf and SDL_mixer are not yet tagged properly
# in github to be included via this fetch mechanism.
# As they are updated, I will endevour to keep this gist updated
cmake_minimum_required(VERSION 3.28.3)
project(tiny_renderer C)

set(CMAKE_C_STANDARD 23)

# Include the command that downloads libraries
include(FetchContent)

# define a function for adding git dependencies
function(include_dependency libName gitURL gitTag)
    # setup the declare
    FetchContent_Declare(${libName}
            GIT_REPOSITORY ${gitURL}
            GIT_TAG        ${gitTag}
            GIT_SHALLOW    TRUE
            GIT_PROGRESS   TRUE
    )

    FetchContent_MakeAvailable(${libName})
endfunction()

# add SDL3 support
find_package(SDL3 QUIET)
if (NOT SDL3_FOUND)
    message(STATUS "Getting SDL3 from Github")
    include_dependency(SDL3 https://github.com/libsdl-org/SDL.git release-3.2.18)
else()
    message(STATUS "Using local SDL3")
endif()

add_executable(tiny_renderer WIN32 
    ./src/main.c
    ./src/config.h
    ./src/type.c
    ./src/vector_math.c
    ./src/transformation.c
    ./src/image_view.c
    ./src/render.c
    ./src/util/util.c
    ./src/component.h)

# set the include directory
target_include_directories(tiny_renderer PUBLIC ${SDL3_INCLUDE_DIRS})

target_compile_definitions(tiny_renderer PRIVATE SDL_MAIN_USE_CALLBACKS)

# link all libraries to the project
target_link_libraries(tiny_renderer PRIVATE SDL3::SDL3 m)

target_compile_options(tiny_renderer  PRIVATE -fsanitize=address)
target_link_options(tiny_renderer  PRIVATE -fsanitize=address)

if (WIN32)
    add_custom_command(
            TARGET tiny_renderer POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:tiny_renderer>"
            VERBATIM
    )
endif()