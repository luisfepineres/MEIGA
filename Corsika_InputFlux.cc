// Codigo para analizar el input flux usando el archivo root dado por Corsika_decoding.cc
#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <iostream>
#include <map>
#include <string>
#include <TMath.h>
#include <THStack.h>
#include <TCanvas.h>
#include <TLegend.h>

using namespace std;

void CreateHistograms(const std::string& inputFile, const std::string& outputFile) {
    // Open the ROOT file
    TFile file(inputFile.c_str(), "READ");
    if (file.IsZombie()) {
        std::cout << "Error: Failed to open input file." << std::endl;
        return;
    }

    // Get the data tree
    TTree* tree = dynamic_cast<TTree*>(file.Get("data"));
    if (!tree) {
        std::cout << "Error: Failed to retrieve data tree from ROOT file." << std::endl;
        file.Close();
        return;
    }

    // Set branch addresses
    Int_t id;
    Double_t px, py, pz;
    Double_t x, y, z;
    Int_t shower_id, prm_id;
    Double_t prm_energy, prm_theta, prm_phi;

    tree->SetBranchAddress("id", &id);
    tree->SetBranchAddress("px", &px);
    tree->SetBranchAddress("py", &py);
    tree->SetBranchAddress("pz", &pz);
    tree->SetBranchAddress("x", &x);
    tree->SetBranchAddress("y", &y);
    tree->SetBranchAddress("z", &z);
    tree->SetBranchAddress("shower_id", &shower_id);
    tree->SetBranchAddress("prm_id", &prm_id);
    tree->SetBranchAddress("prm_energy", &prm_energy);
    tree->SetBranchAddress("prm_theta", &prm_theta);
    tree->SetBranchAddress("prm_phi", &prm_phi);

    // Create a map to store histograms
    std::map<int, TH1F*> histograms;

    // Loop over the entries in the tree
    Long64_t numEntries = tree->GetEntries();
    Double_t ptot;
    for (Long64_t i = 0; i < numEntries; i++) {
        tree->GetEntry(i);

        // Create histogram if it doesn't exist for the current ID
        if (histograms.find(id) == histograms.end()) {
            std::string histName = "histo_partID_" + std::to_string(id);
            histograms[id] = new TH1F(histName.c_str(), ("Histogram for ID " + std::to_string(id)).c_str(), 500, -15, 15);
        }

        // Fill the histogram with the branch value
        ptot = TMath::Log(TMath::Sqrt((px * px) + (py * py) + (pz * pz)));
        histograms[id]->Fill(ptot);
    }

    // Create output file and save histograms
    TFile output(outputFile.c_str(), "RECREATE");
    
  
    // Loop over the histograms and add them to the stack
   

    for (auto& pair : histograms) {
        
        int id = pair.first;
        TH1F* histo = pair.second;

        // Set line color based on ID
        int lineColor;
        if (id == 1) lineColor = kBlue;
        else if (id == 2) lineColor = kOrange;
        else if (id == 3) lineColor = kGreen;
        else if (id == 5) lineColor = kRed;
        else if (id == 6) lineColor = kMagenta;
        else if (id == 8) lineColor = kMagenta;
        else if (id == 13) lineColor = kViolet;
        else if (id == 14) lineColor = kBlack;
        else lineColor = kBlack;
        // Set histogram properties
        histo->SetLineColor(lineColor);
        histo->SetLineWidth(1.7);
         // Set axis titles
        histo->GetXaxis()->SetTitle("Log(p/MeV)");
        histo->GetYaxis()->SetTitle("Counts");
       


        // Set titles based on ID
        if (id == 1) histo->SetTitle("Input Flux associated with ID 1");
        else if (id == 2) histo->SetTitle("Input Flux associated with ID 2");
        else if (id == 3) histo->SetTitle("Input Flux associated with ID 3");
        else if (id == 5) histo->SetTitle("Input Flux associated with ID 5");
        else if (id == 6) histo->SetTitle("Input Flux associated with ID 6");
        else if (id == 8) histo->SetTitle("Input Flux associated with ID 8");
        else if (id == 13) histo->SetTitle("Input Flux associated with ID 13");
        else if (id == 14) histo->SetTitle("Input Flux associated with ID 14");
        else histo->SetTitle(("Input Flux associated with ID " + std::to_string(id)).c_str());
        // Set logarithmic scale on Y-axis
        //histo->SetLogy();
        // Save histogram as image
        std::string imageName = "histogram_id_" + std::to_string(id) + ".png";
        pair.second->Write();       
        delete pair.second;     
    }
    output.Close();

    std::cout << "Histograms saved to " << outputFile << std::endl;

    // Close the input file
    file.Close();
}



int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./CreateHistograms <inputFile.root> <outputFile.root>" << std::endl;
        return 1;
    }

    std::string inputFile(argv[1]);
    std::string outputFile(argv[2]);

    CreateHistograms(inputFile, outputFile);
    return 0;
}
