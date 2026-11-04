#include "../include/Analyzer.hpp"

GeneralHistogram::GeneralHistogram(const std::string &key,
                                   const std::string &title_label)
    : name(key) {
  h_mult = new TH1F((key + "_mult").c_str(),
                    (title_label + " Multiplicity;N;Events").c_str(), 400, 0.0,
                    400.0);
  h_pT = new TH1F((key + "_pt").c_str(),
                  (title_label + " pT;p_{T} [GeV/c];Particles").c_str(), 100,
                  0.0, 10.0);
  h_eta =
      new TH1F((key + "_eta").c_str(),
               (title_label + " Eta;#eta;Particles").c_str(), 1100, -5.0, 5.0);
  h_phi = new TH1F((key + "_phi").c_str(),
                   (title_label + " Phi;#phi [rad];Particles").c_str(), 64, 0.0,
                   M_PI);
}

void GeneralHistogram::fillParticle(const Particle *p) {
  total_count++;
  h_pT->Fill(p->pT());
  h_eta->Fill(p->eta());
  h_phi->Fill(std::abs(p->phi()));
}

void GeneralHistogram::endEvent() {
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
    : channels{{{[](const Pythia8::Particle *) { return true; },
                 GeneralHistogram("all", "All Particles")},
                {[](const Pythia8::Particle *p) { return p->isCharged(); },
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
  TFile outFile(filename.c_str(), "RECREATE");
  for (auto &[filter, histos] : channels) {
    histos.write();
  }
  outFile.Close();
}

Analyzer &Analyzer::operator+=(const Analyzer &other) {

  for (size_t i = 0; i < channels.size(); ++i) {
    channels[i].histograms.add(other.channels[i].histograms);
  }
  return *this;
}
