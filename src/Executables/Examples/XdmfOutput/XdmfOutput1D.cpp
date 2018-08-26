// Distributed under the MIT License.
// See LICENSE.txt for details.

#include <hdf5.h>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "DataStructures/DataBox/Prefixes.hpp"  // IWYU pragma: keep
#include "DataStructures/DataVector.hpp"
#include "DataStructures/Index.hpp"
#include "DataStructures/Matrix.hpp"
#include "DataStructures/Tensor/Tensor.hpp"
#include "DataStructures/Variables.hpp"           // IWYU pragma: keep
#include "Evolution/Systems/ScalarWave/Tags.hpp"  // IWYU pragma: keep
#include "IO/Connectivity.hpp"
#include "IO/VolumeDataFile.hpp"
#include "PointwiseFunctions/AnalyticSolutions/WaveEquation/PlaneWave.hpp"
#include "PointwiseFunctions/MathFunctions/MathFunction.hpp"
#include "PointwiseFunctions/MathFunctions/PowX.hpp"
#include "Utilities/ConstantExpressions.hpp"
#include "Utilities/FileSystem.hpp"
#include "Utilities/Gsl.hpp"
#include "Utilities/TMPL.hpp"

extern "C" void CkRegisterMainModule(void) {}

tnsr::I<double, 1, Frame::Inertial> extract_point_from_coords(
    const size_t offset, const tnsr::I<DataVector, 1>& x) {
  // Extract coordinates from [[x_coord], [y_coord], [z_coord]] and
  // output[x_coord[i], y_coord[i], z_coord[i]], where i = offset
  return tnsr::I<double, 1, Frame::Inertial>{
      std::array<double, 1>{{x.get(0)[offset]}}};
}

int main() {
  // Initialize a scalar wave
  const double kx = -1.5;
  const double center_x = 2.4;
  // const double omega = std::abs(kx);
  const double t = 3.1;

  // Set up the grid
  const tnsr::I<DataVector, 1, Frame::Grid> grid_coords(
      DataVector{0.0, 1.0, 2.0});
  const Index<1> extents{3};
  const std::string element_id{"[0][0]"};

  // Find analytic solution to scalar wave
  DataVector scalar_dv(3);
  const ScalarWave::Solutions::PlaneWave<1> pw(
      {{kx}}, {{center_x}}, std::make_unique<MathFunctions::PowX>(3));
  for (size_t i = 0; i < grid_coords.get(0).size(); ++i) {
    tnsr::I<double, 1, Frame::Inertial> point{
        std::array<double, 1>{{grid_coords.get(0)[i]}}};
    scalar_dv[i] = pw.psi(point, t).get();
  }
  const Scalar<DataVector> scalar{scalar_dv};
  std::unordered_map<
      std::string, std::pair<std::vector<std::string>, std::vector<DataVector>>>
      detyped_tensors;
  detyped_tensors.emplace("scalar"s, scalar.get_vector_of_data());

  // Write analytic solutions to .h5 and .xdmf files
  vis::VolumeFile my_file("ScalarWave1D.h5", 2, false);
  my_file.write_xdmf_time(t);
  my_file.write_element_connectivity_and_coordinates(t, grid_coords, extents,
                                                     element_id);
  my_file.write_element_data(detyped_tensors, extents, element_id);
}
