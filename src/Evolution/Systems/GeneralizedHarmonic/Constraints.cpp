// Distributed under the MIT License.
// See LICENSE.txt for details.

#include "Evolution/Systems/GeneralizedHarmonic/Constraints.hpp"

#include <cstddef>

#include "DataStructures/Tensor/Tensor.hpp"  // IWYU pragma: keep
#include "Utilities/GenerateInstantiations.hpp"
#include "Utilities/Gsl.hpp"
#include "Utilities/MakeWithValue.hpp"

namespace {
// Functions to compute generalized-harmonic 2-index constraint, where
// function arguments are in the order that each quantity first
// appears in the corresponding term in Eq. (44) of
// http://arXiv.org/abs/gr-qc/0512093v3
template <size_t SpatialDim, typename Frame, typename DataType>
void two_index_constraint_add_term_1_of_11(
    const gsl::not_null<tnsr::ia<DataType, SpatialDim, Frame>*> constraint,
    const tnsr::II<DataType, SpatialDim, Frame>& inverse_spatial_metric,
    const tnsr::ijaa<DataType, SpatialDim, Frame>& d_phi) noexcept {
  for (size_t i = 0; i < SpatialDim; ++i) {
    for (size_t a = 0; a < SpatialDim + 1; ++a) {
      for (size_t j = 0; j < SpatialDim; ++j) {
        for (size_t k = 0; k < SpatialDim; ++k) {
          constraint->get(i, a) +=
              inverse_spatial_metric.get(j, k) * d_phi.get(j, i, k + 1, a);
        }
      }
    }
  }
}

template <size_t SpatialDim, typename Frame, typename DataType>
void two_index_constraint_add_term_2_of_11(
    const gsl::not_null<tnsr::ia<DataType, SpatialDim, Frame>*> constraint,
    const tnsr::A<DataType, SpatialDim, Frame>& spacetime_normal_vector,
    const tnsr::a<DataType, SpatialDim, Frame>& spacetime_normal_one_form,
    const tnsr::AA<DataType, SpatialDim, Frame>& inverse_spacetime_metric,
    const tnsr::ijaa<DataType, SpatialDim, Frame>& d_phi) noexcept {
  for (size_t i = 0; i < SpatialDim; ++i) {
    for (size_t a = 0; a < SpatialDim + 1; ++a) {
      for (size_t c = 0; c < SpatialDim + 1; ++c) {
        for (size_t d = 0; d < SpatialDim + 1; ++d) {
          if (a > 0) {
            constraint->get(i, a) -= 0.5 * inverse_spacetime_metric.get(c, d) *
                                     d_phi.get(a - 1, i, c, d);
          }
          for (size_t j = 0; j < SpatialDim; ++j) {
            constraint->get(i, a) -= 0.5 * spacetime_normal_vector.get(j + 1) *
                                     spacetime_normal_one_form.get(a) *
                                     inverse_spacetime_metric.get(c, d) *
                                     d_phi.get(j, i, c, d);
          }
        }
      }
    }
  }
}

template <size_t SpatialDim, typename Frame, typename DataType>
void two_index_constraint_add_term_3_of_11(
    const gsl::not_null<tnsr::ia<DataType, SpatialDim, Frame>*> constraint,
    const tnsr::A<DataType, SpatialDim, Frame>& spacetime_normal_vector,
    const tnsr::iaa<DataType, SpatialDim, Frame>& d_pi) noexcept {
  for (size_t i = 0; i < SpatialDim; ++i) {
    for (size_t a = 0; a < SpatialDim + 1; ++a) {
      for (size_t b = 0; b < SpatialDim + 1; ++b) {
        constraint->get(i, a) +=
            spacetime_normal_vector.get(b) * d_pi.get(i, b, a);
      }
    }
  }
}

template <size_t SpatialDim, typename Frame, typename DataType>
void two_index_constraint_add_term_4_of_11(
    const gsl::not_null<tnsr::ia<DataType, SpatialDim, Frame>*> constraint,
    const tnsr::a<DataType, SpatialDim, Frame>& spacetime_normal_one_form,
    const tnsr::AA<DataType, SpatialDim, Frame>& inverse_spacetime_metric,
    const tnsr::iaa<DataType, SpatialDim, Frame>& d_pi) noexcept {
  for (size_t i = 0; i < SpatialDim; ++i) {
    for (size_t a = 0; a < SpatialDim + 1; ++a) {
      for (size_t c = 0; c < SpatialDim + 1; ++c) {
        for (size_t d = 0; d < SpatialDim + 1; ++d) {
          constraint->get(i, a) -= 0.5 * spacetime_normal_one_form.get(a) *
                                   inverse_spacetime_metric.get(c, d) *
                                   d_pi.get(i, c, d);
        }
      }
    }
  }
}

template <size_t SpatialDim, typename Frame, typename DataType>
void two_index_constraint_add_term_5_of_11(
    const gsl::not_null<tnsr::ia<DataType, SpatialDim, Frame>*> constraint,
    const tnsr::ia<DataType, SpatialDim, Frame>& d_gauge_function) noexcept {
  for (size_t i = 0; i < SpatialDim; ++i) {
    for (size_t a = 0; a < SpatialDim + 1; ++a) {
      constraint->get(i, a) += d_gauge_function.get(i, a);
    }
  }
}

template <size_t SpatialDim, typename Frame, typename DataType>
void two_index_constraint_add_term_6_of_11(
    const gsl::not_null<tnsr::ia<DataType, SpatialDim, Frame>*> constraint,
    const tnsr::A<DataType, SpatialDim, Frame>& spacetime_normal_vector,
    const tnsr::a<DataType, SpatialDim, Frame>& spacetime_normal_one_form,
    const tnsr::iaa<DataType, SpatialDim, Frame>& phi,
    const tnsr::AA<DataType, SpatialDim, Frame>&
        inverse_spacetime_metric) noexcept {
  for (size_t i = 0; i < SpatialDim; ++i) {
    for (size_t a = 0; a < SpatialDim + 1; ++a) {
      for (size_t c = 0; c < SpatialDim + 1; ++c) {
        for (size_t d = 0; d < SpatialDim + 1; ++d) {
          for (size_t e = 0; e < SpatialDim + 1; ++e) {
            for (size_t f = 0; f < SpatialDim + 1; ++f) {
              if (a > 0) {
                constraint->get(i, a) += 0.5 * phi.get(a - 1, c, d) *
                                         phi.get(i, e, f) *
                                         inverse_spacetime_metric.get(c, e) *
                                         inverse_spacetime_metric.get(d, f);
              }
              for (size_t j = 0; j < SpatialDim; ++j) {
                constraint->get(i, a) +=
                    0.5 * spacetime_normal_vector.get(j + 1) *
                    spacetime_normal_one_form.get(a) * phi.get(j, c, d) *
                    phi.get(i, e, f) * inverse_spacetime_metric.get(c, e) *
                    inverse_spacetime_metric.get(d, f);
              }
            }
          }
        }
      }
    }
  }
}

template <size_t SpatialDim, typename Frame, typename DataType>
void two_index_constraint_add_term_7_of_11(
    const gsl::not_null<tnsr::ia<DataType, SpatialDim, Frame>*> constraint,
    const tnsr::II<DataType, SpatialDim, Frame>& inverse_spatial_metric,
    const tnsr::iaa<DataType, SpatialDim, Frame>& phi,
    const tnsr::AA<DataType, SpatialDim, Frame>& inverse_spacetime_metric,
    const tnsr::A<DataType, SpatialDim, Frame>& spacetime_normal_vector,
    const tnsr::a<DataType, SpatialDim, Frame>&
        spacetime_normal_one_form) noexcept {
  for (size_t i = 0; i < SpatialDim; ++i) {
    for (size_t a = 0; a < SpatialDim + 1; ++a) {
      for (size_t j = 0; j < SpatialDim; ++j) {
        for (size_t k = 0; k < SpatialDim; ++k) {
          for (size_t c = 0; c < SpatialDim + 1; ++c) {
            for (size_t d = 0; d < SpatialDim + 1; ++d) {
              for (size_t e = 0; e < SpatialDim + 1; ++e) {
                constraint->get(i, a) +=
                    0.5 * inverse_spatial_metric.get(j, k) * phi.get(j, c, d) *
                    phi.get(i, k + 1, e) * inverse_spacetime_metric.get(c, d) *
                    spacetime_normal_vector.get(e) *
                    spacetime_normal_one_form.get(a);
              }
            }
          }
        }
      }
    }
  }
}

template <size_t SpatialDim, typename Frame, typename DataType>
void two_index_constraint_add_term_8_of_11(
    const gsl::not_null<tnsr::ia<DataType, SpatialDim, Frame>*> constraint,
    const tnsr::II<DataType, SpatialDim, Frame>& inverse_spatial_metric,
    const tnsr::iaa<DataType, SpatialDim, Frame>& phi) noexcept {
  for (size_t i = 0; i < SpatialDim; ++i) {
    for (size_t a = 0; a < SpatialDim + 1; ++a) {
      for (size_t j = 0; j < SpatialDim; ++j) {
        for (size_t k = 0; k < SpatialDim; ++k) {
          for (size_t m = 0; m < SpatialDim; ++m) {
            for (size_t n = 0; n < SpatialDim; ++n) {
              constraint->get(i, a) -= inverse_spatial_metric.get(j, k) *
                                       inverse_spatial_metric.get(m, n) *
                                       phi.get(j, m + 1, a) *
                                       phi.get(i, k + 1, n + 1);
            }
          }
        }
      }
    }
  }
}

template <size_t SpatialDim, typename Frame, typename DataType>
void two_index_constraint_add_term_9_of_11(
    const gsl::not_null<tnsr::ia<DataType, SpatialDim, Frame>*> constraint,
    const tnsr::iaa<DataType, SpatialDim, Frame>& phi,
    const tnsr::aa<DataType, SpatialDim, Frame>& pi,
    const tnsr::a<DataType, SpatialDim, Frame>& spacetime_normal_one_form,
    const tnsr::AA<DataType, SpatialDim, Frame>& inverse_spacetime_metric,
    const tnsr::A<DataType, SpatialDim, Frame>&
        spacetime_normal_vector) noexcept {
  for (size_t i = 0; i < SpatialDim; ++i) {
    for (size_t a = 0; a < SpatialDim + 1; ++a) {
      for (size_t b = 0; b < SpatialDim + 1; ++b) {
        for (size_t c = 0; c < SpatialDim + 1; ++c) {
          for (size_t d = 0; d < SpatialDim + 1; ++d) {
            for (size_t e = 0; e < SpatialDim + 1; ++e) {
              constraint->get(i, a) +=
                  0.5 * phi.get(i, c, d) * pi.get(b, e) *
                  spacetime_normal_one_form.get(a) *
                  (inverse_spacetime_metric.get(c, b) *
                       inverse_spacetime_metric.get(d, e) +
                   0.5 * inverse_spacetime_metric.get(b, e) *
                       spacetime_normal_vector.get(c) *
                       spacetime_normal_vector.get(d));
            }
          }
        }
      }
    }
  }
}

template <size_t SpatialDim, typename Frame, typename DataType>
void two_index_constraint_add_term_10_of_11(
    const gsl::not_null<tnsr::ia<DataType, SpatialDim, Frame>*> constraint,
    const tnsr::iaa<DataType, SpatialDim, Frame>& phi,
    const tnsr::aa<DataType, SpatialDim, Frame>& pi,
    const tnsr::A<DataType, SpatialDim, Frame>& spacetime_normal_vector,
    const tnsr::AA<DataType, SpatialDim, Frame>&
        inverse_spacetime_metric) noexcept {
  for (size_t i = 0; i < SpatialDim; ++i) {
    for (size_t a = 0; a < SpatialDim + 1; ++a) {
      for (size_t b = 0; b < SpatialDim + 1; ++b) {
        for (size_t c = 0; c < SpatialDim + 1; ++c) {
          for (size_t d = 0; d < SpatialDim + 1; ++d) {
            constraint->get(i, a) -= phi.get(i, c, d) * pi.get(b, a) *
                                     spacetime_normal_vector.get(c) *
                                     (inverse_spacetime_metric.get(b, d) +
                                      0.5 * spacetime_normal_vector.get(b) *
                                          spacetime_normal_vector.get(d));
          }
        }
      }
    }
  }
}

template <size_t SpatialDim, typename Frame, typename DataType>
void two_index_constraint_add_term_11_of_11(
    const gsl::not_null<tnsr::ia<DataType, SpatialDim, Frame>*> constraint,
    const Scalar<DataType>& gamma2,
    const tnsr::a<DataType, SpatialDim, Frame>& spacetime_normal_one_form,
    const tnsr::AA<DataType, SpatialDim, Frame>& inverse_spacetime_metric,
    const tnsr::A<DataType, SpatialDim, Frame>& spacetime_normal_vector,
    const tnsr::iaa<DataType, SpatialDim, Frame>&
        three_index_constraint) noexcept {
  for (size_t i = 0; i < SpatialDim; ++i) {
    for (size_t a = 0; a < SpatialDim + 1; ++a) {
      for (size_t d = 0; d < SpatialDim + 1; ++d) {
        for (size_t c = 0; c < SpatialDim + 1; ++c) {
          constraint->get(i, a) += 0.5 * get(gamma2) *
                                   spacetime_normal_one_form.get(a) *
                                   inverse_spacetime_metric.get(c, d) *
                                   three_index_constraint.get(i, c, d);
        }
        constraint->get(i, a) -= get(gamma2) * spacetime_normal_vector.get(d) *
                                 three_index_constraint.get(i, a, d);
      }
    }
  }
}
}  // namespace

