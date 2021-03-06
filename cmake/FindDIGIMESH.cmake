find_path(DIGIMESH_INCLUDE_DIR digimesh/digimesh.h
  PATHS "${CMAKE_CURRENT_LIST_DIR}/../include"
  PATH_SUFFIXES digimesh)
set(DIGIMESH_INCLUDE_DIRS ${DIGIMESH_INCLUDE_DIR})

find_library(DIGIMESH_LIBRARY NAMES digimesh
  PATHS "${CMAKE_CURRENT_LIST_DIR}/../lib")
set(DIGIMESH_LIBRARIES ${DIGIMESH_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DIGIMESH
  DEFAULT_MSG
  DIGIMESH_LIBRARY
  DIGIMESH_INCLUDE_DIR)

mark_as_advanced(DIGIMESH_INCLUDE_DIR DIGIMESH_LIBRARY)
