# CMakeLists.txt project file for SDL3 projects
# 
# NB: SDL_image, SDL_ttf and SDL_mixer are not yet tagged properly
# in github to be included via this fetch mechanism.
# As they are updated, I will endevour to keep this gist updated
cmake_minimum_required(VERSION 3.28.3)
project(no_nonsense_engine C)



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


add_executable(no_nonsense_engine WIN32 
   ./src/config.h
    ./src/main.c
   ./src/type.c
   ./src/vector_math.c
   ./src/model.c
   ./src/transformation.c
   ./src/image_view.c
   ./src/render.c
   ./src/util/util.c
   ./src/tga_image.c
   ./dependencies/microui/microui.c
   ./dependencies/microui/atlas.inl
    )

set_target_properties(no_nonsense_engine PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# set the include directory
target_include_directories(no_nonsense_engine PUBLIC 
    src 
    dependencies)



target_compile_definitions(no_nonsense_engine PRIVATE CIMGUI_DEFINE_ENUMS_AND_STRUCTS=1)
# link all libraries to the project
target_link_libraries(no_nonsense_engine PRIVATE 
                                m
                                SDL3::SDL3
                                )

target_compile_options(no_nonsense_engine  PRIVATE -fsanitize=address)
target_link_options(no_nonsense_engine  PRIVATE -fsanitize=address)

if (WIN32)
    add_custom_command(
            TARGET no_nonsense_engine POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:SDL3::SDL3>" "$<TARGET_FILE_DIR:no_nonsense_engine>"
            VERBATIM
    )
endif()