#include "../include/EventGenerator.hpp"
#include <vector>

bool EventGenerator::init(const int32_t particle_a, const int32_t particle_b,
                          const std::string &proccess_type, const double eCM) {

  pythia.readString("Beams:idA = " + std::to_string(particle_a));
  pythia.readString("Beams:idB = " + std::to_string(particle_b));
  pythia.readString("Beams:eCM = " + std::to_string(eCM));
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

void EventGenerator::generate(const int num_of_events) {

  for (int iEvent = 0; iEvent < num_of_events; iEvent++) {

    if (!pythia.next())
      continue;

    std::vector<const Particle *>
        finalParticles; // this prevents doing deep copy of particles
    finalParticles.reserve(pythia.event.size());

    for (int i = 0; i < pythia.event.size(); ++i) {
      if (pythia.event[i].isFinal()) {
        finalParticles.push_back(&pythia.event[i]);
      }
    }
    // analyze
  }
}
