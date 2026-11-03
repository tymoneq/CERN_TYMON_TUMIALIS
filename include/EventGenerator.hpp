#include "Pythia8/Pythia.h"
#include "Analyzer.hpp"
#pragma once

using namespace Pythia8;

class EventGenerator {
private:
  Pythia pythia;

public:
  // currently the only process_type is MinimumBias
  EventGenerator()
      : pythia("/home/tymon/coding/Pythia/pythia8318/share/Pythia8/xmldoc") {
    // Wyciszenie logów
    pythia.readString("Next:numberShowInfo = 0");
    pythia.readString("Next:numberShowProcess = 0");
    pythia.readString("Next:numberShowEvent = 0");
  }
  bool init(const int32_t particle_a, const int32_t particle_b,
            const std::string &proccess_type, const double eCM = 13600.0);

  void generate(const int num_of_events = 100'000);

  ~EventGenerator() {}
};