namespace GeneralizedHarmonic {
template <size_t SpatialDim, typename Frame, typename DataType>
tnsr::iaa<DataType, SpatialDim, Frame> three_index_constraint(
    const tnsr::iaa<DataType, SpatialDim, Frame>& d_spacetime_metric,
    const tnsr::iaa<DataType, SpatialDim, Frame>& phi) noexcept {
  auto constraint =
      make_with_value<tnsr::iaa<DataType, SpatialDim, Frame>>(phi, 0.0);
  three_index_constraint<SpatialDim, Frame, DataType>(&constraint,
                                                      d_spacetime_metric, phi);
  return constraint;
}

template <size_t SpatialDim, typename Frame, typename DataType>
void three_index_constraint(
    const gsl::not_null<tnsr::iaa<DataType, SpatialDim, Frame>*> constraint,
    const tnsr::iaa<DataType, SpatialDim, Frame>& d_spacetime_metric,
    const tnsr::iaa<DataType, SpatialDim, Frame>& phi) noexcept {
  // Declare iterators for d_spacetime_metric and phi outside the for loop,
  // because they are const but constraint is not
  auto d_spacetime_metric_it = d_spacetime_metric.begin(), phi_it = phi.begin();

  for (auto constraint_it = (*constraint).begin();
       constraint_it != (*constraint).end();
       ++constraint_it, (void)++d_spacetime_metric_it, (void)++phi_it) {
    *constraint_it = *d_spacetime_metric_it - *phi_it;
  }
}

template <size_t SpatialDim, typename Frame, typename DataType>
tnsr::a<DataType, SpatialDim, Frame> gauge_constraint(
    const tnsr::a<DataType, SpatialDim, Frame>& gauge_function,
    const tnsr::a<DataType, SpatialDim, Frame>& spacetime_normal_one_form,
    const tnsr::A<DataType, SpatialDim, Frame>& spacetime_normal_vector,
    const tnsr::II<DataType, SpatialDim, Frame>& inverse_spatial_metric,
    const tnsr::AA<DataType, SpatialDim, Frame>& inverse_spacetime_metric,
    const tnsr::aa<DataType, SpatialDim, Frame>& pi,
    const tnsr::iaa<DataType, SpatialDim, Frame>& phi) noexcept {
  auto constraint =
      make_with_value<tnsr::a<DataType, SpatialDim, Frame>>(pi, 0.0);
  gauge_constraint<SpatialDim, Frame, DataType>(
      &constraint, gauge_function, spacetime_normal_one_form,
      spacetime_normal_vector, inverse_spatial_metric, inverse_spacetime_metric,
      pi, phi);
  return constraint;
}

template <size_t SpatialDim, typename Frame, typename DataType>
void gauge_constraint(
    const gsl::not_null<tnsr::a<DataType, SpatialDim, Frame>*> constraint,
    const tnsr::a<DataType, SpatialDim, Frame>& gauge_function,
    const tnsr::a<DataType, SpatialDim, Frame>& spacetime_normal_one_form,
    const tnsr::A<DataType, SpatialDim, Frame>& spacetime_normal_vector,
    const tnsr::II<DataType, SpatialDim, Frame>& inverse_spatial_metric,
    const tnsr::AA<DataType, SpatialDim, Frame>& inverse_spacetime_metric,
    const tnsr::aa<DataType, SpatialDim, Frame>& pi,
    const tnsr::iaa<DataType, SpatialDim, Frame>& phi) noexcept {
  for (size_t a = 0; a < SpatialDim + 1; ++a) {
    (*constraint).get(a) = gauge_function.get(a);
    for (size_t i = 0; i < SpatialDim; ++i) {
      for (size_t j = 0; j < SpatialDim; ++j) {
        (*constraint).get(a) +=
            inverse_spatial_metric.get(i, j) * phi.get(i, j + 1, a);
      }
    }
    for (size_t b = 0; b < SpatialDim + 1; ++b) {
      (*constraint).get(a) += spacetime_normal_vector.get(b) * pi.get(b, a);
      for (size_t c = 0; c < SpatialDim + 1; ++c) {
        (*constraint).get(a) -= 0.5 * spacetime_normal_one_form.get(a) *
                                pi.get(b, c) *
                                inverse_spacetime_metric.get(b, c);
        if (a > 0) {
          (*constraint).get(a) -=
              0.5 * phi.get(a - 1, b, c) * inverse_spacetime_metric.get(b, c);
        }
        for (size_t i = 0; i < SpatialDim; ++i) {
          (*constraint).get(a) -= 0.5 * spacetime_normal_one_form.get(a) *
                                  spacetime_normal_vector.get(i + 1) *
                                  phi.get(i, b, c) *
                                  inverse_spacetime_metric.get(b, c);
        }
      }
    }
  }
}

template <size_t SpatialDim, typename Frame, typename DataType>
tnsr::ia<DataType, SpatialDim, Frame> two_index_constraint(
    const tnsr::ia<DataType, SpatialDim, Frame>& d_gauge_function,
    const tnsr::a<DataType, SpatialDim, Frame>& spacetime_normal_one_form,
    const tnsr::A<DataType, SpatialDim, Frame>& spacetime_normal_vector,
    const tnsr::II<DataType, SpatialDim, Frame>& inverse_spatial_metric,
    const tnsr::AA<DataType, SpatialDim, Frame>& inverse_spacetime_metric,
    const tnsr::aa<DataType, SpatialDim, Frame>& pi,
    const tnsr::iaa<DataType, SpatialDim, Frame>& phi,
    const tnsr::iaa<DataType, SpatialDim, Frame>& d_pi,
    const tnsr::ijaa<DataType, SpatialDim, Frame>& d_phi,
    const Scalar<DataType>& gamma2,
    const tnsr::iaa<DataType, SpatialDim, Frame>&
        three_index_constraint) noexcept {
  auto constraint =
      make_with_value<tnsr::ia<DataType, SpatialDim, Frame>>(pi, 0.0);
  two_index_constraint<SpatialDim, Frame, DataType>(
      &constraint, d_gauge_function, spacetime_normal_one_form,
      spacetime_normal_vector, inverse_spatial_metric, inverse_spacetime_metric,
      pi, phi, d_pi, d_phi, gamma2, three_index_constraint);
  return constraint;
}

template <size_t SpatialDim, typename Frame, typename DataType>
void two_index_constraint(
    gsl::not_null<tnsr::ia<DataType, SpatialDim, Frame>*> constraint,
    const tnsr::ia<DataType, SpatialDim, Frame>& d_gauge_function,
    const tnsr::a<DataType, SpatialDim, Frame>& spacetime_normal_one_form,
    const tnsr::A<DataType, SpatialDim, Frame>& spacetime_normal_vector,
    const tnsr::II<DataType, SpatialDim, Frame>& inverse_spatial_metric,
    const tnsr::AA<DataType, SpatialDim, Frame>& inverse_spacetime_metric,
    const tnsr::aa<DataType, SpatialDim, Frame>& pi,
    const tnsr::iaa<DataType, SpatialDim, Frame>& phi,
    const tnsr::iaa<DataType, SpatialDim, Frame>& d_pi,
    const tnsr::ijaa<DataType, SpatialDim, Frame>& d_phi,
    const Scalar<DataType>& gamma2,
    const tnsr::iaa<DataType, SpatialDim, Frame>&
        three_index_constraint) noexcept {
  for (auto& component : *constraint) {
    component = 0.0;
  }

  two_index_constraint_add_term_1_of_11(constraint, inverse_spatial_metric,
                                        d_phi);
  two_index_constraint_add_term_2_of_11(constraint, spacetime_normal_vector,
                                        spacetime_normal_one_form,
                                        inverse_spacetime_metric, d_phi);
  two_index_constraint_add_term_3_of_11(constraint, spacetime_normal_vector,
                                        d_pi);
  two_index_constraint_add_term_4_of_11(constraint, spacetime_normal_one_form,
                                        inverse_spacetime_metric, d_pi);
  two_index_constraint_add_term_5_of_11(constraint, d_gauge_function);
  two_index_constraint_add_term_6_of_11(constraint, spacetime_normal_vector,
                                        spacetime_normal_one_form, phi,
                                        inverse_spacetime_metric);
  two_index_constraint_add_term_7_of_11(
      constraint, inverse_spatial_metric, phi, inverse_spacetime_metric,
      spacetime_normal_vector, spacetime_normal_one_form);
  two_index_constraint_add_term_8_of_11(constraint, inverse_spatial_metric,
                                        phi);
  two_index_constraint_add_term_9_of_11(
      constraint, phi, pi, spacetime_normal_one_form, inverse_spacetime_metric,
      spacetime_normal_vector);
  two_index_constraint_add_term_10_of_11(
      constraint, phi, pi, spacetime_normal_vector, inverse_spacetime_metric);
  two_index_constraint_add_term_11_of_11(
      constraint, gamma2, spacetime_normal_one_form, inverse_spacetime_metric,
      spacetime_normal_vector, three_index_constraint);
}
}  // namespace GeneralizedHarmonic

