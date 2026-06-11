cmake_minimum_required(VERSION 3.12.0)

set(THIRD_PARTY_FOLDER "ThirdParty")

cmake_policy(SET CMP0077 NEW)

# Skip libraries installation
set(SKIP_INSTALL_ALL ON)
set(THIRD_PARTY_LIBRARIES "")

# ##############################################################################
# SPDLOG library
# ##############################################################################
set(SPDLOG_SUBDIR "spdlog")

message(STATUS "[${SPDLOG_SUBDIR}] subdir build")
if(NOT SOREX_STATIC_LIBRARY)
  message(VERBOSE "[ThirdParty] spdlog enable fPIC compile flag")
  set(SPDLOG_BUILD_PIC CACHE BOOL "[SPDLOG] Build position independent code" ON)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${SPDLOG_SUBDIR}")
set_target_properties(
  spdlog PROPERTIES FOLDER "${THIRD_PARTY_FOLDER}/${SPDLOG_SUBDIR}"
)

list(APPEND THIRD_PARTY_PUBLIC_LIBS spdlog::spdlog)
get_target_property(
  SPDLOG_LIBRARY_INCLUDE_DIRS spdlog::spdlog INCLUDE_DIRECTORIES
)

# ##############################################################################
# GLM library
# ##############################################################################
set(GLM_SUBDIR "glm")
message(CHECK_START "[${GLM_SUBDIR}] subdir build")

set(GLM_ENABLE_CXX_20 CACHE BOOL "[${GLM_SUBDIR}] Enable C++ 20" ON)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${GLM_SUBDIR}")
set_target_properties(
  glm PROPERTIES FOLDER "${THIRD_PARTY_FOLDER}/${GLM_SUBDIR}"
)

set(GLM_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/${GLM_SUBDIR}/glm")

list(APPEND THIRD_PARTY_LIBRARIES glm::glm)
target_include_directories(${ENGINE_TARGET} PRIVATE "${GLM_INCLUDE_DIR}")

message(CHECK_PASS "done")

# ##############################################################################
# GLAD library
# ##############################################################################
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/glad")

# ##############################################################################
# GLFW library
# ##############################################################################
set(GLFW_SUBDIR "glfw")

set(GLFW_BUILD_EXAMPLES
    OFF
    CACHE BOOL "Build the GLFW example programs"
)
set(GLFW_BUILD_TESTS
    OFF
    CACHE BOOL "Build the GLFW test programs"
)
set(GLFW_BUILD_DOCS
    OFF
    CACHE BOOL "Build the GLFW documentation"
)
set(GLFW_INSTALL
    OFF
    CACHE BOOL "Generate installation target"
)
# TODO: Use Wayland: a valgrind output for the glfw 3.5.0 reports memory leaks.
# Need to investigate the problem: `glfwCreateWindow`;

# Force to use X11, Debian: `apt-get install xorg-dev` @see GLFW Documentation
set(GLFW_BUILD_WAYLAND
    ON
    CACHE BOOL "Build support for Wayland"
)
set(GLFW_BUILD_X11
    OFF
    CACHE BOOL "Build support for X11"
)

message(CHECK_START "[${GLFW_SUBDIR}] subdir build")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${GLFW_SUBDIR}")
set_target_properties(
  glfw PROPERTIES FOLDER "${THIRD_PARTY_FOLDER}/${GLFW_SUBDIR}"
)
set_target_properties(
  update_mappings PROPERTIES FOLDER "${THIRD_PARTY_FOLDER}/${GLFW_SUBDIR}"
)

set(GLFW_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/${GLFW_SUBDIR}/include")
target_include_directories(${ENGINE_TARGET} PRIVATE "${GLFW_INCLUDE_DIR}")

list(APPEND THIRD_PARTY_LIBRARIES glfw)
message(CHECK_PASS "done")

# ##############################################################################
# stblib https://github.com/nothings/stb
# ##############################################################################
set(STB_SUBDIR "stb")

message(STATUS "[${STB_SUBDIR}] library headers included")

target_include_directories(${ENGINE_TARGET}
    PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${STB_SUBDIR}>"
)

# ##############################################################################
# tinyxml2
# ##############################################################################
set(TINYXML2_SUBDIR "tinyxml2")

message(CHECK_START "[${TINYXML2_SUBDIR}] subdir build")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${TINYXML2_SUBDIR}")
target_include_directories(${ENGINE_TARGET}
    PRIVATE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${TINYXML2_SUBDIR}>"
)
set_target_properties(
  tinyxml2 PROPERTIES FOLDER "${THIRD_PARTY_FOLDER}/${TINYXML2_SUBDIR}"
)

list(APPEND THIRD_PARTY_LIBRARIES tinyxml2)
message(CHECK_PASS "done")

# ##############################################################################
set(ENGINE_THIRD_PARTY_INCLUDE_DIRECTORIES
    ${THIRD_PARTY_INCLUDE_DIRECTORIES}
    PARENT_SCOPE
)
set(ENGINE_THIRD_PARTY_PUBLIC_LIBS
    ${THIRD_PARTY_PUBLIC_LIBS}
    PARENT_SCOPE
)
set(ENGINE_THIRD_PARTY_LIBRARIES
    ${THIRD_PARTY_LIBRARIES}
    PARENT_SCOPE
)
