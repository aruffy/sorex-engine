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
# set(GLM_SUBDIR "glm") message(CHECK_START "[${GLM_SUBDIR}] subdir build")
#
# set(GLM_ENABLE_CXX_20 CACHE BOOL "[${GLM_SUBDIR}] Enable C++ 20" ON)
#
# add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${GLM_SUBDIR}")
# set_target_properties( glm PROPERTIES FOLDER
# "${THIRD_PARTY_FOLDER}/${GLM_SUBDIR}" )
#
# set(GLM_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/${GLM_SUBDIR}/glm")
#
# list(APPEND THIRD_PARTY_LIBRARIES glm::glm) list(APPEND
# THIRD_PARTY_INCLUDE_DIRECTORIES "${GLM_INCLUDE_DIR}")
#
# message(CHECK_PASS "done")

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
# TODO: Keep as option
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
