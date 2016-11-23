#include <math.h>
#include <string>
#include <fstream>
#include <iomanip>
#include <iostream>
#include "Windows.h"
#include "Simpletron.h"
#include "SMLUtilities.h"


using namespace std;

Simpletron::Simpletron():
	input (""),
	intInput (0),
	operand (0), //Indicate the memory location on which the current instruction operates
	accumulator (0), //Represent the accumulator register
	operationCode (0), //Indicate the operation currently being performed
	instructionRegister (0), //Contains the current instruction
	instructionCounter (0), //Keep track of the location in memory that contains the instruction being performed
	logDetail (false), //If true, debug information will print to the consoleinputFile(smlPath)
	ifstreamInitialized(false)
{
	fill(memory, memory + arraySize, 0); //Initialize memoryArray array to 0
}

/*Is just like the default constructor, but initializes the ifstream object*/
Simpletron::Simpletron(const string& smlPath):
	input (""),
	intInput (0),
	operand (0), //Indicate the memory location on which the current instruction operates
	accumulator (0), //Represent the accumulator register
	operationCode (0), //Indicate the operation currently being performed
	instructionRegister (0), //Contains the current instruction
	instructionCounter (0), //Keep track of the location in memory that contains the instruction being performed
	logDetail (false), //If true, debug information will print to the consoleinputFile(smlPath)
	inputFile(smlPath, ios::in),
	ifstreamInitialized(true) //Since there is no way to initialize the ifstream object later, and no way to know if it was initialized
{
	fill(memory, memory + arraySize, 0); //Initialize memoryArray array to 0
}

void Simpletron::Execute(){
	if (!ifstreamInitialized){
		SMLUtilities::terminate("Simpletron object must be initialized with a file path.");
	}

	/*Begin reading the input*/
	if (!inputFile){
		SMLUtilities::terminate("The SML file could not be opened.");
	}
	else{
		while (!inputFile.eof()){

			inputFile >> intInput;

			try {
				//intInput = stoi(input);
				if (intInput <= 9999 && intInput >= -9999){
					memory[instructionCounter++] = intInput;
				}
				else{
					SMLUtilities::terminate("File contains a number not between -9999 and +9999");
				}
			}
			catch (invalid_argument &){
				SMLUtilities::terminate("File contains a number not between -9999 and +9999");
			}
		}
	}

	/*Begin parsing the SML*/
	instructionCounter = 0; //Reset to beginning of instruction array
	instructionRegister = memory[instructionCounter];
	operationCode = instructionRegister / 100;//arraySize;
	operand = instructionRegister % 100;//arraySize;

	while (operationCode != 43){
		switch(operationCode)
		{
		case 10:
			Simpletron::read(operand, memory, logDetail);
			++instructionCounter;
			break;

		case 11:
			Simpletron::write(operand, memory, logDetail);
			++instructionCounter;
			break;

		case 20:
			accumulator = Simpletron::load(operand, memory, logDetail);
			++instructionCounter;
			break;

		case 21:
			Simpletron::store(accumulator, operand, memory, logDetail);
			++instructionCounter;
			break;

		case 30:
			accumulator = Simpletron::add(accumulator, operand, memory, logDetail);
			if (accumulator > 9999 || accumulator < -9999){
				cerr << "*** ERROR: Accumulator overflow. Performing register/memory dump. ***" << endl;
				operationCode = 43;
			}
			else{
				++instructionCounter;
			}
			break;

		case 31:
			accumulator = Simpletron::subtract(accumulator, operand, memory, logDetail);
			if (accumulator > 9999 || accumulator < -9999){
				cerr << "*** ERROR: Accumulator overflow. Performing register/memory dump. ***" << endl;
				operationCode = 43;
			}
			else{
				++instructionCounter;
			}
			break;

		case 32:
			try{
				accumulator = Simpletron::divide(accumulator, operand, memory, logDetail);
				++instructionCounter;
			}
			catch(runtime_error &r){
				cerr << r.what() << endl;
				operationCode = 43;
			}
			break;

		case 33:
			accumulator = Simpletron::multiply(accumulator, operand, memory, logDetail);
			++instructionCounter;
			break;

		case 34:
			accumulator = Simpletron::modulus(accumulator, operand, memory, logDetail);
			++instructionCounter;
			break;

		case 35:
			accumulator = Simpletron::power(accumulator, operand, memory, logDetail);
			++instructionCounter;
			break;

		case 40:
			Simpletron::branch(operand, instructionCounter, logDetail);
			break;

		case 41:
			instructionCounter += Simpletron::branchNeg(operand, instructionCounter, accumulator, logDetail) ? 0 : 1;
			break;

		case 42:
			instructionCounter += Simpletron::branchZero(operand, instructionCounter, accumulator, logDetail) ? 0 : 1;
			break;

		default:
			cerr << "*** ERROR: Default case executed. Performing register/memory dump. ***" << endl;
			operationCode = 43;
			break;
		}

		if (operationCode != 43){
			instructionRegister = memory[instructionCounter];
			operationCode = instructionRegister / 100;//arraySize;
			operand = instructionRegister % 100;//arraySize;
		}
	}

	Simpletron::halt(accumulator, instructionCounter, instructionRegister, operationCode, operand, memory, arraySize);

}

void Simpletron::setDebugMode(const bool& debug) {
	logDetail = debug;
}

