#include "Pythia8/Pythia.h"
#include "TFile.h"
#include "TH1F.h"
#include <cmath>
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
private:
  size_t NUM_OF_TYPES;
  std::vector<GeneralHistogram> allHistograms;

public:
  Analyzer(const size_t num_of_types = 4) : NUM_OF_TYPES(num_of_types) {
    allHistograms.resize(NUM_OF_TYPES);
  }
};