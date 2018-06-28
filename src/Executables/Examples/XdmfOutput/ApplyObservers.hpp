// Distributed under the MIT License.
// See LICENSE.txt for details.

///
/// \file
/// Defines the main class for ApplyObservers

#pragma once

class Main : public CBase_Main {
 public:
  Main(CkArgMsg* msg);
  void write_volume_xml();
};
