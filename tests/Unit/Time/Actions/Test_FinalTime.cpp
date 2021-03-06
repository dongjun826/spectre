// Distributed under the MIT License.
// See LICENSE.txt for details.

#include "tests/Unit/TestingFramework.hpp"

#include <array>
#include <utility>
// IWYU pragma: no_include <unordered_map>

#include "DataStructures/DataBox/DataBox.hpp"
#include "Time/Actions/FinalTime.hpp"  // IWYU pragma: keep
#include "Time/Slab.hpp"
#include "Time/Tags.hpp"  // IWYU pragma: keep
#include "Time/Time.hpp"
#include "Time/TimeId.hpp"
#include "Utilities/Gsl.hpp"
#include "Utilities/TMPL.hpp"
#include "Utilities/TaggedTuple.hpp"
#include "tests/Unit/ActionTesting.hpp"

namespace {
struct Metavariables;
struct component {
  using metavariables = Metavariables;
  using chare_type = ActionTesting::MockArrayChare;
  using array_index = int;
  using const_global_cache_tag_list = tmpl::list<OptionTags::FinalTime>;
  using action_list = tmpl::list<Actions::FinalTime>;
  using simple_tags = db::AddSimpleTags<Tags::TimeId, Tags::TimeStep>;
  using compute_tags = db::AddComputeTags<Tags::Time>;
  using initial_databox =
      db::compute_databox_type<tmpl::append<simple_tags, compute_tags>>;
};

struct Metavariables {
  using component_list = tmpl::list<component>;
  using const_global_cache_tag_list = tmpl::list<>;
};
}  // namespace

SPECTRE_TEST_CASE("Unit.Time.Actions.FinalTime", "[Unit][Time][Actions]") {
  const Slab slab(3., 6.);

  using MockRuntimeSystem = ActionTesting::MockRuntimeSystem<Metavariables>;
  using MockDistributedObjectsTag =
      MockRuntimeSystem::MockDistributedObjectsTag<component>;
  MockRuntimeSystem::TupleOfMockDistributedObjects dist_objects{};
  tuples::get<MockDistributedObjectsTag>(dist_objects)
      .emplace(0, ActionTesting::MockDistributedObject<component>{
                      db::create<typename component::simple_tags,
                                 typename component::compute_tags>(
                          TimeId{}, TimeDelta{})});
  MockRuntimeSystem runner{{5.}, std::move(dist_objects)};
  auto& box = runner.algorithms<component>()
                  .at(0)
                  .get_databox<typename component::initial_databox>();

  struct Test {
    Time time{};
    TimeDelta time_step{};
    bool expected_result{};
  };
  const std::array<Test, 4> tests{{
      {slab.start(), slab.duration(), false},
      {slab.start(), -slab.duration(), true},
      {slab.end(), slab.duration(), true},
      {slab.end(), -slab.duration(), false}}};

  for (const auto& test : tests) {
    db::mutate<Tags::TimeId, Tags::TimeStep>(
        make_not_null(&box), [&test](const auto time_id, const auto time_step) {
          *time_id = TimeId(time_step->is_positive(), 0, test.time);
          *time_step = test.time_step;
        });

    runner.next_action<component>(0);
    CHECK(test.expected_result ==
          runner.algorithms<component>().at(0).get_terminate());
  }
}
