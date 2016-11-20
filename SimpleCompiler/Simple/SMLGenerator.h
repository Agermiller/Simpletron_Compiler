#ifndef SMLGENERATOR_H
#define SMLGENERATOR_H

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include "TableEntry.h"

using namespace std;

class SMLGenerator{
public:
	SMLGenerator();
	SMLGenerator(ifstream& inputSimpleFile);
	void SMLGeneratorOutput(ofstream&) const;
private:
	static const int PROGRAMSIZE = 100;
	vector <TableEntry> SymbolTable;//(PROGRAMSIZE);
	int flags [PROGRAMSIZE];
	int outputArray[PROGRAMSIZE];
	int pass;
	int instructionCounter;
	int symbolTableIndex;
	int dataCounter;
	string lineVar;
	string operationVar;
	string operandsVar;
	string input;
	int tokenCtr;
	bool evalNextToken;
	const char constType;
	const char variableType;
	const char lineType;
	const int smlReadCode;
	const int smlWriteCode;
	const int smlLoadCode;
	const int smlStoreCode;
	const int smlSubtractCode;
	const int smlBranchCode;
	const int smlBranchNegCode;
	const int smlBranchZeroCode;
	const int smlHaltCode;
	map<char, int> operatorMap;

	void addSML(const int&, int&);
	void tokenizeInput(ifstream&);
	void lineStore(const string&);
	void commandStore(const string&, string&);
	int searchSymbolTable(const string&);
	int searchSymbolTable(const string&, const char&);
	int searchSymbolTable(const string&, const char&, const char&);
	void addVarOrConst(string&);
	void memoryCheck(const int&, const int&);
	void terminate(const string&);
	void postfixEval(string&);
	void evaluateLetStatement (string&, int&, int&);
	void evaluateIfGotoStatement (string&, int&, int&);
};

#endif
