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

void Analyzer::addChannel(ParticleFilter filter, const std::string &key,
                          const std::string &title_label) {
  channels.emplace_back(std::move(filter), GeneralHistogram(key, title_label));
}
