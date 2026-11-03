#include "Pythia8/Pythia.h"
#include "TFile.h"
#include "TH1F.h"
#include <cmath>
#include <functional>
#include <vector>

using namespace Pythia8;

using ParticleFilter = std::function<bool(const Particle *)>;

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
  std::vector<pair<ParticleFilter, GeneralHistogram>> channels;
  Analyzer() = default;

  Analyzer(const Analyzer &) = delete;
  Analyzer &operator=(const Analyzer &) = delete;

public:
  static auto &get() {
    static Analyzer singleton;
    return singleton;
  }

  void addChannel(ParticleFilter filter, const std::string &key,
                  const std::string &title_label);

  void analyze(const std::vector<const Particle *> &Particles);

  void saveToFile(const std::string &filename = "analysis.root");
  ~Analyzer() = default;
};