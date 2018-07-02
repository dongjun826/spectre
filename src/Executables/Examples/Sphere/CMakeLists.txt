
#Distributed under the MIT License.
#See LICENSE.txt for details.

set(EXECUTABLE SphereCreate)

    add_executable(${EXECUTABLE} EXCLUDE_FROM_ALL SphereCreate.cpp)

        target_link_libraries(
            ${EXECUTABLE} CoordinateMaps DataStructures Domain DomainCreators IO
                MathFunctions Utilities WaveEquation ${SPECTRE_LIBRARIES})

            set_target_properties(${EXECUTABLE} PROPERTIES LINK_FLAGS
                                  "-nomain-module -nomain")
