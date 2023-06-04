Este es un formato .shw que contiene  12 columnas referentes a la informacion suministrada por Corsika. Las columnas son :                                                                     
 primera columna : Id                                                                                                                                   
 segunda columna : px 
 tercera columna : py 
 cuarta columna : pz 
 quinta columna : x 
 sexta columna : y 
 septima columna : z 
 octava columna : shower_id 
 novena columna : prm_id 
 decima columna : prm_energy 
undecima columna : prm_theta 
duodecima columna : prm_phi

Deseo guardar todo en un solo archivo .root que cree un arbol. Que anada el nu,ero del evento y todas las columnas como brachs
0005 -1.67029e+00 +2.52492e-01 +3.79519e+00 -2.26858e+03 -3.77545e+03 +9.54478e+02 00000001 0703 +1.41286e+02 +22.098 +149.536

#include <TFile.h>
#include <TTree.h>

void ConvertToRoot(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream input(inputFile);
    if (!input.is_open()) {
        std::cout << "Error: Failed to open input file." << std::endl;
        return;
    }

    // Create ROOT file and tree
    TFile file(outputFile.c_str(), "RECREATE");
    TTree tree("data", "Data Tree");

    // Define variables for each column
    Int_t event;
    Int_t id;
    Double_t px, py, pz;
    Double_t x, y, z;
    Int_t shower_id, prm_id;
    Double_t prm_energy, prm_theta, prm_phi;

    // Set branch addresses
    tree.Branch("event", &event, "event/I");
    tree.Branch("id", &id, "id/I");
    tree.Branch("px", &px, "px/D");
    tree.Branch("py", &py, "py/D");
    tree.Branch("pz", &pz, "pz/D");
    tree.Branch("x", &x, "x/D");
    tree.Branch("y", &y, "y/D");
    tree.Branch("z", &z, "z/D");
    tree.Branch("shower_id", &shower_id, "shower_id/I");
    tree.Branch("prm_id", &prm_id, "prm_id/I");
    tree.Branch("prm_energy", &prm_energy, "prm_energy/D");
    tree.Branch("prm_theta", &prm_theta, "prm_theta/D");
    tree.Branch("prm_phi", &prm_phi, "prm_phi/D");

    std::string line;
    int lineNum = 0;
    while (std::getline(input, line)) {
        std::istringstream iss(line);
        if (!(iss >> id >> px >> py >> pz >> x >> y >> z >> shower_id >> prm_id >> prm_energy >> prm_theta >> prm_phi)) {
            std::cout << "Error: Invalid input format at line " << lineNum + 1 << std::endl;
            continue;
        }

        // Set values for each variable
        event = lineNum;
        // Fill the tree with the current event
        tree.Fill();

        lineNum++;
    }

    // Write the tree to the file and close it
    file.Write();
    file.Close();

    std::cout << "Conversion completed successfully. ROOT file saved as " << outputFile << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: ./ConvertToRoot <inputFile.shw> <outputFile.root>" << std::endl;
        return 1;
    }

    std::string inputFile(argv[1]);
    std::string outputFile(argv[2]);

    ConvertToRoot(inputFile, outputFile);

    return 0;
}
