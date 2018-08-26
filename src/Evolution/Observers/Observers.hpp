// Distributed under the MIT License.
// See LICENSE.txt for details

#pragma once

#include <array>
#include <cstddef>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "DataStructures/DataBox/DataBox.hpp"
#include "DataStructures/DataBox/DataBoxTag.hpp"
#include "DataStructures/DataBox/Prefixes.hpp"
#include "DataStructures/Variables.hpp"
#include "Domain/Domain.hpp"
#include "Domain/Element.hpp"
#include "Domain/ElementId.hpp"
#include "Domain/Mesh.hpp"
#include "Domain/Tags.hpp"
#include "IO/Connectivity.hpp"
#include "IO/VolumeDataFile.hpp"
#include "NumericalAlgorithms/DiscontinuousGalerkin/Tags.hpp"
#include "Parallel/ConstGlobalCache.hpp"
#include "Parallel/Printf.hpp"
#include "PointwiseFunctions/AnalyticSolutions/Tags.hpp"
#include "Time/Tags.hpp"
#include "Time/Time.hpp"
#include "Utilities/ConstantExpressions.hpp"
#include "Utilities/FileSystem.hpp"
#include "Utilities/Gsl.hpp"
#include "Utilities/MakeArray.hpp"
#include "Utilities/Requires.hpp"
#include "Utilities/TMPL.hpp"

/// \cond
namespace tuples {
template <typename...>
class TaggedTuple;
}  // namespace tuples

namespace Parallel {
template <typename Metavariables>
class ConstGlobalCache;
}  // namespace Parallel
/// \endcond

namespace Actions {
template <size_t Dim>
struct Observe {
  template <typename DbTagsList, typename... InboxTags, typename Metavariables,
            typename ArrayIndex, typename ActionList,
            typename ParallelComponent>
  static auto apply(db::DataBox<DbTagsList>& box,
                    tuples::TaggedTuple<InboxTags...>& /*inboxes*/,
                    const Parallel::ConstGlobalCache<Metavariables>& /*cache*/,
                    const ArrayIndex& /*array_index*/,
                    const ActionList /*meta*/,
                    const ParallelComponent* const /*meta*/) noexcept {
    // Retrieve element
    const auto element = db::get<Tags::Element<Dim>>(box);
    const auto element_id{element.id()};
    size_t block_id = element_id.block_id();
    const auto segment_ids = element_id.segment_ids();
    const auto segment_id = segment_ids[0].index();
    std::stringstream segment_stream;
    segment_stream << "Block[" << segment_id << "]";
    Parallel::printf("Block id: %d\n", block_id);
    Parallel::printf("Segement id: %d\n", segment_id);

    // Retrieve coordinate
    const auto mesh = db::get<Tags::Mesh<Dim>>(box);
    const auto extents = mesh.extents();
    Parallel::printf("Extents: %d\n", extents[0]);
    const auto inertial_coords =
        db::get<Tags::Coordinates<Tags::ElementMap<Dim>,
                                  Tags::LogicalCoordinates<Dim>>>(box);

    // Retrieve time
    const auto time = db::get<Tags::Time>(box).value();
    Parallel::printf("Current Time: %f\n", time);

    // Generate name of output file
    std::stringstream dir;
    dir << "VolumeData_" << std::setfill('0') << std::setw(8)
        << static_cast<size_t>(time * 1000.0 + 0.5) << "_00000000";
    std::stringstream file_name;
    file_name << dir.str() << "/Node0.h5";

    // Create file and write data if file doesn't exist. Otherwise
    // append data to output file.
    if (!file_system::check_if_file_exists(file_name.str())) {
      file_system::create_directory(dir.str());
      vis::VolumeFile output(file_name.str(), 2, false);
      output.write_xdmf_time(time);
      // Write coordinate and topology data to .h5 and .xdmf files
      output.write_element(time, inertial_coords, extents,
                           segment_stream.str());
    } else {
      vis::VolumeFile output(file_name.str(), 2, true);
      // Write coordinate and topology data to .h5 and .xdmf files
      output.write_element(time, inertial_coords, extents,
                           segment_stream.str());
    }

    // output.write_element_data(detyped_tensors, extents, element_id.str());
    return std::forward_as_tuple(std::move(box));
  }
};

}  // namespace Actions
