cmake_minimum_required (VERSION 3.3)
project (crawl)

include ("CMakeFiles.txt")


#####
### Define targets.
#####

add_executable (tilegen "")  # sources added below
add_executable (crawl "")    # sources added below


#####
### Define file generation rules.
#####

find_package (FLEX)
find_package (BISON)
flex_target (levcomp_scanner ${UTIL_DIR}/levcomp.lpp ${UTIL_DIR}/levcomp.lex.cc)
bison_target (levcomp_parser ${UTIL_DIR}/levcomp.ypp ${UTIL_DIR}/levcomp.tab.cc
              COMPILE_FLAGS --defines=${UTIL_DIR}/levcomp.tab.h)
add_flex_bison_dependency (levcomp_scanner levcomp_parser)

foreach (_input main dngn floor wall feat player gui icons)
        add_custom_command (
                COMMAND            tilegen ${RLTILES_DIR}/dc-${_input}.txt
                OUTPUT             tiledef-${_input}.cc tiledef-${_input}.h
                DEPENDS            ${RLTILES_DIR}/dc-unrand.txt
                WORKING_DIRECTORY  ${RLTILES_DIR}
                COMMENT            "TileGen: ${_input}"
        )
        list (APPEND TILEDEF_SRCS ${RLTILES_DIR}/tiledef-${_input}.cc)
        list (APPEND TILEDEF_HDRS ${RLTILES_DIR}/tiledef-${_input}.h)
endforeach ()

add_custom_command (
        COMMAND            ${UTIL_DIR}/art-data.pl
        OUTPUT             ${SRC_DIR}/art-data.h ${SRC_DIR}/art-enum.h
                           ${RLTILES_DIR}/tiledef-unrand.cc
                           ${RLTILES_DIR}/dc-unrand.txt
        WORKING_DIRECTORY  ${SRC_DIR}
)

add_custom_command (
        COMMAND            ${UTIL_DIR}/gen-mst.pl
        OUTPUT             mon-mst.h
        WORKING_DIRECTORY  ${SRC_DIR}
)

add_custom_command (
        COMMAND            ${UTIL_DIR}/gen-mi-enum
        OUTPUT             mi-enum.h
        WORKING_DIRECTORY  ${SRC_DIR}
)

add_custom_command (
        COMMAND            ${UTIL_DIR}/cmd-name.pl
        OUTPUT             cmd-name.h
        WORKING_DIRECTORY  ${SRC_DIR}
)

add_custom_command (
        COMMAND            ${UTIL_DIR}/gen_ver.pl build.h
        OUTPUT             build.h
        WORKING_DIRECTORY  ${SRC_DIR}
)

add_custom_command (
        COMMAND            ${UTIL_DIR}/gen-cflg.pl compflag.h "" "" "" ""
        OUTPUT             compflag.h
        WORKING_DIRECTORY  ${SRC_DIR}
)

add_custom_command (POST_BUILD
        TARGET             crawl
        COMMAND            ${UTIL_DIR}/gen-luatags.pl
        WORKING_DIRECTORY  ${SRC_DIR}
        COMMENT            "Generating Lua tags"
)


#####
### Locate dependencies.
#####

include (FindThreads)

find_package (ZLIB)
find_package (Lua)

include (FindPkgConfig)
pkg_check_modules (CURSES ncursesw)
if (NOT ${CURSES_FOUND})
        find_package(Curses)
endif ()

target_link_libraries (crawl PRIVATE
        ${ZLIB_LIBRARIES}
        ${LUA_LIBRARIES}
        ${CURSES_LIBRARIES}
        sqlite3
        pthread
)

target_include_directories (crawl PRIVATE
        ${SRC_DIR}
        ${RLTILES_DIR}
        ${UTIL_DIR}
        ${ZLIB_INCLUDE_DIRS}
        ${LUA_INCLUDE_DIRS}
        ${CURSES_INCLUDE_DIRS}
)

target_include_directories (tilegen PRIVATE ${SRC_DIR})


#####
### Handle preprocessor directives and build options.
#####

target_compile_definitions (crawl PRIVATE -DASSERTS -DCLUA_BINDINGS)

option (ENABLE_FULLDEBUG "Enable full in-game debugging." NO)
if (ENABLE_FULLDEBUG)
        target_compile_definitions (crawl PRIVATE -DFULLDEBUG)
endif ()

option (ENABLE_WIZMODE "Enable Wizard mode." YES)
if (ENABLE_WIZMODE)
        target_compile_definitions (crawl PRIVATE -DWIZARD)
endif ()

option (ENABLE_TILES "Enable local tiles." NO)
if (ENABLE_TILES)
        target_compile_definitions (crawl PRIVATE -DUSE_TILE -DUSE_TILE_LOCAL)
        target_sources (crawl PRIVATE ${TILES_SRCS})
endif ()

option (ENABLE_WEBTILES "Enable web tiles." NO)
if (ENABLE_WEBTILES)
        target_compile_definitions (crawl PRIVATE -DUSE_TILE -DUSE_TILE_WEB)
        target_sources (crawl PRIVATE ${WEBTILES_SRCS})
endif ()

# Compiler options.
target_compile_features (crawl PRIVATE cxx_constexpr)
target_compile_features (tilegen PRIVATE cxx_constexpr)

add_compile_options (
        -pedantic -Wall -Werror -Wextra
        -Wformat-security -Wmissing-declarations -Wno-array-bounds
        -Wno-format-zero-length -Wno-parentheses -Wredundant-decls
        -Wshadow -Wundef -Wwrite-strings
        ${CURSES_CFLAGS}
)

# Feature detection.
include (CheckFunctionExists)
check_function_exists (fdatasync CRAWL_HAVE_FDATASYNC)
check_function_exists (strlcpy   CRAWL_HAVE_STRLCPY)
check_function_exists (mkstemp   CRAWL_HAVE_MKSTEMP)
configure_file (${SRC_DIR}/config.h.in ${CMAKE_BINARY_DIR}/config.h)

target_include_directories (crawl PRIVATE ${CMAKE_BINARY_DIR})


#####
### Add common source files.
#####

target_sources (crawl PRIVATE
        ${CRAWL_SRCS} ${CRAWL_HDRS}
        ${PLATFORM_SRCS} ${PLATFORM_HDRS}
        ${TILEDEF_SRCS} ${TILEDEF_HDRS}
        ${FLEX_levcomp_scanner_OUTPUTS}
        ${BISON_levcomp_parser_OUTPUTS}
)

target_sources (tilegen PRIVATE ${TILEGEN_SRCS} ${TILEGEN_HDRS})


# vim:ts=8:sw=8:et:ft=cmake
