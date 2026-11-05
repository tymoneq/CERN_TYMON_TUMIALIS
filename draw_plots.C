#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "TCanvas.h"
#include "TError.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TString.h"

void draw_plots() {
  gErrorIgnoreLevel = kWarning;

  // Read the histograms produced by the simulation.
  TFile* file = TFile::Open("analysis.root", "READ");
  if (!file || file->IsZombie()) {
    std::cerr << "Error opening analysis.root!" << std::endl;
    return;
  }

  // Match each particle category with each histogram variable.
  std::vector<std::string> particles = {"ch", "pip", "pim", "Kp",
                                        "Km", "p",   "pbar"};
  std::vector<std::string> variables = {"hN", "hPt", "hEta", "hPhi"};

  std::map<std::string, std::string> particle_labels = {
      {"ch", "Charged Particles"},
      {"pip", "Pion+"},
      {"pim", "Pion-"},
      {"Kp", "Kaon+"},
      {"Km", "Kaon-"},
      {"p", "Proton"},
      {"pbar", "Antiproton"}};

  std::map<std::string, std::string> axis_titles = {{"mult", "Multiplicity N"},
                                                    {"pt", "p_{T} [GeV/c]"},
                                                    {"eta", "#eta"},
                                                    {"phi", "#phi [rad]"}};

  int saved_count = 0;

  for (const auto& part : particles) {
    for (const auto& var : variables) {
      TString histName = Form("%s_%s", var.c_str(), part.c_str());
      TH1F* h = (TH1F*)file->Get(histName);

      if (!h) {
        std::cout << "[WARNING] Histogram doesn't exists: " << histName
                  << std::endl;
        continue;
      }

      // Format each histogram and save it in both requested file formats.
      TString canvasName = Form("c_%s_%s", part.c_str(), var.c_str());
      TString canvasTitle =
          Form("%s - %s", particle_labels[part].c_str(), var.c_str());
      TCanvas* c = new TCanvas(canvasName, canvasTitle, 800, 600);

      h->SetLineColor(kBlue + 1);
      h->SetLineWidth(2);
      h->SetFillColor(kAzure - 9);
      h->GetYaxis()->SetTitle("Events / Particles");

      h->Draw("HIST");

      TLegend* leg = new TLegend(0.65, 0.75, 0.88, 0.88);

      leg->SetBorderSize(0);
      leg->SetFillStyle(0);
      leg->SetTextFont(42);
      leg->SetTextSize(0.04);

      leg->AddEntry(h, particle_labels[part].c_str(), "f");
      leg->AddEntry((TObject*)0, Form("Entries: %.0f", h->GetEntries()), "");

      leg->Draw();

      c->SaveAs(Form("plots/%s_%s.png", part.c_str(), var.c_str()));
      c->SaveAs(Form("plots/%s_%s.pdf", part.c_str(), var.c_str()));

      //  delete leg;
      delete c;
      saved_count++;
    }
  }

  std::cout << "Successfully drown and saved " << saved_count << " plots"
            << std::endl;
}