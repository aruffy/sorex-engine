set(SRCROOT ${CMAKE_CURRENT_LIST_DIR})

#TODO PRIVATE
target_include_directories(${ENGINE_TARGET} PUBLIC "${SRCROOT}/include")
target_sources(${ENGINE_TARGET} PRIVATE "${SRCROOT}/src/glad.c")