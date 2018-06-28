// Distributed under the MIT License.
// See LICENSE.txt for details.

<<<<<<< 5d10037294c274a48b91a9238719f781432693ba
#include "Executables/ApplyObservers/WrappedApplyObservers.decl.hpp"

=======
>>>>>>> Add ApplyObservers
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
<<<<<<< 5d10037294c274a48b91a9238719f781432693ba
#include "ErrorHandling/Error.hpp"
#include "Executables/ApplyObservers/ApplyObservers.hpp"
#include "Informer/Informer.hpp"
#include "Parallel/Parallel.hpp"
#include "Utilities/FileSystem.hpp"
#include "Utilities/StdHelpers.hpp"

Main::Main(CkArgMsg* msg) {
  Informer::print_startup_info(msg);
  write_volume_xml();
  Parallel::exit();
}

void Main::write_volume_xml() {
  auto files = fs::ls();
=======

#include "ErrorHandling/Error.hpp"
#include "Utilities/FileSystem.hpp"
#include "Utilities/StdHelpers.hpp"

extern "C" void CkRegisterMainModule(void) {}

int main() {
  auto files = file_system::ls();
>>>>>>> Add ApplyObservers
  // Paraview requires the times be in increasing order, so we sort the dir
  // names holding the data, which count in increasing time steps.
  std::sort(files.begin(), files.end());
  std::vector<std::string> volume_data_dirs;
  for (const auto& file : files) {
    if ("VolumeData_" == file.substr(0, 11)) {
      volume_data_dirs.push_back(file);
    }
  }

  // Open the main XML file and write the main header and header for the body.
  std::string file_name("VisVolumeData.xmf");
  FILE* xml = std::fopen(file_name.c_str(), "w");
  if (nullptr == xml) {
    ERROR("Failed to create file '" << file_name << "'.");
  }
  std::fprintf(
      xml, "<?xml version=\"1.0\" ?>\n<!DOCTYPE Xdmf SYSTEM \"Xdmf.dtd\" [\n");
  std::stringstream body;
  body << "<Xdmf Version=\"2.0\">\n  <Domain>\n"
       << "    <Grid Name=\"Evolution\" GridType=\"Collection\" "
          "CollectionType=\"Temporal\">\n";

  std::regex instance_regex(R"(Node([\d]+)\.xmf)");
  std::smatch base_match;
  std::cout << "The number of xmf files found is " << volume_data_dirs.size()
            << "\n";
  // We search for the xmf files in each VolumeData_... directory, then set
  // these to be linked into the main xmf file.
  for (const auto& volume_dir : volume_data_dirs) {
    std::cout << "outer loop is executed\n";
<<<<<<< 5d10037294c274a48b91a9238719f781432693ba
    files = fs::ls(volume_dir);
=======
    files = file_system::ls(volume_dir);
>>>>>>> Add ApplyObservers
    bool first = true;
    for (const auto& file : files) {
      std::cout << "inner loop is executed\n";
      if (std::regex_match(file, base_match, instance_regex)) {
        std::cout << "conditonal statement exectuted\n";
        std::fprintf(xml, "<!ENTITY %s_Node%d SYSTEM \"%s/%s\">\n",
                     volume_dir.c_str(), std::atoi(base_match[1].str().c_str()),
                     volume_dir.c_str(), file.c_str());
        if (first) {
          body << "      <Grid Name=\"Grids\" GridType=\"Collection\">\n";
          first = false;
        }
        body << "        &"
             << formatted_string("%s_Node%d", volume_dir.c_str(),
                                 std::atoi(base_match[1].str().c_str()))
             << ";\n";
      }
    }
    if (not first) {
      body << "      </Grid>\n";
    }
  }
  body << "    </Grid>\n  </Domain>\n</Xdmf>\n";
  std::fprintf(xml, "]>\n%s", body.str().c_str());
  std::fclose(xml);
}

<<<<<<< 5d10037294c274a48b91a9238719f781432693ba
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "Executables/ApplyObservers/ApplyObservers.def.h"
#pragma GCC diagnostic pop
=======

>>>>>>> Add ApplyObservers
