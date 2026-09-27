cmake_minimum_required(VERSION 3.16)

project(NewGG VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  cmake_policy(SET CMP0091 NEW)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

option(NEWGG_USE_LIBCXX "Link Clang builds against libc++" OFF)

set(NEWGG_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/I_GGX64")
set(NEWGG_RULES_FILE "${NEWGG_SRC_DIR}/rules.mk")

if(NOT EXISTS "${NEWGG_RULES_FILE}")
  message(FATAL_ERROR "Missing source list: ${NEWGG_RULES_FILE}")
endif()

file(STRINGS "${NEWGG_RULES_FILE}" NEWGG_RULES_LINES ENCODING UTF-8)
set(NEWGG_SOURCES)
set(NEWGG_MISSING)
foreach(LINE IN LISTS NEWGG_RULES_LINES)
  if(LINE MATCHES "^objs \\+= \\$\\(PREFIX\\)/(.+)\\.o")
    set(REL "${CMAKE_MATCH_1}")
    string(REPLACE "\"" "" REL "${REL}")
    if(EXISTS "${NEWGG_SRC_DIR}/${REL}.cpp")
      list(APPEND NEWGG_SOURCES "${NEWGG_SRC_DIR}/${REL}.cpp")
    elseif(EXISTS "${NEWGG_SRC_DIR}/${REL}.c")
      list(APPEND NEWGG_SOURCES "${NEWGG_SRC_DIR}/${REL}.c")
    else()
      list(APPEND NEWGG_MISSING "${REL}")
    endif()
  endif()
endforeach()

list(LENGTH NEWGG_SOURCES NEWGG_SOURCE_COUNT)
if(NEWGG_SOURCE_COUNT EQUAL 0)
  message(FATAL_ERROR "CMake parsed no sources from ${NEWGG_RULES_FILE}")
endif()
if(NEWGG_MISSING)
  list(JOIN NEWGG_MISSING ", " NEWGG_MISSING_TEXT)
  message(FATAL_ERROR "Missing sources listed in rules.mk: ${NEWGG_MISSING_TEXT}")
endif()
message(STATUS "NewGG: ${NEWGG_SOURCE_COUNT} source files from rules.mk")

add_executable(NewGG ${NEWGG_SOURCES})
set_target_properties(NewGG PROPERTIES OUTPUT_NAME NewGG)

target_include_directories(NewGG PRIVATE
  "${NEWGG_SRC_DIR}"
  "${NEWGG_SRC_DIR}/endgame"
  "${CMAKE_CURRENT_SOURCE_DIR}/endgame"
)

target_compile_definitions(NewGG PRIVATE
  $<$<PLATFORM_ID:Windows>:WIN32>
  $<$<PLATFORM_ID:Windows>:_CONSOLE>
  $<$<PLATFORM_ID:Windows>:UNICODE>
  $<$<PLATFORM_ID:Windows>:_UNICODE>
  $<$<CONFIG:Release>:NDEBUG>
  $<$<CONFIG:RelWithDebInfo>:NDEBUG>
  $<$<CONFIG:MinSizeRel>:NDEBUG>
)

set(NEWGG_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
string(TOLOWER "${NEWGG_ARCH}" NEWGG_ARCH)
if(CMAKE_OSX_ARCHITECTURES)
  string(TOLOWER "${CMAKE_OSX_ARCHITECTURES}" NEWGG_ARCH)
endif()

if(MSVC)
  target_compile_options(NewGG PRIVATE
    /utf-8
    /MP
    /W3
    /wd4244
    /wd4706
    /wd4996
    $<$<CONFIG:Release>:/O2 /Oi /Ot /fp:fast>
  )
else()
  target_compile_options(NewGG PRIVATE
    -O3
    -Wno-shift-negative-value
    -Wno-c++11-extensions
    -Wno-switch
    -Wno-unknown-pragmas
  )

  if(NEWGG_ARCH MATCHES "arm64|aarch64")
    if(APPLE)
      target_compile_options(NewGG PRIVATE -arch arm64)
    endif()
  else()
    target_compile_options(NewGG PRIVATE
      -msse
      -msse2
      -msse3
      -msse4.1
      -msse4.2
      -mpopcnt
    )
  endif()

  if(NEWGG_USE_LIBCXX)
    target_compile_options(NewGG PRIVATE -stdlib=libc++)
    target_link_options(NewGG PRIVATE -stdlib=libc++)
  endif()
endif()

find_package(Threads REQUIRED)
target_link_libraries(NewGG PRIVATE Threads::Threads)

install(TARGETS NewGG RUNTIME DESTINATION bin)
