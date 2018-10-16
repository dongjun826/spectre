// Distributed under the MIT License.
// See LICENSE.txt for details.

#include <hdf5.h>
#include <boost/python.hpp>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "DataStructures/Matrix.hpp"
#include "IO/H5/Dat.hpp"
#include "IO/H5/Header.hpp"
#include "IO/H5/OpenGroup.hpp"
#include "IO/H5/Version.hpp"

namespace bp = boost::python;

namespace py_bindings {
void bind_h5dat() {
  // Wrapper for basice H5Dat operations
  bp::class_<h5::Dat, boost::noncopyable>("H5Dat", bp::no_init)

      .def("append",
           +[](h5::Dat& D, const std::vector<double>& data) { D.append(data); })
      .def("append",
           +[](h5::Dat& D, const std::vector<std::vector<double>>& data) {
             D.append(data);
           })
      .def("append", +[](h5::Dat& D, const Matrix& data) { D.append(data); })
      .def("get_legend", +[](h5::Dat& D) { return D.get_legend(); })
      .def("get_data", +[](h5::Dat& D) { return D.get_data(); })
      .def("get_data_subset",
           +[](h5::Dat& D, const std::vector<size_t>& columns,
               size_t first_row = 0, size_t num_rows = 1) {
             return D.get_data_subset(columns, first_row, num_rows);
           })
      .def("get_dimensions", +[](h5::Dat& D) { return D.get_dimensions(); })
      .def("get_header", +[](h5::Dat& D) { return D.get_header(); })
      .def("get_version", +[](h5::Dat& D) { return D.get_version(); });
}
}  // namespace py_bindings