void Simpletron::read(int& operand, int* memory, bool& logDetail){
	string input = "";
	int flag = true;

	while(flag){
		try{
			cout << "\nEnter a number:";
			cin >> input;
			memory[operand] = stoi(input);
			if (logDetail){
				cout << "DEBUG: Number accepted" << endl;
			}

			flag = false;
		}
		catch(invalid_argument &){
			cout << "Please enter an integer:" ;
		}
	}
}

void Simpletron::write(int& operand, const int* memory, bool& logDetail){
	cout << "\nWrite: " << memory[operand] << endl;
	if (logDetail){
		cout << "DEBUG: Number written" << endl;
	}
}

int Simpletron::load(int& operand, const int* memory, bool& logDetail){
	if (logDetail){
		cout << "DEBUG: Number " << memory[operand] << " loaded" << endl;
	}
	return memory[operand];
}

void Simpletron::store(int& accumulator, int& operand, int* memory, bool& logDetail){
	memory[operand] = accumulator;
	if (logDetail){
		cout << "DEBUG: Number " << memory[operand] << " stored" << endl;
	}
}

int Simpletron::add(int& accumulator, int& operand, int* memory, bool& logDetail){
	if (logDetail){
		cout << "DEBUG: Number added = " << memory[operand] << endl;
	}

	return accumulator + memory[operand];
}

int Simpletron::subtract(int& accumulator, int& operand, int* memory, bool& logDetail){
	if (logDetail){
		cout << "DEBUG: Number subtracted = " << memory[operand] << endl;
	}

	return accumulator - memory[operand];
}

int Simpletron::divide(int& accumulator, int& operand, int* memory, bool& logDetail){
	if (logDetail){
		cout << "DEBUG: Number divided = " << operand <<endl;
	}

	if (memory[operand] == 0){
		throw runtime_error("*** ERROR: Attempt to divide by zero. Performing register/memory dump. ***");
		return 0;
	}
	else{
		return accumulator / memory[operand];
	}
}

int Simpletron::multiply(int& accumulator, int& operand, int* memory, bool& logDetail){
	if (logDetail){
		cout << "DEBUG: Number multiplied = " << memory[operand] << endl;
	}

	return accumulator * memory[operand];
}

int Simpletron::modulus(int& accumulator, int& operand, int* memory, bool& logDetail){
	if (logDetail){
		cout << "DEBUG: Number modulated = " << memory[operand] << endl;
	}

	return accumulator % memory[operand];
}

int Simpletron::power(int& accumulator, int& operand, int* memory, bool& logDetail){
	if (logDetail){
		cout << "DEBUG: Exponent = " << memory[operand] << endl;
	}

	return int(pow(double(accumulator), memory[operand]));
}

void Simpletron::branch(int& operand, int& instructionCounter, bool& logDetail){
	instructionCounter = operand;
	if (logDetail){
		cout << "DEBUG: Branch to " << operand << endl;
	}
}

bool Simpletron::branchNeg(int& operand, int& instructionCounter, int& accumulator, bool& logDetail){
	if (accumulator < 0){
		instructionCounter = operand;
		if (logDetail){
			cout << "DEBUG: Neg branch to " << operand << endl;
		}
		return true;
	}
	if (logDetail){
		cout << "DEBUG: No neg branch" << endl;
	}

	return false;
}

bool Simpletron::branchZero(int& operand, int& instructionCounter, int& accumulator, bool& logDetail){
	if (accumulator == 0){
		instructionCounter = operand;
		if (logDetail){
			cout << "DEBUG: 0 branch to " << operand << endl;
		}
		return true;
	}
	if (logDetail){
		cout << "DEBUG: No 0 branch" << endl;
	}
	return false;
}

void Simpletron::halt(int& acc, int& instCounter, int& instReg, int& operCode, int& operand, const int* memory, const int& arrSize){
	int arrDimHundredth = (int)ceil((double)arrSize / 100.0); //How many 10 x 10 memory blocks to print
	int arrDimension = 10;
	int arrColumnDimension = 10;
	int arrRemainderDimension = arrSize % 100;
	int index = 0;

	cout << "\nREGISTERS:" << endl;
	cout << "accumulator\t\t" << showpos << setw(5) << setfill('0') << internal << acc << endl;
	cout << "instructionCounter\t" << noshowpos << setw(2) << instCounter << endl;
	cout << "instructionRegister\t" << showpos << setw(5) << instReg << endl;
	cout << "operationCode\t\t" << noshowpos << setw(2) << operCode << endl;
	cout << "operand\t\t\t" << setw(2) << operand << endl;


	for (int block = 1; block <= arrDimHundredth; ++block){

		cout << "\nMEMORY:" << endl;
		for (int i = 0; i < arrDimension; ++i){
			if (i == 0){
				cout << setw(8) << noshowpos << setfill(' ') << i;
			}
			else {
				cout << setw(6) << i;
			}
		}

		if (block == arrDimHundredth && arrRemainderDimension != 0){
			arrColumnDimension = (arrRemainderDimension / 10);
		}

		cout << "\n";

		for (int j = 0; j < arrColumnDimension; ++j){
			cout << noshowpos << setw(2) << (j*10) << " ";
			for (int k = 0; k < arrDimension; ++k){
				cout << showpos << setw(5) << setfill('0') << memory[index++] << " ";
			}
			cout << "\n";
		}
	}
}
