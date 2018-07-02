// Distributed under the MIT License.
// See LICENSE.txt for details.

#include <iostream>
#include <regex>
#include <sstream>
#include <string>

#include "ErrorHandling/Error.hpp"
#include "Utilities/FileSystem.hpp"
#include "Utilities/StdHelpers.hpp"

extern "C" void CkRegisterMainModule(void) {}

int main() {
  auto files = file_system::ls();
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

  // We search for the xmf files in each VolumeData_... directory, then set
  // these to be linked into the main xmf file.
  for (const auto& volume_dir : volume_data_dirs) {
    files = file_system::ls(volume_dir);
    bool first = true;
    for (const auto& file : files) {
      if (std::regex_match(file, base_match, instance_regex)) {
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
