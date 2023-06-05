//Codigo para leer el archivo Corsika .shw
#include <iostream>
#include <fstream>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage: ./program <inputFile.shw>" << std::endl;
		return 1;
	}
	std::string filePath = argv[1];
	std::ifstream inputFile(filePath);	        
	if (!inputFile) {
		std::cerr << "Error opening input file: " << filePath << std::endl;
		return 1;
	}
	//Imprimir las primeras n lineas del archivo .shw
	std::string line;
	int n = 15;
	int lineCount = 0;
	while ((std::getline(inputFile, line))&& lineCount < n ) {
	// Procesar cada línea del archivo aquí
		std::cout << line << std::endl;
		lineCount++;
	}						
	inputFile.close();
	return 0;
}
						
