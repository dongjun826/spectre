// Distributed under the MIT License.
// See LICENSE.txt for details.

#include <boost/python.hpp>

namespace py_bindings {
void bind_h5file();
void bind_h5dat();
}  // namespace py_bindings

BOOST_PYTHON_MODULE(_IOH5File) {
  Py_Initialize();
  py_bindings::bind_h5file();
  py_bindings::bind_h5dat();
}

