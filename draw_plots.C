#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "TCanvas.h"
#include "TError.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TMath.h"
#include "TString.h"
#include "TStyle.h"

void draw_plots() {
  // Ignorowanie ostrzeżeń w konsoli podczas rysowania
  gErrorIgnoreLevel = kWarning;

  gStyle->SetOptTitle(1);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadRightMargin(0.08);
  gStyle->SetPadTopMargin(0.12);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetTitleFontSize(0.05);
  gStyle->SetTitleOffset(1.2, "Y");

  // Otwarcie wygenerowanego pliku
  TFile* file = TFile::Open("analysis.root", "READ");
  if (!file || file->IsZombie()) {
    std::cerr << "[ERROR] Cannot open analysis.root. Check if the simulation "
                 "ran successfully."
              << std::endl;
    return;
  }
  std::cout << "[INFO] File analysis.root opened successfully.\n";

  // Listy cząstek i zmiennych do wyrysowania
  std::vector<std::string> particles = {"ch", "pip", "pim", "Kp",
                                        "Km", "p",   "pbar"};
  std::vector<std::string> variables = {"hN", "hPt", "hEta", "hPhi"};

  // Mapowanie skrótów na ładne etykiety (z symbolami LaTeX dla ROOT)
  std::map<std::string, std::string> particle_labels = {
      {"ch", "Charged Particles"},     {"pip", "Pion+ (#pi^{+})"},
      {"pim", "Pion- (#pi^{-})"},      {"Kp", "Kaon+ (K^{+})"},
      {"Km", "Kaon- (K^{-})"},         {"p", "Proton (p)"},
      {"pbar", "Antiproton (#bar{p})"}};

  // Mapowanie zmiennych na odpowiednie tytuły osi X
  std::map<std::string, std::string> axis_titles = {
      {"hN", "Multiplicity N"},
      {"hPt", "p_{T} [GeV/c]"},
      {"hEta", "#eta (Pseudorapidity)"},
      {"hPhi", "#phi [rad]"}};

  int saved_count = 0;

  for (const auto& part : particles) {
    for (const auto& var : variables) {
      TString histName = Form("%s_%s", var.c_str(), part.c_str());
      TH1F* h = (TH1F*)file->Get(histName);

      if (!h) {
        std::cout << "[WARNING] Histogram doesn't exist in file: " << histName
                  << std::endl;
        continue;
      }

      // Przygotowanie płótna
      TString canvasName = Form("c_%s_%s", part.c_str(), var.c_str());
      TString histTitle = Form("%s Distribution for %s", var.c_str(),
                               particle_labels[part].c_str());
      TCanvas* c = new TCanvas(canvasName, histTitle, 800, 600);

      // Konfiguracja wyglądu histogramu (kolorowe wypełnienie zamiast samej
      // linii)
      h->SetTitle(histTitle);
      h->SetLineColor(kBlue + 2);
      h->SetLineWidth(2);
      h->SetFillColor(kAzure - 9);
      h->GetXaxis()->SetTitle(axis_titles[var].c_str());
      h->GetYaxis()->SetTitle("Events / Particles");

      // --- Formatowanie osi X ---
      h->GetXaxis()->CenterTitle(true);
      h->GetXaxis()->SetTitleFont(42);
      h->GetXaxis()->SetLabelFont(42);
      h->GetXaxis()->SetTitleSize(0.045);
      h->GetXaxis()->SetLabelSize(0.04);
      h->GetXaxis()->SetTitleOffset(1.2);

      // --- Formatowanie osi Y ---
      h->GetYaxis()->CenterTitle(true);
      h->GetYaxis()->SetTitleFont(42);
      h->GetYaxis()->SetLabelFont(42);
      h->GetYaxis()->SetTitleSize(0.045);
      h->GetYaxis()->SetLabelSize(0.04);
      h->GetYaxis()->SetTitleOffset(1.5);
      h->GetYaxis()->SetMaxDigits(4);

      if (var == "hPhi") {
        h->GetXaxis()->SetRangeUser(-TMath::Pi(), TMath::Pi());
        h->SetMinimum(0);
      }

      h->Draw("HIST");

      TLegend* leg = new TLegend(0.60, 0.75, 0.88, 0.86);
      leg->SetBorderSize(0);
      leg->SetFillStyle(0);
      leg->SetTextFont(42);
      leg->SetTextSize(0.04);
      leg->SetMargin(0.2);

      leg->AddEntry(h, particle_labels[part].c_str(), "f");
      leg->AddEntry((TObject*)0, Form("Entries: %.0f", h->GetEntries()), "");
      leg->Draw();

      c->SaveAs(Form("plots/%s_%s.png", part.c_str(), var.c_str()));
      c->SaveAs(Form("plots/%s_%s.pdf", part.c_str(), var.c_str()));

      delete leg;
      delete c;
      saved_count++;
    }
  }

  std::cout << "[SUCCESS] Drawn and saved " << saved_count
            << " plots successfully into 'plots/' directory." << std::endl;

  file->Close();
  delete file;
}