cmake_minimum_required(VERSION 3.10)

project(x2tjs)

set(CORE_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../..")
include(${CORE_ROOT_DIR}/common.cmake)

if(NOT EMSCRIPTEN)
    message(FATAL_ERROR "x2tjs is an Emscripten-only target")
endif()

if(NOT TARGET x2tlib)
    add_subdirectory(${CORE_ROOT_DIR}/X2tConverter/build/cmake/library x2tlib)
endif()

add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/src/x2t_api.cpp
)

set_default_options(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE x2tlib)

# Driven from JS, like the other wasm modules in core: the caller writes the
# input into MEMFS, calls x2t_convert, and reads the result back out.
target_link_options(${PROJECT_NAME} PRIVATE
    # Use Emscripten's FreeType port. Linking core's bundled FreeType makes
    # binary -> docx hang with no output; the port does not.
    "-sUSE_FREETYPE=1"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sFILESYSTEM=1"
    "-sENVIRONMENT=web,worker,node"
    "-sMODULARIZE=1"
    "-sEXPORT_NAME=createX2T"
    "-sEXPORTED_FUNCTIONS=['_x2t_convert','_x2t_create_allfonts','_malloc','_free']"
    "-sEXPORTED_RUNTIME_METHODS=['FS','ccall','cwrap','ENV']"
    "-sINVOKE_RUN=0"
    "-sEXIT_RUNTIME=0"
)
