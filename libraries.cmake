if (DEFINED LIBRARIES_DIR)
  return()
endif()

set(LIBRARIES_DIR "${PROJECT_SOURCE_DIR}/libraries")
message("-- libraries: ${LIBRARIES_DIR}")

message("-- fmt")
add_subdirectory(${LIBRARIES_DIR}/fmt)

message("-- czmq")
add_subdirectory(${LIBRARIES_DIR}/czmq)

message("-- SDL2")
add_subdirectory(${LIBRARIES_DIR}/SDL)

# message("-- spdlog")
# add_subdirectory(${LIBRARIES_DIR}/spdlog)


