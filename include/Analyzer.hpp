#pragma once

#include "Pythia8/Pythia.h"
#include "TFile.h"
#include "TH1F.h"
#include <cmath>
#include <functional>
#include <vector>

using namespace Pythia8;

struct GeneralHistogram {
  std::string name;
  TH1F *h_mult; // Krotność (N)
  TH1F *h_pT;   // Pęd poprzeczny (pT)
  TH1F *h_eta;  // Pseudorapidność (eta)
  TH1F *h_phi;  // Kąt azymutalny (phi)
  int64_t total_count = 0;

  GeneralHistogram(const std::string &key, const std::string &title_label);

  void fillParticle(const Particle *p);
  void endEvent();
  void write();
};

class Analyzer {
public:
  using ParticleFilter = bool (*)(const Particle *);

  static constexpr int num_of_channels = 8;
  struct Channel {
    ParticleFilter filter;
    GeneralHistogram histograms;
  };

private:
  std::array<Channel, num_of_channels> channels;

  Analyzer(const Analyzer &) = delete;
  Analyzer &operator=(const Analyzer &) = delete;

public:
  Analyzer();

  void analyze(const std::vector<const Particle *> &Particles);

  void saveToFile(const std::string &filename = "analysis.root");
  ~Analyzer() = default;
};