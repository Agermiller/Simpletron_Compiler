#ifndef SIMPLETRON
#define SIMPLETRON

#include <fstream>
#include <string>

class Simpletron{
public:
	Simpletron();
	Simpletron(const std::string&);
	void setDebugMode(const bool&);
	void Execute();
private:
	std::ifstream inputFile;
	bool ifstreamInitialized;
	std::string input;
	int intInput;
	static const int arraySize = 100;
	int memory [arraySize];
	int operand; 
	int accumulator; 
	int operationCode; 
	int instructionRegister;
	int instructionCounter; 
	bool logDetail;

	static void read(int&, int*, bool&);
	static void write(int&, const int*, bool&);
	static int load(int&, const int*, bool&);
	static void store(int&, int&, int*, bool&);
	static int add(int&, int&, int*, bool&);
	static int subtract(int&, int&, int*, bool&);
	static int divide(int&, int&, int*, bool&);
	static int multiply(int&, int&, int*, bool&);
	static int modulus(int&, int&, int*, bool&);
	static int power(int&, int&, int*, bool&);
	static void branch(int&, int&, bool&);
	static bool branchNeg(int&, int&, int&, bool&);
	static bool branchZero(int&, int&, int&, bool&);
	static void halt(int&, int&, int&, int&, int&, const int*, const int&);
};

#endif
