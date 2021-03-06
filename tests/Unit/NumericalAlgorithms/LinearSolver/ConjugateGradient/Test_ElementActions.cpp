// Distributed under the MIT License.
// See LICENSE.txt for details.

#include "tests/Unit/TestingFramework.hpp"

#include <algorithm>
#include <string>
#include <unordered_map>

#include "DataStructures/DataBox/DataBox.hpp"
#include "DataStructures/DataBox/DataBoxTag.hpp"
#include "DataStructures/DataBox/Prefixes.hpp"  // IWYU pragma: keep
#include "DataStructures/DenseVector.hpp"
#include "NumericalAlgorithms/LinearSolver/ConjugateGradient/ElementActions.hpp"  // IWYU pragma: keep
#include "NumericalAlgorithms/LinearSolver/IterationId.hpp"
#include "NumericalAlgorithms/LinearSolver/Tags.hpp"  // IWYU pragma: keep
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"
#include "tests/Unit/ActionTesting.hpp"
// IWYU pragma: no_forward_declare db::DataBox

namespace {

struct VectorTag : db::SimpleTag {
  using type = DenseVector<double>;
  static std::string name() noexcept { return "VectorTag"; }
};

using operand_tag = db::add_tag_prefix<LinearSolver::Tags::Operand, VectorTag>;
using residual_tag =
    db::add_tag_prefix<LinearSolver::Tags::Residual, VectorTag>;
using residual_magnitude_tag =
    LinearSolver::Tags::Magnitude<LinearSolver::Tags::Residual<VectorTag>>;

using simple_tags =
    db::AddSimpleTags<VectorTag, LinearSolver::Tags::IterationId,
                      ::Tags::Next<LinearSolver::Tags::IterationId>,
                      operand_tag, residual_tag, residual_magnitude_tag,
                      LinearSolver::Tags::HasConverged>;

template <typename Metavariables>
struct ElementArray {
  using metavariables = Metavariables;
  using chare_type = ActionTesting::MockArrayChare;
  using array_index = int;
  using const_global_cache_tag_list = tmpl::list<>;
  using action_list = tmpl::list<>;
  using initial_databox = db::compute_databox_type<simple_tags>;
};

struct System {
  using fields_tag = VectorTag;
};

struct Metavariables {
  using component_list = tmpl::list<ElementArray<Metavariables>>;
  using system = System;
  using const_global_cache_tag_list = tmpl::list<>;
};

}  // namespace

SPECTRE_TEST_CASE(
    "Unit.Numerical.LinearSolver.ConjugateGradient.ElementActions",
    "[Unit][NumericalAlgorithms][LinearSolver][Actions]") {
  using MockRuntimeSystem = ActionTesting::MockRuntimeSystem<Metavariables>;
  using MockDistributedObjectsTag =
      MockRuntimeSystem::MockDistributedObjectsTag<ElementArray<Metavariables>>;

  const int self_id{0};

  MockRuntimeSystem::TupleOfMockDistributedObjects dist_objects{};
  tuples::get<MockDistributedObjectsTag>(dist_objects)
      .emplace(self_id,
               db::create<simple_tags>(
                   DenseVector<double>(3, 0.), LinearSolver::IterationId{0},
                   LinearSolver::IterationId{0}, DenseVector<double>(3, 2.),
                   DenseVector<double>(3, 1.), 10., false));
  MockRuntimeSystem runner{{}, std::move(dist_objects)};
  const auto get_box = [&runner, &self_id]() -> decltype(auto) {
    return runner.algorithms<ElementArray<Metavariables>>()
        .at(self_id)
        .get_databox<ElementArray<Metavariables>::initial_databox>();
  };
  {
    const auto& box = get_box();
    CHECK(db::get<LinearSolver::Tags::IterationId>(box).step_number == 0);
    CHECK(db::get<LinearSolver::Tags::Operand<VectorTag>>(box) ==
          DenseVector<double>(3, 2.));
  }

  // Can't test the other element actions because reductions are not yet
  // supported. The full algorithm is tested in
  // `Test_ConjugateGradientAlgorithm.cpp` and
  // `Test_DistributedConjugateGradientAlgorithm.cpp`.

  SECTION("InitializeResidualMagnitude") {
    runner.simple_action<ElementArray<Metavariables>,
                         LinearSolver::cg_detail::InitializeResidualMagnitude>(
        self_id, 3.);
    const auto& box = get_box();
    CHECK(db::get<residual_magnitude_tag>(box) == 3.);
  }
  SECTION("UpdateOperand") {
    runner.simple_action<ElementArray<Metavariables>,
                         LinearSolver::cg_detail::UpdateOperand>(self_id, 2.,
                                                                 1., false);
    const auto& box = get_box();
    CHECK(db::get<LinearSolver::Tags::IterationId>(box).step_number == 1);
    CHECK(db::get<LinearSolver::Tags::Operand<VectorTag>>(box) ==
          DenseVector<double>(3, 5.));
    CHECK(db::get<residual_magnitude_tag>(box) == 1.);
    CHECK(db::get<LinearSolver::Tags::HasConverged>(box) == false);
  }
  SECTION("Converge") {
    runner.simple_action<ElementArray<Metavariables>,
                         LinearSolver::cg_detail::UpdateOperand>(self_id, 2.,
                                                                 1., true);
    const auto& box = get_box();
    CHECK(db::get<LinearSolver::Tags::HasConverged>(box) == true);
  }
}
