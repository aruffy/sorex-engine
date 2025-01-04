cmake_minimum_required(VERSION 3.8.0)

include(BuildTools)
include(TargetPlatform)

if(TARGET_PLATFORM_LINUX)
  set(PLATFORM_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/Linux")
elseif(TARGET_PLATFORM_WINDOWS)
  set(PLATFORM_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/Win32")
else()
  message(FATAL_ERROR "${TARGET_PLATFORM} platform isn't supported")
endif()

if(PLATFORM_SOURCE_DIR)
  sorex_target_source(${ENGINE_TARGET} "${PLATFORM_SOURCE_DIR}")
  target_include_directories(
    ${ENGINE_TARGET} PUBLIC "$<BUILD_INTERFACE:${PLATFORM_SOURCE_DIR}/Include>"
  )
endif()

if(DESKTOP)
  add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Desktop")
endif()

# TODO: Detect in the TargetPlatform
set(DESKTOP TRUE)

if(DESKTOP)
  set(DESKTOP_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/Desktop")

  sorex_target_source(${ENGINE_TARGET} "${DESKTOP_SOURCE_DIR}")
  target_include_directories(
    ${ENGINE_TARGET} PUBLIC "$<BUILD_INTERFACE:${DESKTOP_SOURCE_DIR}/Include>"
  )
endif()

set(OPENGL_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/OpenGL")
sorex_target_source(${ENGINE_TARGET} "${OPENGL_SOURCE_DIR}")
target_include_directories(
  ${ENGINE_TARGET} PRIVATE "$<BUILD_INTERFACE:${OPENGL_SOURCE_DIR}/Include>"
)
