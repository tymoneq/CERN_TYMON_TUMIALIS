#include "include/Analyzer.hpp"
#include "include/EventGenerator.hpp"
#include <iostream>

void create_channels() {
  std::cout << "Starting creating channels\n";

  // 2. Cząstki naładowane
  Analyzer::get().addChannel(
      [](const Pythia8::Particle *p) { return p->isCharged(); }, "ch",
      "Charged Particles");

  // 3. Piony
  Analyzer::get().addChannel(
      [](const Pythia8::Particle *p) { return p->id() == 211; }, "pip",
      "Pion+");
  Analyzer::get().addChannel(
      [](const Pythia8::Particle *p) { return p->id() == -211; }, "pim",
      "Pion-");

  // 4. Kaony
  Analyzer::get().addChannel(
      [](const Pythia8::Particle *p) { return p->id() == 321; }, "Kp", "Kaon+");
  Analyzer::get().addChannel(
      [](const Pythia8::Particle *p) { return p->id() == -321; }, "Km",
      "Kaon-");

  // 5. Protony
  Analyzer::get().addChannel(
      [](const Pythia8::Particle *p) { return p->id() == 2212; }, "p",
      "Proton");
  Analyzer::get().addChannel(
      [](const Pythia8::Particle *p) { return p->id() == -2212; }, "pbar",
      "Antiproton");

  std::cout << "All channels created\n";
}

int main() {
  create_channels();
  EventGenerator eg;
  if (!eg.init(2212, 2212, "MinimumBias", 13600.0)) {
    return 1;
  }
  std::cout << "Starting generating events\n";

  eg.generate();

  Analyzer::get().saveToFile();

  return 0;
}
