#include "../include/EventGenerator.hpp"
#include <vector>

bool EventGenerator::init(const int32_t particle_a, const int32_t particle_b,
                          const std::string &proccess_type, const double eCM,
                          const int thread_id) {

  // Configure the beams and collision energy for this generator instance.
  pythia.readString("Beams:idA = " + std::to_string(particle_a));
  pythia.readString("Beams:idB = " + std::to_string(particle_b));
  pythia.readString("Beams:eCM = " + std::to_string(eCM));

  if (thread_id != -1) {
    // Give each worker a repeatable, distinct random-number sequence.
    pythia.readString("Random:setSeed = on");
    pythia.readString("Random:seed = " + std::to_string(thread_id + 1));
  }

  if (proccess_type == "MinimumBias") {
    pythia.readString("SoftQCD:all = on");
  } else {
    std::cerr << "Uknown proccess type\n";
    return false;
  }

  bool success_init = pythia.init();

  if (!success_init) {
    std::cerr << "Something went wrong when initializing pythia\n";
  }
  return success_init;
}

void EventGenerator::generate(Analyzer &analyzer, int32_t num_of_events) {

  std::vector<const Particle *> finalParticles;
  // Failed generation attempts are skipped; only successful events are
  // analyzed.
  for (int32_t iEvent = 0; iEvent < num_of_events; iEvent++) {

    if (!pythia.next())
      continue;

    // Store pointers to final particles to avoid copying Pythia particle data.
    finalParticles.clear();
    finalParticles.reserve(pythia.event.size());

    for (int i = 0; i < pythia.event.size(); ++i) {
      if (pythia.event[i].isFinal()) {
        finalParticles.push_back(&pythia.event[i]);
      }
    }
    analyzer.analyze(finalParticles);
  }
}
