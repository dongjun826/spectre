// Distributed under the MIT License.
// See LICENSE.txt for details.

#include <hdf5.h>
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

inline tnsr::I<double, 1, Frame::Inertial> extract_point_from_coords(
    const size_t offset, const tnsr::I<DataVector, 1, Frame::Grid>& x) {
  // Extract coordinates from [[x_coord], [y_coord], [z_coord]] and
  // output[x_coord[i], y_coord[i], z_coord[i]], where i = offset
  return tnsr::I<double, 1, Frame::Inertial>{
      std::array<double, 1>{{x.get(0)[offset]}}};
}

inline tnsr::I<double, 2, Frame::Inertial> extract_point_from_coords(
    const size_t offset, const tnsr::I<DataVector, 2, Frame::Grid>& x) {
  return tnsr::I<double, 2, Frame::Inertial>{
      std::array<double, 2>{{x.get(0)[offset], x.get(1)[offset]}}};
}

inline tnsr::I<double, 3, Frame::Inertial> extract_point_from_coords(
    const size_t offset, const tnsr::I<DataVector, 3, Frame::Grid>& x) {
  return tnsr::I<double, 3, Frame::Inertial>{std::array<double, 3>{
      {x.get(0)[offset], x.get(1)[offset], x.get(2)[offset]}}};
}

int main() {
  // Initialize a scalar wave
  const double kx = -1.5;
  const double ky = -1.5;
  const double center_x = 2.4;
  const double center_y = -4.8;

  // Set up the grid
  const tnsr::I<DataVector, 2, Frame::Grid> grid_coords{
      {{DataVector{0.0, 1.0, 2.0, 0.0, 1.0, 2.0},
        DataVector{-1.0, -1.0, -1.0, 0.2, 0.2, 0.2}}}};
  const Index<2> extents{3, 2};
  const std::string element_id{"[0][0]"};

  // Find analytic solution to scalar wave
  DataVector scalar_dv(6);
  const ScalarWave::Solutions::PlaneWave<2> pw(
      {{kx, ky}}, {{center_x, center_y}},
      std::make_unique<MathFunctions::PowX>(3));
  for (double t = 0.0; t < 0.4; t = t + 0.1) {
    for (size_t i = 0; i < grid_coords.get(0).size(); ++i) {
      auto point = extract_point_from_coords(i, grid_coords);
      scalar_dv[i] = pw.psi(point, t).get();
    }
    Scalar<DataVector> scalar{scalar_dv};
    std::unordered_map<std::string, std::pair<std::vector<std::string>,
                                              std::vector<DataVector>>>
        detyped_tensors;
    detyped_tensors.emplace("scalar"s, scalar.get_vector_of_data());

    // Write analytic solutions to .h5 and .xdmf files
    std::stringstream directory;
    directory << "Step_" << std::setfill('0') << std::setw(8)
              << static_cast<size_t>(t * 1000.0 + 0.5) << "_00000000";
    file_system::create_directory(directory.str());
    std::stringstream file_name;
    file_name << directory.str() << "/ScalarWave2D.h5";
    vis::VolumeFile output(file_name.str(), 2);
    output.write_xdmf_time(t);
    output.write_element_connectivity_and_coordinates(t, grid_coords, extents,
                                                      element_id);
    output.write_element_data(detyped_tensors, extents, element_id);
  }
}
