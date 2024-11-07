cmake_minimum_required(VERSION 3.8.0)

set(THIRD_PARTY_FOLDER "ThirdParty")

# Skip libraries installation
set(SKIP_INSTALL_ALL ON)

# ##############################################################################
# SPDLOG library
# ##############################################################################
set(SPDLOG_SUBDIR "spdlog")
message(STATUS "[${SPDLOG_SUBDIR}] subdir build")

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/${SPDLOG_SUBDIR}")
set_target_properties(
  spdlog PROPERTIES FOLDER "${THIRD_PARTY_FOLDER}/${SPDLOG_SUBDIR}"
)

list(APPEND THIRD_PARTY_PUBLIC_LIBS spdlog::spdlog)
get_target_property(
  SPDLOG_LIBRARY_INCLUDE_DIRS spdlog::spdlog INCLUDE_DIRECTORIES
)

# target_include_directories(${ENGINE_TARGET} PUBLIC
# ${SPDLOG_LIBRARY_INCLUDE_DIRS})
# ##############################################################################

set(ENGINE_THIRD_PARTY_INCLUDE_DIRECTORIES
    ${THIRD_PARTY_INCLUDE_DIRECTORIES}
    PARENT_SCOPE
)
set(ENGINE_THIRD_PARTY_PUBLIC_LIBS
    ${THIRD_PARTY_PUBLIC_LIBS}
    PARENT_SCOPE
)
