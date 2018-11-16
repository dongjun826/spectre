// Distributed under the MIT License.
// See LICENSE.txt for details.

#include <hdf5.h>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "boost/python/stl_iterator.hpp"

#include "DataStructures/Matrix.hpp"
#include "IO/H5/Dat.hpp"
#include "IO/H5/Header.hpp"
#include "IO/H5/OpenGroup.hpp"
#include "IO/H5/Version.hpp"

namespace bp = boost::python;

template <typename T>
inline std::vector<T> py_list_to_std_vector(const bp::object& iterable) {
  return std::vector<T>(bp::stl_input_iterator<T>(iterable),
                        bp::stl_input_iterator<T>());
}

template <class T>
inline bp::list std_vector_to_py_list(std::vector<T> vector) {
  typename std::vector<T>::iterator iter;
  bp::list list;
  for (iter = vector.begin(); iter != vector.end(); ++iter) {
    list.append(*iter);
  }
  return list;
}

namespace py_bindings {
void bind_h5dat() {
  // Wrapper for basic H5Dat operations
  bp::class_<h5::Dat, boost::noncopyable>("H5Dat", bp::no_init)
      .def("append",
           +[](h5::Dat& D, const bp::list& data) {
             D.append(py_list_to_std_vector<double>(data));
           })
      .def("append", +[](h5::Dat& D, const Matrix& data) { D.append(data); })
      .def("get_legend",
           +[](h5::Dat& D) -> bp::list {
             return std_vector_to_py_list<std::string>(D.get_legend());
           })
      .def("get_data", +[](h5::Dat& D) -> Matrix { return D.get_data(); })
      .def("get_data_subset",
           +[](h5::Dat& D, bp::list& columns, size_t first_row = 0,
               size_t num_rows = 1) -> Matrix {
             return D.get_data_subset(py_list_to_std_vector<size_t>(columns),
                                      first_row, num_rows);
           })
      .def("get_dimensions",
           +[](h5::Dat& D) -> bp::list {
             std::array<hsize_t, 2> dimension = D.get_dimensions();
             bp::list dim;
             dim.append(dimension[0]);
             dim.append(dimension[1]);
             return dim;
           })
      .def("get_header", +[](h5::Dat& D) { return D.get_header(); })
      .def("get_version", +[](h5::Dat& D) { return D.get_version(); });
}
}  // namespace py_bindings
