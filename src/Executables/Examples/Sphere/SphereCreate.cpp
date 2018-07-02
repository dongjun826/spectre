// Distributed under the MIT License.
// See LICENSE.txt for details.

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "DataStructures/DataVector.hpp"
#include "DataStructures/Index.hpp"
#include "DataStructures/Tensor/Tensor.hpp"
#include "Domain/Block.hpp"
#include "Domain/BlockNeighbor.hpp"
#include "Domain/CoordinateMaps/Affine.hpp"
#include "Domain/CoordinateMaps/CoordinateMap.hpp"
#include "Domain/CoordinateMaps/Equiangular.hpp"
#include "Domain/CoordinateMaps/ProductMaps.hpp"
#include "Domain/CoordinateMaps/Wedge3D.hpp"
#include "Domain/Domain.hpp"
#include "Domain/DomainCreators/DomainCreator.hpp"
#include "Domain/DomainCreators/Sphere.hpp"
#include "Domain/LogicalCoordinates.hpp"
#include "IO/Connectivity.hpp"
#include "IO/VolumeDataFile.hpp"
#include "PointwiseFunctions/AnalyticSolutions/WaveEquation/PlaneWave.hpp"
#include "PointwiseFunctions/MathFunctions/MathFunction.hpp"
#include "PointwiseFunctions/MathFunctions/PowX.hpp"
#include "Utilities/ConstantExpressions.hpp"
#include "Utilities/FileSystem.hpp"
#include "Utilities/MakeArray.hpp"

extern "C" void CkRegisterMainModule(void) {}

int main() {

  // Set up domain
  const double inner_radius = 0.5;
  const double outer_radius = 4.0;
  const size_t refinement = 2;
  const std::array<size_t, 2> grid_points_r_angular{{4, 4}};
  const DomainCreators::Sphere<Frame::Inertial>& sphere{
      inner_radius, outer_radius, refinement, grid_points_r_angular, true};
  const auto domain = sphere.create_domain();

  // Initialize a scalar wave
  const double kx = -1.0;
  const double ky = -1.0;
  const double kz = -1.0;
  const double center_x = 0.0;
  const double center_y = 0.0;
  const double center_z = 0.0;

  // Acquire analytic solution to scalar wave
  const ScalarWave::Solutions::PlaneWave<3> pw(
    {{kx, ky, kz}}, {{center_x, center_y, center_z}},
    std::make_unique<MathFunctions::PowX>(3));

  for (double t = 0.0; t < 1.0; t = t + 0.1) {
    // Set up .h5 and .xdmf files
    std::stringstream directory;
    directory << "VolumeData_" << std::setfill('0') << std::setw(8)
              << static_cast<size_t>(t * 1000.0 + 0.5) << "_00000000";
    file_system::create_directory(directory.str());
    std::stringstream file_name;
    file_name << directory.str() << "/Node0.h5";
    vis::VolumeFile output(file_name.str(), 2);
    output.write_xdmf_time(t);

    // Find analytic solution for each block
    for (size_t i = 0; i < domain.blocks().size(); ++i) {
      const Index<3> extents(Index<3>(10, 10, 10));
      std::stringstream element_id;
      element_id << "[0][" << i << "]";
      const auto logical_coords = logical_coordinates(extents);
      const auto grid_coords =
          domain.blocks()[i].coordinate_map()(logical_coords);
      const tnsr::I<DataVector, 3> coord{std::array<DataVector, 3>{
          {grid_coords[0], grid_coords[1], grid_coords[2]}}};
      const Scalar<DataVector> scalar{pw.psi(coord, t).get()};
      std::unordered_map<std::string, std::pair<std::vector<std::string>,
                                                std::vector<DataVector>>>
          detyped_tensors;
      detyped_tensors.emplace("scalar"s, scalar.get_vector_of_data());

      // Write analytic solutions to .h5 and .xdmf files
      output.write_element_connectivity_and_coordinates(t, coord, extents,
                                                        element_id.str());
      output.write_element_data(detyped_tensors, extents, element_id.str());
    }
  }
}
