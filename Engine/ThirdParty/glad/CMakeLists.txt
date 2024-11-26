set(SRCROOT ${CMAKE_CURRENT_LIST_DIR})

target_include_directories(${ENGINE_TARGET} PRIVATE "${SRCROOT}/include")
target_sources(${ENGINE_TARGET} PRIVATE "${SRCROOT}/src/glad.c")
