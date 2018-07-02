#Distributed under the MIT License.
#See LICENSE.txt for details.

set(EXECUTABLE XdmfOutput2)

    add_executable(${EXECUTABLE} EXCLUDE_FROM_ALL XdmfOutput2D.cpp)

        target_link_libraries(${EXECUTABLE} DataStructures IO MathFunctions
                                  Utilities WaveEquation ${SPECTRE_LIBRARIES})

            set_target_properties(${EXECUTABLE} PROPERTIES LINK_FLAGS
                                  "-nomain-module -nomain")

                set(EXECUTABLE ApplyObservers)

                    add_executable(${
                        EXECUTABLE} EXCLUDE_FROM_ALL ApplyObservers.cpp)

                        target_link_libraries(${
                            EXECUTABLE} ErrorHandling Utilities ${
                            SPECTRE_LIBRARIES})

                            set_target_properties(${
                                EXECUTABLE} PROPERTIES LINK_FLAGS
                                                  "-nomain-module -nomain")
