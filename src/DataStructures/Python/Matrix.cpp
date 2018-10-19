// Distributed under the MIT License.
// See LICENSE.txt for details.

#include <boost/python.hpp>
#include <sstream>
#include <string>
#include <utility>

#include "DataStructures/Matrix.hpp"
#include "PythonBindings/BoundChecks.hpp"
#include <boost/python/reference_existing_object.hpp>
#include <boost/python/return_value_policy.hpp>
#include <boost/python/tuple.hpp>
namespace bp = boost::python;

namespace py_bindings {
void bind_matrix() {
  // Wrapper for basic Matrix operations
  bp::class_<Matrix>("Matrix", bp::init<size_t, size_t>())
    .def("size",
         +[](const Matrix& M){
           bp::tuple a= bp::make_tuple<size_t,size_t>(M.rows(), M.columns());
           return a;
         })
    // __getitem__ and __setitem__ are the subscript operators (M[*,*]).
    // To define (and overload) operator() use __call__
    .def("__getitem__",
         +[] (const Matrix& M, bp::tuple x) -> const double   {
           return M(bp::extract<size_t>(x[0]), bp::extract<size_t>(x[1]));
         }) //bp::return_value_policy<bp::reference_existing_object>())
    // Need __str__ for converting to string/printing
    .def("__str__",
         +[](const Matrix& M) {
           std::stringstream ss;
           ss << M;
           return ss.str();
         })
    // Will Check to see if this can dynamically resize the matrix
    .def("__setitem__",
         +[] (Matrix& M, bp::tuple x, const double val){
           M(bp::extract<size_t>(x[0]),bp::extract<size_t>(x[1])) = val;
         });


}
}
