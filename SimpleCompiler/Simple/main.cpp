#include <fstream>
#include <iostream>
#include <Windows.h> //Use for Sleep()
#include "SMLGenerator.h"
#include "Simpletron.h"

using namespace std;

int main(){

	string simpleFilePath = "C:/Users/Austin/Documents/Visual Studio 2010/Projects/SimpleCompiler/Simple.txt";
	string smlFilePath =    "C:/Users/Austin/Documents/Visual Studio 2010/Projects/SimpleCompiler/SMLFile.txt";
	
	ifstream inputSimpleFile(simpleFilePath, ios::in);

	if (!inputSimpleFile){
		cerr << "The file could not be opened." << endl;
		Sleep(6000);
		exit(1);
	}
	else{
		SMLGenerator sg = SMLGenerator(inputSimpleFile);

		//Create output file
		ofstream outputSMLFile(smlFilePath, ios::out);

		if (!outputSMLFile){
			cerr << "Unable to create output file. " << endl;
			Sleep(10000);
			exit(1);
		}
		else{

			/*An SML file is output is not necessary since SML will be output to the screen eventually 
			anyway, but I decided to do this for my own reference.*/
			sg.SMLGeneratorOutput(outputSMLFile);
			outputSMLFile.close();
			
			/*Instead of passing an ifstream object to the constructor like I did in the SMLGenerator class, 
			I decided to do something different and just pass the file path string.
			This way the file stream can be initialized in the constructor using the string, since the assignment operator
			does not work for ifstream objects. I think I like this approach better.*/
			
			Simpletron sm = Simpletron(smlFilePath);
			sm.Execute();
		}
	}
	system("Pause");
}
