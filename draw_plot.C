#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "TCanvas.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TString.h"

void draw_plot() {
  TFile* file = new TFile("analysis.root", "READ");
  if (!file || file->IsZombie()) {
    std::cerr << "Error opening a file" << std::endl;
    return;
  }

  std::vector<std::string> particles = {"all", "ch", "pip", "pim",
                                        "Kp",  "Km", "p",   "pbar"};
  std::vector<int> colors = {kBlack,     kRed,        kBlue,    kAzure + 7,
                             kGreen + 2, kSpring - 1, kMagenta, kOrange + 1};

  std::vector<std::string> variables = {"h_mult", "h_pT", "h_eta", "h_phi"};

  std::map<std::string, std::string> axis_titles = {
      {"h_mult", "Multiplicity N"},
      {"h_pT", "p_{T} [GeV]"},
      {"h_eta", "#eta"},
      {"h_phi", "#phi [rad]"}};

  for (const auto& var : variables) {
    TCanvas* c = new TCanvas(var.c_str(), var.c_str(), 800, 600);

    TLegend* leg = new TLegend(0.7, 0.7, 0.9, 0.9);
    leg->SetBorderSize(0);

    float maxY = 0;
    std::vector<TH1F*> histos_to_draw;

    for (size_t i = 0; i < particles.size(); ++i) {
      TString histName = Form("%s_%s", var.c_str(), particles[i].c_str());

      TH1F* h = (TH1F*)file->Get(histName);
      if (!h)
        continue;

      h->SetLineColor(colors[i]);
      h->SetLineWidth(2);
      h->SetStats(0);

      h->GetXaxis()->SetTitle(axis_titles[var].c_str());
      h->GetYaxis()->SetTitle("Events / Particles");

      if (h->GetMaximum() > maxY)
        maxY = h->GetMaximum();

      histos_to_draw.push_back(h);
      leg->AddEntry(h, particles[i].c_str(), "l");
    }

    for (size_t i = 0; i < histos_to_draw.size(); ++i) {
      if (i == 0) {
        histos_to_draw[i]->SetMaximum(maxY * 1.2);
        histos_to_draw[i]->Draw("HIST");
      } else {
        histos_to_draw[i]->Draw("HIST SAME");
      }
    }
    leg->Draw();

    c->SaveAs(Form("%s_plot.png", var.c_str()));
    c->SaveAs(Form("%s_plot.pdf", var.c_str()));
  }
}