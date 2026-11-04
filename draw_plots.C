#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "TCanvas.h"
#include "TError.h"
#include "TFile.h"
#include "TH1F.h"
#include "TString.h"

void draw_plots() {
  gErrorIgnoreLevel = kWarning;

  TFile* file = TFile::Open("analysis.root", "READ");
  if (!file || file->IsZombie()) {
    std::cerr << "Error opening analysis.root!" << std::endl;
    return;
  }

  std::vector<std::string> particles = {"ch", "pip", "pim", "Kp",
                                        "Km", "p",   "pbar"};
  std::vector<std::string> variables = {"mult", "pt", "eta", "phi"};

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
      TString histName = Form("%s_%s", part.c_str(), var.c_str());
      TH1F* h = (TH1F*)file->Get(histName);

      if (!h) {
        std::cout << "[WARNING] Histogram doesn't exists: " << histName << std::endl;
        continue;
      }


      TString canvasName = Form("c_%s_%s", part.c_str(), var.c_str());
      TString canvasTitle =
          Form("%s - %s", particle_labels[part].c_str(), var.c_str());
      TCanvas* c = new TCanvas(canvasName, canvasTitle, 800, 600);

      
      h->SetLineColor(kBlue + 1);
      h->SetLineWidth(2);
      h->SetFillColor(kAzure - 9);  
      h->GetXaxis()->SetTitle(axis_titles[var].c_str());
      h->GetYaxis()->SetTitle("Events / Particles");

      h->Draw("HIST");

      
      c->SaveAs(Form("plots/%s_%s.png", part.c_str(), var.c_str()));
      c->SaveAs(Form("plots/%s_%s.pdf", part.c_str(), var.c_str()));

      delete c;
      saved_count++;
    }
  }

  std::cout << "Successfully drown and saved " << saved_count
            << " plots" << std::endl;
}