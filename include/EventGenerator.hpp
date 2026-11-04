#include "Analyzer.hpp"
#include "Pythia8/Pythia.h"
#pragma once

using namespace Pythia8;

class EventGenerator {
private:
  Pythia pythia;

public:
  // Construct Pythia with the XML path supplied by CMake and quiet its logs.
  EventGenerator() : pythia(PYTHIA_XML_DIR) { // macro from cmake
    pythia.readString("Next:numberShowInfo = 0");
    pythia.readString("Next:numberShowProcess = 0");
    pythia.readString("Next:numberShowEvent = 0");
  }
  bool init(const int32_t particle_a, const int32_t particle_b,
            const std::string &proccess_type, const double eCM = 13600.0,
            const int thread_id = -1);

  // Generate events and pass their final-state particles to the analyzer.
  void generate(Analyzer &analyzer, const int num_of_events = 100'000);

  ~EventGenerator() {}
};
