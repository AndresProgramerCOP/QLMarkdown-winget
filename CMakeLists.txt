cmake_minimum_required(VERSION 3.16)
project(qlmarkdown-all VERSION 1.0)

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(WITH_HIGHLIGHT "Build with highlight wrapper support (requires Boost and Lua)" OFF)

# Include cmark-gfm as a subdirectory
add_subdirectory(cmark-gfm)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-gfm/src
    ${CMAKE_BINARY_DIR}/cmark-gfm/src
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-gfm/extensions
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/extensions
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/MIMEtype
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/b64.c
    ${CMAKE_CURRENT_SOURCE_DIR}/highlight-wrapper
)

# MIMEtype and b64 sources
set(CMARK_EXTRA_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/extensions/extra-extensions.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/extensions/mention.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/extensions/syntaxhighlight.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/extensions/inlineimage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/extensions/emoji.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/extensions/emoji_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/extensions/heads.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/extensions/heads_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/extensions/highlight.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/extensions/math_ext.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/extensions/sub_ext.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/extensions/sup_ext.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/MIMEtype/MIMEtype.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/b64.c/encode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/b64.c/decode.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/b64.c/buffer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/c_log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/string_utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cmark-extra/url.cpp
)

# Optional Highlight library wrapper integration
if(WITH_HIGHLIGHT)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/highlight-wrapper/highlight)
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/highlight-wrapper/highlight/src/include
    )
    list(APPEND CMARK_EXTRA_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/highlight-wrapper/wrapper_highlight.cpp
    )
else()
    # Stub highlight-wrapper functions so compile succeeds with WITH_HIGHLIGHT=OFF
    list(APPEND CMARK_EXTRA_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/highlight-wrapper/highlight_stubs.cpp
    )
endif()

# Find CURL (used by inlineimages)
find_package(CURL)
if(CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
else()
    # If CURL is not found on Windows, we mock the curl_easy_unescape call or rely on standard functions
    add_compile_definitions(WITHOUT_CURL)
endif()

# Build cmark-extra library
add_library(cmark-extra STATIC ${CMARK_EXTRA_SOURCES})
target_link_libraries(cmark-extra PUBLIC libcmark-gfm-extensions_static libcmark-gfm_static)
if(CURL_FOUND)
    target_link_libraries(cmark-extra PRIVATE ${CURL_LIBRARIES})
endif()

# Build QLMarkdown CLI
add_executable(qlmarkdown_cli ${CMAKE_CURRENT_SOURCE_DIR}/qlmarkdown-cli/main.cpp)
target_link_libraries(qlmarkdown_cli PRIVATE cmark-extra)

if(WITH_HIGHLIGHT)
    target_link_libraries(qlmarkdown_cli PRIVATE highlight-lib)
endif()
