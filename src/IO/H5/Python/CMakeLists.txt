# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "PyIOH5File")

spectre_python_add_module(
  IOH5File
  SOURCES Bindings.cpp Dat.cpp File.cpp
  MODULE_PATH "IO/H5/"
  )

target_link_libraries(
  ${LIBRARY}
  PUBLIC IO
  ${SPECTRE_LINK_PYBINDINGS}
  )

set(LIBRARY "PyIOH5Dat")

spectre_python_add_module(
  IOH5Dat
  SOURCES Bindings.cpp Dat.cpp File.cpp
  MODULE_PATH "IO/H5/"
  )

target_link_libraries(
  ${LIBRARY}
  PUBLIC IO
  ${SPECTRE_LINK_PYBINDINGS}
  )
