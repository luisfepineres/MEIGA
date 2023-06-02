#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "TFile.h"
#include "TTree.h"
using namespace std;

struct ParticleData {
	int ParticleID;
	double px;
	double py;
	double pz;
	double x;
	double y;
	double z;
	int shower_id;
	int prm_id;
	double prm_energy;
	double prm_theta;
	double prm_phi;
};

void SaveParticleData(const std::string& outputFileName, const std::map<int, std::vector<ParticleData>>& particleDataMap) {
	TFile outputFile(outputFileName.c_str(), "RECREATE");
	if (!outputFile.IsOpen()) {
		std::cerr << "Error creating output file: " << outputFileName << std::endl;
		return;
	}

	for (const auto& entry : particleDataMap) {
		int particleID = entry.first;
	        const std::vector<ParticleData>& particleDataList = entry.second;
		std::stringstream treeName;
		treeName << "ParticleData_" << particleID;

		TTree tree(treeName.str().c_str(), treeName.str().c_str());
                int size = particleDataList.size();
		tree.Branch("size", &size, "size/I");
		int ParticleID[size];
		double px[size];							
		double py[size];
		double pz[size];                                                                         
		double x[size];
		double y[size];
		double z[size];		                            
                int shower_id[size]; 	
		int prm_id[size];																			      
                double prm_energy[size];	
	        double prm_theta[size]; 
                double prm_phi[size];
					
		for (int i = 0; i < size; ++i) {
			ParticleID[i] = particleDataList[i].ParticleID;
 			px[i] = particleDataList[i].px;
			py[i] = particleDataList[i].py;
			pz[i] = particleDataList[i].pz;
			x[i] = particleDataList[i].x;
			y[i] = particleDataList[i].y;
			z[i] = particleDataList[i].z;																								
			shower_id[i]= particleDataList[i].shower_id; 	
			prm_id[i]= particleDataList[i].prm_id;																			      
                	prm_energy[i]= particleDataList[i].prm_energy;	
	        	prm_theta[i]= particleDataList[i].prm_theta; 
                	prm_phi[i]= particleDataList[i].prm_phi;
		}
																																																			 																	         

		tree.Branch("ParticleID", ParticleID, "ParticleID[size]/I");
		tree.Branch("px", px, "px[size]/D");
		tree.Branch("py", py, "py[size]/D");
		tree.Branch("pz", pz, "pz[size]/D");
		tree.Branch("x", x, "x[size]/D");
		tree.Branch("y", y, "y[size]/D");
		tree.Branch("z", z, "z[size]/D");
	        tree.Branch("shower_id", shower_id, "shower_id[size]/I");
                tree.Branch("prm_id", prm_id, "prm_id[size]/I");
                tree.Branch("prm_energy", prm_energy, "prm_energy[size]/D");
                tree.Branch("prm_theta", prm_theta, "prm_theta[size]/D");
                tree.Branch("prm_phi", prm_phi, "prm_phi[size]/D");
																																                             
		for (int i = 0; i < size; ++i) {
			ParticleID[i] = particleDataList[i].ParticleID;
            		px[i] = particleDataList[i].px;
            		py[i] = particleDataList[i].py;
            		pz[i] = particleDataList[i].pz;
            		x[i] = particleDataList[i].x;
            		y[i] = particleDataList[i].y;
            		z[i] = particleDataList[i].z;
            		shower_id[i] = particleDataList[i].shower_id;
            		prm_id[i] = particleDataList[i].prm_id;
            		prm_energy[i] = particleDataList[i].prm_energy;
            		prm_theta[i] = particleDataList[i].prm_theta;
            		prm_phi[i] = particleDataList[i].prm_phi;
            		tree.Fill();
    			
		}
 		tree.Write();
 	}
        outputFile.Close();
}

void ReadSHWFile(const std::string& shwFileName) {
    std::ifstream shwFile(shwFileName);
    if (!shwFile.is_open()) {
        std::cerr << "Error opening SHW file: " << shwFileName << std::endl;
        return;
    }
    
    std::map<int, std::vector<ParticleData>> particleDataMap;
    std::string line;
    int linecount = 0 ;
    while ((std::getline(shwFile, line)) and linecount<10000) {
        if (line.empty() || line[0] == '#')continue;

        std::stringstream ss(line);
        ParticleData particleData;

        ss >> particleData.ParticleID >> particleData.px >> particleData.py >> particleData.pz
           >> particleData.x >> particleData.y >> particleData.z >> particleData.shower_id
           >> particleData.prm_id >> particleData.prm_energy >> particleData.prm_theta >> particleData.prm_phi;

        particleDataMap[particleData.ParticleID].push_back(particleData);
        linecount++;
        cout << "Particle ID: " << particleData.ParticleID << endl;
    }

    shwFile.close();

    // Create ROOT file and save particle data
    //std::string outputFileName = shwFileName + ".root";

   // Remover la extensión ".shw" del nombre de archivo de entrada
   size_t extensionPos = shwFileName.find_last_of(".");
   std::string baseFileName = shwFileName.substr(0, extensionPos);

   // Definir el nombre de archivo de salida con extensión ".root"
   std::string outputFileName = baseFileName + ".root";

    SaveParticleData(outputFileName, particleDataMap);

    std::cout << "Data saved to ROOT file: " << outputFileName << std::endl;
}

//main function
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <shwFileName>" << std::endl;
        return 1;
    }

    std::string shwFileName = argv[1];

    ReadSHWFile(shwFileName);

    return 0;
}

   
