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
option(NEWGG_ANDROID_STATIC "Statically link the Android binary" ON)

set(NEWGG_ARCH "" CACHE STRING "CPU architecture: x86-64, avx2, bmi2, avx512, avxvnni, vnni512, arm64")
set_property(CACHE NEWGG_ARCH PROPERTY STRINGS
  x86-64 avx2 bmi2 avx512 avxvnni vnni512 arm64)

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

# MinGW/Ninja cannot write .obj files under Chinese source paths on Windows
# (GNU as: "can't create .../1对1/1马对1马.cpp.obj"). Compile ASCII wrappers
# that include the real sources instead.
if(WIN32 AND NOT MSVC)
  set(NEWGG_WRAP_DIR "${CMAKE_BINARY_DIR}/src_wrap")
  file(MAKE_DIRECTORY "${NEWGG_WRAP_DIR}")
  set(NEWGG_WRAP_SOURCES)
  set(NEWGG_WRAP_INDEX 0)
  foreach(SRC IN LISTS NEWGG_SOURCES)
    math(EXPR NEWGG_WRAP_INDEX "${NEWGG_WRAP_INDEX}+1")
    get_filename_component(SRC_EXT "${SRC}" EXT)
    set(WRAP_FILE "${NEWGG_WRAP_DIR}/s${NEWGG_WRAP_INDEX}${SRC_EXT}")
    file(TO_CMAKE_PATH "${SRC}" SRC_INCLUDE)
    file(WRITE "${WRAP_FILE}" "#include \"${SRC_INCLUDE}\"\n")
    list(APPEND NEWGG_WRAP_SOURCES "${WRAP_FILE}")
  endforeach()
  set(NEWGG_SOURCES "${NEWGG_WRAP_SOURCES}")
  message(STATUS "NewGG: using ${NEWGG_WRAP_INDEX} ASCII object wrappers for MinGW")
endif()

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

if(NOT NEWGG_ARCH)
  if(ANDROID)
    set(NEWGG_ARCH "arm64")
  elseif(CMAKE_OSX_ARCHITECTURES MATCHES "arm64|aarch64")
    set(NEWGG_ARCH "arm64")
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
    set(NEWGG_ARCH "arm64")
  else()
    set(NEWGG_ARCH "x86-64")
  endif()
endif()
string(TOLOWER "${NEWGG_ARCH}" NEWGG_ARCH)
string(REPLACE "x86-64-" "" NEWGG_ARCH "${NEWGG_ARCH}")
string(REPLACE "_" "-" NEWGG_ARCH "${NEWGG_ARCH}")
message(STATUS "NewGG architecture: ${NEWGG_ARCH}")

set(NEWGG_IS_MSVC_CL FALSE)
if(MSVC AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(NEWGG_IS_MSVC_CL TRUE)
endif()

if(NEWGG_IS_MSVC_CL)
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
endif()

# Pikafish-compatible x86-64 instruction-set flags.
if(NEWGG_ARCH STREQUAL "arm64")
  if(APPLE)
    target_compile_options(NewGG PRIVATE -arch arm64)
  endif()
elseif(NEWGG_IS_MSVC_CL)
  if(NEWGG_ARCH STREQUAL "avx2" OR NEWGG_ARCH STREQUAL "bmi2")
    target_compile_options(NewGG PRIVATE /arch:AVX2)
  elseif(NEWGG_ARCH STREQUAL "avxvnni")
    target_compile_options(NewGG PRIVATE /arch:AVX2)
  elseif(NEWGG_ARCH STREQUAL "avx512" OR NEWGG_ARCH STREQUAL "vnni512")
    target_compile_options(NewGG PRIVATE /arch:AVX512)
  endif()
else()
  if(NOT NEWGG_ARCH STREQUAL "arm64")
    target_compile_options(NewGG PRIVATE
      -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mpopcnt
    )
  endif()
  if(NEWGG_ARCH STREQUAL "avx2")
    target_compile_options(NewGG PRIVATE -mavx2 -mbmi)
  elseif(NEWGG_ARCH STREQUAL "bmi2")
    target_compile_options(NewGG PRIVATE -mavx2 -mbmi -mbmi2)
  elseif(NEWGG_ARCH STREQUAL "avxvnni")
    target_compile_options(NewGG PRIVATE -mavx2 -mbmi -mbmi2 -mavxvnni)
  elseif(NEWGG_ARCH STREQUAL "avx512")
    target_compile_options(NewGG PRIVATE
      -mavx2 -mbmi -mbmi2 -mavx512f -mavx512bw -mavx512dq -mavx512vl
    )
  elseif(NEWGG_ARCH STREQUAL "vnni512")
    target_compile_options(NewGG PRIVATE
      -mavx2 -mbmi -mbmi2 -mavx512f -mavx512bw -mavx512dq -mavx512vl -mavx512vnni
    )
  endif()
endif()

if(NEWGG_USE_LIBCXX AND NOT ANDROID)
  target_compile_options(NewGG PRIVATE -stdlib=libc++)
  target_link_options(NewGG PRIVATE -stdlib=libc++)
endif()

if(ANDROID AND NEWGG_ANDROID_STATIC)
  target_link_options(NewGG PRIVATE -static -Wno-unused-command-line-argument)
endif()

# MinGW/Clang on Windows otherwise needs libgcc_s_seh-1.dll, libstdc++-6.dll,
# and libwinpthread-1.dll next to the exe or on PATH.
if(WIN32 AND NOT MSVC)
  target_link_options(NewGG PRIVATE -static)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_options(NewGG PRIVATE -static-libgcc -static-libstdc++)
  endif()
endif()

find_package(Threads REQUIRED)
target_link_libraries(NewGG PRIVATE Threads::Threads)

add_custom_command(TARGET NewGG POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${NEWGG_SRC_DIR}/ggX64set2017.ini"
    "$<TARGET_FILE_DIR:NewGG>/ggX64set2017.ini"
  COMMENT "Copy ggX64set2017.ini next to NewGG"
)

install(TARGETS NewGG RUNTIME DESTINATION bin)
