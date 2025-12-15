# FindDK2NU.cmake
#
# Tries to locate a dk2nu installation that provides:
#   include/dk2nu/tree/dk2nu.h
#   lib/libdk2nuTree.(so|dylib|a)
#
# Provides:
#   DK2NU_FOUND
#   DK2NU_INCLUDE_DIRS
#   DK2NU_TREE_LIBRARY
#   DK2NU::Tree (imported target)

# Hints:
set(_dk2nu_hints "")
if(DEFINED DK2NU_ROOT)
  list(APPEND _dk2nu_hints "${DK2NU_ROOT}")
endif()
if(DEFINED CMAKE_PREFIX_PATH)
  foreach(_p ${CMAKE_PREFIX_PATH})
    list(APPEND _dk2nu_hints "${_p}")
  endforeach()
endif()

# Include and library search
find_path(DK2NU_INCLUDE_DIRS
  NAMES dk2nu/tree/dk2nu.h
  HINTS ${_dk2nu_hints}
  PATH_SUFFIXES include
)

find_library(DK2NU_TREE_LIBRARY
  NAMES dk2nuTree
  HINTS ${_dk2nu_hints}
  PATH_SUFFIXES lib lib64
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(DK2NU
  REQUIRED_VARS DK2NU_INCLUDE_DIRS DK2NU_TREE_LIBRARY
)

if(DK2NU_FOUND AND NOT TARGET DK2NU::Tree)
  add_library(DK2NU::Tree UNKNOWN IMPORTED)
  set_target_properties(DK2NU::Tree PROPERTIES
    IMPORTED_LOCATION "${DK2NU_TREE_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${DK2NU_INCLUDE_DIRS}"
  )
endif()

mark_as_advanced(DK2NU_INCLUDE_DIRS DK2NU_TREE_LIBRARY)

