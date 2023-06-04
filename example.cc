#include <TCanvas.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TROOT.h>
#include <TFile.h>

#include <iostream>
#include <string>
#include <json/json.h>
#include <json/value.h>
#include <fstream>
#include <vector>

using namespace std;


string completepath(string s, string cfiledir) {
    if (s[0] == '/') {
        return s;
    } else {
        return cfiledir + "/" + s;
    }
}

void Process(const string& cfgfile, map<string, Json::Value>& processedData, map<string, bool>& processedcfg) {
    ifstream cfgfile2(cfgfile);
    if (!cfgfile2) {
        cout << "[ERROR] Process: Configuration file " << cfgfile << " does not exist. Wrong path or misspelled filename." << endl;
        return;
    }
    Json::Value cfgdata;
    cfgfile2 >> cfgdata;
    Json::Value outputcfg = cfgdata["Output"];
    string cfgfiledir = cfgfile.substr(0, cfgfile.find_last_of('/'));

    bool compressOutput = outputcfg.get("CompressOutput", false).asBool();
    string ifile = completepath(outputcfg.get("OutputFile", "").asString(), cfgfiledir);
    if (compressOutput) {
        ifile += ".gz";
    }
    if (!ifstream(ifile)) {
        cout << "[ERROR] Process: Input file " << ifile << " does not exist. Wrong path or misspelled filename." << endl;
        return;
    }

    bool saveInput = outputcfg.get("SaveInput", false).asBool();
    bool savePETimeDistribution = outputcfg.get("SavePETimeDistribution", false).asBool();
    bool saveComponentsPETimeDistribution = outputcfg.get("SaveComponentsPETimeDistribution", false).asBool();
    bool saveEnergy = outputcfg.get("SaveEnergy", false).asBool();
    bool saveComponentsEnergy = outputcfg.get("SaveComponentsEnergy", false).asBool();

    if (saveComponentsPETimeDistribution) {
        savePETimeDistribution = false;
    }
    if (saveComponentsEnergy) {
        saveEnergy = false;
    }

    cout << endl;
    cout << "[INFO] Process: Reading output file: " << ifile << endl;
    Json::Value data;
    if (compressOutput) {
        gzifstream infile(ifile);
        infile >> data;
    } else {
        ifstream infile(ifile);
        infile >> data;
    }

    vector<string> detectors;
    vector<string> optdevices;
    Json::Value::Members members = data.getMemberNames();
    for (const auto& name : members) {
        if (name.find("Detector") != string::npos) {
            detectors.push_back(name);
        } else if (name.find("OptDevice") != string::npos) {
            optdevices.push_back(name);
        }
    }

    cout << "[INFO] Process: Found information of Detector(s):" << endl;
    for (const auto& det : detectors) {
        cout << det << endl;
    }

    cout << "[INFO] Process: Found information of Optical Device(s):" << endl;
    for (const auto& optdev : optdevices) {
        cout << optdev << endl;
    }

    map<string, Json::Value> processedData;

    if (saveInput) {
        processedData["InputFlux"] = GetInputFlux(data);
    }
    if (saveEnergy) {
        processedData["DepositedEnergy"] = GetDepositedEnergy(data, detectors);
    }
    if (saveComponentsEnergy) {
processedData["ComponentsDepositedEnergy"] = GetComponentsDepositedEnergy(data, detectors);
}
if (saveComponentsPETimeDistribution) {
processedData["ComponentsPETimeDistribution"] = GetComponentsTraces(data, optdevices);
}
if (savePETimeDistribution) {
processedData["PETimeDistribution"] = GetTraces(data, optdevices);
}

processedData.swap(processedData);
processedcfg["InputFlux"] = saveInput;
processedcfg["PETimeDistribution"] = savePETimeDistribution;
processedcfg["ComponentsPETimeDistribution"] = saveComponentsPETimeDistribution;
processedcfg["DepositedEnergy"] = saveEnergy;
processedcfg["ComponentsDepositedEnergy"] = saveComponentsEnergy;

}

int main(int argc, char* argv[]) {
if (argc != 2) {
cout << "Uso: programa archivo_configuracion.json" << endl;
return 1;
}

string cfgfile(argv[1]);

map<string, Json::Value> processedData;
map<string, bool> processedcfg;

Process(cfgfile, processedData, processedcfg);

TFile* outputFile = new TFile("output.root", "RECREATE");

for (const auto& entry : processedData) {
    const string& dataType = entry.first;
    const Json::Value& data = entry.second;

    if (dataType == "InputFlux") {
        TGraph* graph = new TGraph(data.size());
        for (int i = 0; i < data.size(); ++i) {
            const Json::Value& point = data[i];
            double x = point["x"].asDouble();
            double y = point["y"].asDouble();
            graph->SetPoint(i, x, y);
        }
        graph->SetName("input_flux");
        graph->Write();
    }

    // ...
    // Añade más bloques de código para guardar los otros tipos de datos procesados en el archivo de salida de Root
      if (dataType == "DepositedEnergy") {
        TH1D* hist = new TH1D("deposited_energy", "Deposited Energy", 100, 0, 100);
        for (int i = 0; i < data.size(); ++i) {
            const Json::Value& point = data[i];
            double energy = point["energy"].asDouble();
            hist->Fill(energy);
        }
        hist->Write();
    }
    
    if (dataType == "ComponentsDepositedEnergy") {
        TCanvas* canvas = new TCanvas("components_deposited_energy", "Components Deposited Energy");
        int numComponents = data.getMemberNames().size();
        int numBins = 100;
        double minEnergy = 0.0;
        double maxEnergy = 100.0;
        TH1D** histograms = new TH1D*[numComponents];
        TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
        for (int i = 0; i < numComponents; ++i) {
            const string& component = data.getMemberNames()[i];
            const Json::Value& componentData = data[component];
            double mean = componentData["mean"].asDouble();
            double sigma = componentData["sigma"].asDouble();
            histograms[i] = new TH1D(("component_" + to_string(i)).c_str(), ("Component " + to_string(i)).c_str(), numBins, minEnergy, maxEnergy);
            histograms[i]->FillRandom("gaus", 1000);
            histograms[i]->Scale(1000.0 / histograms[i]->GetEntries());
            histograms[i]->SetLineColor(i + 1);
            histograms[i]->Draw(i == 0 ? "" : "SAME");
            legend->AddEntry(histograms[i], ("Component " + to_string(i)).c_str(), "l");
        }
        legend->Draw();
        canvas->Write();
    }
    
    if (dataType == "ComponentsPETimeDistribution") {
        TH1D* hist = new TH1D("components_pe_time_distribution", "Components PE Time Distribution", 100, 0, 100);
        for (const auto& component : data.getMemberNames()) {
            const Json::Value& componentData = data[component];
            double mean = componentData["mean"].asDouble();
            double sigma = componentData["sigma"].asDouble();
            hist->FillRandom("gaus", 1000);
            hist->SetBinContent(hist->FindBin(mean), hist->GetBinContent(hist->FindBin(mean)) + 1000);
        }
        hist->Write();
    }

    if (dataType == "PETimeDistribution") {
        TH1D* hist = new TH1D("pe_time_distribution", "PE Time Distribution", 100, 0, 100);
        for (int i = 0; i < data.size(); ++i) {
            const Json::Value& point = data[i];
            double time = point["time"].asDouble();
            hist->Fill(time);
        }
        hist->Write();
    }

}

outputFile->Close();
delete outputFile;

return 0;
}

