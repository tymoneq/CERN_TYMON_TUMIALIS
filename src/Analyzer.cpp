#include "../include/Analyzer.hpp"

GeneralHistogram::GeneralHistogram(const std::string &key,
                                   const std::string &title_label)
    : name(key) {
  // Book one event-level multiplicity and three particle-level distributions.
  h_mult = new TH1F(("hN_" + key).c_str(),
                    (title_label + " Multiplicity;N;Events").c_str(), 400, 0.0,
                    400.0);
  h_pT = new TH1F(("hPt_" + key).c_str(),
                  (title_label + " pT;p_{T} [GeV/c];Particles").c_str(), 100,
                  0.0, 10.0);
  h_eta =
      new TH1F(("hEta_" + key).c_str(),
               (title_label + " Eta;#eta;Particles").c_str(), 100, -5.0, 5.0);
  h_phi = new TH1F(("hPhi_" + key).c_str(),
                   (title_label + " Phi;#phi [rad];Particles").c_str(), 64, 0.0,
                   M_PI);
}

GeneralHistogram::~GeneralHistogram() {
  delete h_mult;
  delete h_pT;
  delete h_eta;
  delete h_phi;
}

void GeneralHistogram::fillParticle(const Particle *p) {
  // Count this particle and record its kinematic values.
  total_count++;
  h_pT->Fill(p->pT());
  h_eta->Fill(p->eta());
  h_phi->Fill(std::abs(p->phi()));
}

void GeneralHistogram::endEvent() {
  // Store the category count for the completed event, then reset it.
  h_mult->Fill(total_count);
  total_count = 0;
}

void GeneralHistogram::write() {
  h_mult->Write();
  h_pT->Write();
  h_eta->Write();
  h_phi->Write();
}

void GeneralHistogram::add(const GeneralHistogram &other) {
  h_mult->Add(other.h_mult);
  h_pT->Add(other.h_pT);
  h_eta->Add(other.h_eta);
  h_phi->Add(other.h_phi);
}

void Analyzer::analyze(const std::vector<const Particle *> &Particles) {

  for (const auto *p : Particles) {
    for (auto &[filter, histos] : channels) {
      if (filter(p)) {
        histos.fillParticle(p);
      }
    }
  }

  for (auto &[filter, histos] : channels) {
    histos.endEvent();
  }
}

Analyzer::Analyzer()
    // Each channel pairs a particle-selection rule with its histograms.
    : channels{{{[](const Pythia8::Particle *p) { return p->isCharged(); },
                 GeneralHistogram("ch", "Charged Particles")},
                {[](const Pythia8::Particle *p) { return p->id() == 211; },
                 GeneralHistogram("pip", "Pion+")},
                {[](const Pythia8::Particle *p) { return p->id() == -211; },
                 GeneralHistogram("pim", "Pion-")},
                {[](const Pythia8::Particle *p) { return p->id() == 321; },
                 GeneralHistogram("Kp", "Kaon+")},
                {[](const Pythia8::Particle *p) { return p->id() == -321; },
                 GeneralHistogram("Km", "Kaon-")},
                {[](const Pythia8::Particle *p) { return p->id() == 2212; },
                 GeneralHistogram("p", "Proton")},
                {[](const Pythia8::Particle *p) { return p->id() == -2212; },
                 GeneralHistogram("pbar", "Antiproton")}}} {}

void Analyzer::saveToFile(const std::string &filename) {
  // Write every category's histograms into a new ROOT file.
  TFile outFile(filename.c_str(), "RECREATE");
  outFile.cd();

  for (auto &[filter, histos] : channels) {
    histos.write();
  }
  outFile.Close();
}

Analyzer &Analyzer::operator+=(const Analyzer &other) {

  // Add matching channel histograms when combining worker results.
  for (size_t i = 0; i < channels.size(); ++i) {
    channels[i].histograms.add(other.channels[i].histograms);
  }
  return *this;
}
