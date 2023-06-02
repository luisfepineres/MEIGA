//Input flux 
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TColor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "TFile.h"
#include "TTree.h"
#include "TKey.h"

using namespace std;

void AnalyzeParticleData(const std::string& inputFileName, const std::string& outputFileName) {
    // Abrir el archivo ROOT de entrada
    TFile inputFile(inputFileName.c_str(), "READ");
    if (!inputFile.IsOpen()) {
        cout << "Error opening input file: " << inputFileName << endl;
        return;
    }

    // Leer los árboles de datos para todas las partículas
    std::map<int, TTree*> particleTrees;

    TIter next(inputFile.GetListOfKeys());
    TKey* key;
    while ((key = dynamic_cast<TKey*>(next()))) {
        if (key->GetClassName() == TString("TTree")) {
            TTree* tree = dynamic_cast<TTree*>(key->ReadObj());
            if (tree) {
                TString treeName = tree->GetName();
                if (treeName.BeginsWith("ParticleData_")) {
                    int particleID = treeName.ReplaceAll("ParticleData_", "").Atoi();
                    particleTrees[particleID] = tree;
                }
            }
        }
    }

    // Crear el archivo ROOT de salida
    TFile outputFile(outputFileName.c_str(), "RECREATE");
    if (!outputFile.IsOpen()) {
        cout << "Error creating output file: " << outputFileName << endl;
        inputFile.Close();
        return;
    }

    // Colores para cada partícula
    std::map<int, int> colors;
    

    // Iterar sobre cada árbol de partícula
    for (const auto& entry : particleTrees) {
        int particleID = entry.first;
        TTree* tree = entry.second;
        // Obtener el tamaño del vector ptot del branch
        int size = 0;
        TBranch* branchSize = tree->GetBranch("size");
        if (branchSize) {
            branchSize->SetAddress(&size);
            branchSize->GetEntry(0);
        }

        // Crear un histograma para cada partícula
        TH1F* hist = new TH1F(Form("hist_%d", particleID), Form("ptot Distribution (Particle ID %d)", particleID), 210, -10.5, 10.5);

        // Leer el branch "ptot" de cada árbol y llenar el histograma
        Double_t ptot[size];
        TBranch* branch = tree->GetBranch("ptot");
        if (branch) {
            branch->SetAddress(&ptot);
            Long64_t nEntries = tree->GetEntries();
           
            // Llenar el histograma con los valores de ptot
                for (int j = 0; j < size; ++j) {
                    ptot[j] = std::log10(ptot[j]);
                    hist->Fill(ptot[j]);
                }
        }

        // Asignar un color único a cada histograma
        int color = TColor::GetColorPalette(particleID);
        hist->SetLineColor(color);
        //hist->SetLineColor(particleID);

         hist->Write();
    }
   
     // Cerrar el archivo de salida
    outputFile.Close();
    inputFile.Close();

   

  

    cout << "Histograms saved to file: " << outputFileName << endl;
}


int main(int argc, char** argv) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <inputFileName> <outputFileName>" << endl;
        return 1;
    }

    std::string inputFileName = argv[1];
    std::string outputFileName = argv[2];

    AnalyzeParticleData(inputFileName, outputFileName);

    return 0;
}


               
