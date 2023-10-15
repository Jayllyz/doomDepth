cmake_minimum_required(VERSION 3.0.0)
project(doomDepth VERSION 1.0.0)

set(CMAKE_C_STANDARD gnu11)
set(CMAKE_C_FLAGS "-Wall")

find_package(PkgConfig REQUIRED)

pkg_check_modules(SQLITE3 REQUIRED sqlite3)
include_directories(${SQLITE3_INCLUDE_DIRS})
link_directories(${SQLITE3_LIBRARY_DIRS})
add_definitions(${SQLITE3_CFLAGS_OTHER})

find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
    add_custom_target(
        format
        COMMENT "Running clang-format to change files"
        COMMAND ${CLANG_FORMAT} -i -style=file ${CMAKE_CURRENT_SOURCE_DIR}/*.c
        COMMAND ${CLANG_FORMAT} -i -style=file ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        COMMAND ${CLANG_FORMAT} -i -style=file ${CMAKE_CURRENT_SOURCE_DIR}/settings/*.c
    )
endif()

add_executable(doomDepth src/main.c src/utils.c src/start.c src/ansii_print.c src/fight.c src/map.c)
target_link_libraries(doomDepth ${SQLITE3_LIBRARIES})
