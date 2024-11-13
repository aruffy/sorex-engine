cmake_minimum_required(VERSION 3.8.0)

include(BuildTools)
include(TargetPlatform)
include(GenerateExportHeader)

if(WIN32)
  set(ENGINE_TARGET "Sorex")
else()
  set(ENGINE_TARGET "sorex")
endif()

option(SOREX_STATIC_LIBRARY "Compile Sorex engine as static library" OFF)
option(SRXENG_ADDRESS_SANITIZER "Enable address sanitizer for engine code" OFF)
option(SRXENG_BUILD_PIC "Build position independent code (-fPIC)" OFF)

if(SRXENG_BUILD_PIC)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORTS_H "${CMAKE_CURRENT_BINARY_DIR}/CMakeSorexExports.h")

if(SOREX_STATIC_LIBRARY)
  add_library(${ENGINE_TARGET} STATIC ${CMAKE_EXPORTS_H})
  target_compile_definitions(${ENGINE_TARGET} PUBLIC SOREX_STATIC_LIB)
else()
  add_library(${ENGINE_TARGET} SHARED ${CMAKE_EXPORTS_H})
  target_compile_definitions(${ENGINE_TARGET} PUBLIC SOREX_EXPORT_API)
endif()

add_library(Sorex::Sorex ALIAS ${ENGINE_TARGET})
set_target_properties(${ENGINE_TARGET} PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(${ENGINE_TARGET} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

generate_export_header(
  ${ENGINE_TARGET}
  EXPORT_MACRO_NAME
  SRX_API
  EXPORT_FILE_NAME
  ${CMAKE_EXPORTS_H}
  STATIC_DEFINE
  SOREX_STATIC_LIB
)
# ##############################################################################
# Compile Options
# ##############################################################################
sorex_target_compile_options(${ENGINE_TARGET})
get_target_property(compile_flags ${ENGINE_TARGET} COMPILE_OPTIONS)

message(STATUS "[${ENGINE_TARGET}]  Compile options: ${compile_flags}")

# ##############################################################################
# Preprocessor Definitions
# ##############################################################################
sorex_target_compile_definitions(${ENGINE_TARGET})
if(SRXENG_ADDRESS_SANITIZER)
  message(STATUS "[${ENGINE_TARGET}] Enable AddressSanitizer")

  if(MSVC)
    target_compile_options(${ENGINE_TARGET} PUBLIC /fsanitize=address)
    target_compile_definitions(
      ${ENGINE_TARGET} PUBLIC _DISABLE_VECTOR_ANNOTATION
    )
    target_compile_definitions(
      ${ENGINE_TARGET} PUBLIC _DISABLE_STRING_ANNOTATION
    )
  else()
    target_compile_options(${ENGINE_TARGET} PUBLIC -fsanitize=address)
    target_compile_options(${ENGINE_TARGET} PUBLIC -fsanitize=undefined)
    target_link_options(${ENGINE_TARGET} PUBLIC -fsanitize=address)
    target_link_options(${ENGINE_TARGET} PUBLIC -fsanitize=undefined)
  endif()
endif()

target_precompile_headers(
  ${ENGINE_TARGET} PUBLIC
  "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_SOURCE_DIR}/Precompiled/Precompiled.h>"
)

target_include_directories(
  ${ENGINE_TARGET} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/Include"
)
target_include_directories(
  ${ENGINE_TARGET} PRIVATE "${CMAKE_CURRENT_LIST_DIR}/Source"
)

sorex_target_source(${ENGINE_TARGET} "${CMAKE_CURRENT_LIST_DIR}/Source")
sorex_target_source(${ENGINE_TARGET} "${CMAKE_CURRENT_LIST_DIR}/Include")

# ##############################################################################
# Platform Code
# ##############################################################################

# ##############################################################################
# Include ThirdParty Libraries
# ##############################################################################
set(THIRD_PARTY_LIBS_PATH "${CMAKE_CURRENT_LIST_DIR}/ThirdParty")
set(ENGINE_THIRD_PARTY_PUBLIC_LIBS "")

add_subdirectory(${THIRD_PARTY_LIBS_PATH})
message(
  STATUS
    "[${ENGINE_TARGET}] third-party public libs: ${ENGINE_THIRD_PARTY_PUBLIC_LIBS}"
)

target_link_libraries(${ENGINE_TARGET} PUBLIC ${ENGINE_THIRD_PARTY_PUBLIC_LIBS})

# ##############################################################################
# Create IDE source tree
# ##############################################################################
get_property(
  src_files
  TARGET ${ENGINE_TARGET}
  PROPERTY SOURCES
)

# source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${src_files})