// Explicit Instantiations
/// \cond
#define DIM(data) BOOST_PP_TUPLE_ELEM(0, data)
#define DTYPE(data) BOOST_PP_TUPLE_ELEM(1, data)
#define FRAME(data) BOOST_PP_TUPLE_ELEM(2, data)

#define INSTANTIATE(_, data)                                                 \
  template tnsr::iaa<DTYPE(data), DIM(data), FRAME(data)>                    \
  GeneralizedHarmonic::three_index_constraint(                               \
      const tnsr::iaa<DTYPE(data), DIM(data), FRAME(data)>&                  \
          d_spacetime_metric,                                                \
      const tnsr::iaa<DTYPE(data), DIM(data), FRAME(data)>& phi) noexcept;   \
  template void GeneralizedHarmonic::three_index_constraint(                 \
      gsl::not_null<tnsr::iaa<DTYPE(data), DIM(data), FRAME(data)>*>         \
          constraint,                                                        \
      const tnsr::iaa<DTYPE(data), DIM(data), FRAME(data)>&                  \
          d_spacetime_metric,                                                \
      const tnsr::iaa<DTYPE(data), DIM(data), FRAME(data)>& phi) noexcept;   \
  template tnsr::a<DTYPE(data), DIM(data), FRAME(data)>                      \
  GeneralizedHarmonic::gauge_constraint(                                     \
      const tnsr::a<DTYPE(data), DIM(data), FRAME(data)>& gauge_function,    \
      const tnsr::a<DTYPE(data), DIM(data), FRAME(data)>&                    \
          spacetime_normal_one_form,                                         \
      const tnsr::A<DTYPE(data), DIM(data), FRAME(data)>&                    \
          spacetime_normal_vector,                                           \
      const tnsr::II<DTYPE(data), DIM(data), FRAME(data)>&                   \
          inverse_spatial_metric,                                            \
      const tnsr::AA<DTYPE(data), DIM(data), FRAME(data)>&                   \
          inverse_spacetime_metric,                                          \
      const tnsr::aa<DTYPE(data), DIM(data), FRAME(data)>& pi,               \
      const tnsr::iaa<DTYPE(data), DIM(data), FRAME(data)>& phi) noexcept;   \
  template void GeneralizedHarmonic::gauge_constraint(                       \
      const gsl::not_null<tnsr::a<DTYPE(data), DIM(data), FRAME(data)>*>     \
          constraint,                                                        \
      const tnsr::a<DTYPE(data), DIM(data), FRAME(data)>& gauge_function,    \
      const tnsr::a<DTYPE(data), DIM(data), FRAME(data)>&                    \
          spacetime_normal_one_form,                                         \
      const tnsr::A<DTYPE(data), DIM(data), FRAME(data)>&                    \
          spacetime_normal_vector,                                           \
      const tnsr::II<DTYPE(data), DIM(data), FRAME(data)>&                   \
          inverse_spatial_metric,                                            \
      const tnsr::AA<DTYPE(data), DIM(data), FRAME(data)>&                   \
          inverse_spacetime_metric,                                          \
      const tnsr::aa<DTYPE(data), DIM(data), FRAME(data)>& pi,               \
      const tnsr::iaa<DTYPE(data), DIM(data), FRAME(data)>& phi) noexcept;   \
  template tnsr::ia<DTYPE(data), DIM(data), FRAME(data)>                     \
  GeneralizedHarmonic::two_index_constraint(                                 \
      const tnsr::ia<DTYPE(data), DIM(data), FRAME(data)>& d_gauge_function, \
      const tnsr::a<DTYPE(data), DIM(data), FRAME(data)>&                    \
          spacetime_normal_one_form,                                         \
      const tnsr::A<DTYPE(data), DIM(data), FRAME(data)>&                    \
          spacetime_normal_vector,                                           \
      const tnsr::II<DTYPE(data), DIM(data), FRAME(data)>&                   \
          inverse_spatial_metric,                                            \
      const tnsr::AA<DTYPE(data), DIM(data), FRAME(data)>&                   \
          inverse_spacetime_metric,                                          \
      const tnsr::aa<DTYPE(data), DIM(data), FRAME(data)>& pi,               \
      const tnsr::iaa<DTYPE(data), DIM(data), FRAME(data)>& phi,             \
      const tnsr::iaa<DTYPE(data), DIM(data), FRAME(data)>& d_pi,            \
      const tnsr::ijaa<DTYPE(data), DIM(data), FRAME(data)>& d_phi,          \
      const Scalar<DTYPE(data)>& gamma2,                                     \
      const tnsr::iaa<DTYPE(data), DIM(data), FRAME(data)>&                  \
          three_index_constraint) noexcept;                                  \
  template void GeneralizedHarmonic::two_index_constraint(                   \
      const gsl::not_null<tnsr::ia<DTYPE(data), DIM(data), FRAME(data)>*>    \
          constraint,                                                        \
      const tnsr::ia<DTYPE(data), DIM(data), FRAME(data)>& d_gauge_function, \
      const tnsr::a<DTYPE(data), DIM(data), FRAME(data)>&                    \
          spacetime_normal_one_form,                                         \
      const tnsr::A<DTYPE(data), DIM(data), FRAME(data)>&                    \
          spacetime_normal_vector,                                           \
      const tnsr::II<DTYPE(data), DIM(data), FRAME(data)>&                   \
          inverse_spatial_metric,                                            \
      const tnsr::AA<DTYPE(data), DIM(data), FRAME(data)>&                   \
          inverse_spacetime_metric,                                          \
      const tnsr::aa<DTYPE(data), DIM(data), FRAME(data)>& pi,               \
      const tnsr::iaa<DTYPE(data), DIM(data), FRAME(data)>& phi,             \
      const tnsr::iaa<DTYPE(data), DIM(data), FRAME(data)>& d_pi,            \
      const tnsr::ijaa<DTYPE(data), DIM(data), FRAME(data)>& d_phi,          \
      const Scalar<DTYPE(data)>& gamma2,                                     \
      const tnsr::iaa<DTYPE(data), DIM(data), FRAME(data)>&                  \
          three_index_constraint) noexcept;

GENERATE_INSTANTIATIONS(INSTANTIATE, (1, 2, 3), (double, DataVector),
                        (Frame::Grid, Frame::Inertial))

#undef DIM
#undef DTYPE
#undef FRAME
#undef INSTANTIATE
/// \endcond
