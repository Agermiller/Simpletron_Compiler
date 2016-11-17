#include <map>
#include <stack>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
//#include <iostream>
//#include <Windows.h>
#include "TableEntry.h"
#include "SMLGenerator.h"
#include "SMLUtilities.h"
#include "Infix_to_Postfix.h"

using namespace std;
typedef map<char, int> opMap;

//Default constructor
SMLGenerator::SMLGenerator() :
	pass(1),
	instructionCounter(0),
	symbolTableIndex(0),
	dataCounter(PROGRAMSIZE-1),
	lineVar(""),
	operationVar(""),
	operandsVar(""),
	input(""),
	tokenCtr(0),
	evalNextToken(true),
	constType('C'),
	variableType('V'),
	lineType('L')
{
	fill(flags, flags + PROGRAMSIZE, -1); //Initialize flags array to -1
	fill(outputArray, outputArray + PROGRAMSIZE, 0); //Initialize outputArray array to 0
}

//Custom constructor behaves the same as the default constructor, but also initializes the ifstream object
SMLGenerator::SMLGenerator(ifstream& inputFileStream) :
	pass(1),
	instructionCounter(0),
	symbolTableIndex(0),
	dataCounter(PROGRAMSIZE-1),
	lineVar(""),
	operationVar(""),
	operandsVar(""),
	input(""),
	tokenCtr(0),
	evalNextToken(true),
	constType('C'),
	variableType('V'),
	lineType('L')
{
	SymbolTable.resize(PROGRAMSIZE);
	fill(flags, flags + PROGRAMSIZE, -1); //Initialize flags array to -1
	fill(outputArray, outputArray + PROGRAMSIZE, 0); //Initialize outputArray array to 0

	operatorMap.insert(opMap::value_type('*',33));
	operatorMap.insert(opMap::value_type('/',32));
	//operatorMap.insert(opMap::value_type('%',2)); //Currently Not Supported
	//operatorMap.insert(opMap::value_type('^',2)); //Currently Not Supported
	operatorMap.insert(opMap::value_type('+',30));
	operatorMap.insert(opMap::value_type('-',31));

	tokenizeInput(inputFileStream);
}

void SMLGenerator::SMLGeneratorOutput(ofstream& outFile) const{
	for (int idx = 0; idx < (sizeof(flags)/sizeof(flags[0])); ++idx){
		/*I considered using showpos for this, but was getting a strange result for empty 
		indexes, 00+0 instead of +0000.*/
		if (outputArray[idx] >= 0){
			outFile << "+" << setw(4) << setfill('0') << outputArray[idx] << "\n";
		}
		else{
			outFile << setw(4) << setfill('0') << outputArray[idx] << "\n";
		}
	}
	
}

void SMLGenerator::tokenizeInput(ifstream& inputFileStream){
	while (!inputFileStream.eof())
	{
		//Perform first pass
		while(getline(inputFileStream, input))
		{
			stringstream token(input);  

			token >> lineVar;	//Get first token
			token >> operationVar;	//Get second token
			getline(token, operandsVar);	//Get remainder of input string

			lineStore(lineVar);
			if (operationVar != "rem"){
				commandStore(operationVar, operandsVar);
			}
		}

		/*For debugging purposes. View the first n results of the output array or symbol table*/
		//for (size_t i = 0; i < 15; i++){
		//	cout << SymbolTable.at(i).getSymbol()<<" "  << SymbolTable.at(i).getType() <<" " << SymbolTable.at(i).getLocation() << endl;
		//	//cout << outputArray[i] << endl;
		//}

	}//end while (!inputFileStream.eof())

	//Perform second pass		
	for (size_t idx = 0; idx < (sizeof(flags)/sizeof(flags[0])); ++idx){
		if (flags[idx] != -1){
			//Convert flags element to string and search the symbol table for its location
			int flagLocation;
			ostringstream elementVal;
			elementVal << flags[idx];
			flagLocation = searchSymbolTable(elementVal.str());

			outputArray[idx] += flagLocation;
		}
	}

	/*Add constants to the SML array to prepare it for the final output.
	We do not need to add variables because the default SML for a reserved variable is +0000.*/
	for (int it = 0; it < SymbolTable.size(); ++it){
		if (SymbolTable.at(it).getType() == constType){
			outputArray[SymbolTable.at(it).getLocation()] = stoi(SymbolTable.at(it).getSymbol());
		}
	}

	//Print truncated SML to screen
	/*int i = 0;
	do{
		cout << outputArray[i] << endl;
	} while (outputArray[i++] != 4300);*/

	//cout << "The program has compiled successfully." << endl;	
}


void SMLGenerator::lineStore(const string& line){
	TableEntry tableentry(line, lineType, instructionCounter);
	SymbolTable.at(symbolTableIndex++) = tableentry;
}

void SMLGenerator::commandStore(const string& operation, string& operands){
	int sml;
	int operandLocation;
	#pragma region Input	
	if (operation == "input"){
		SMLUtilities::trim(operands); //Remove leading white space left by getline
		operandLocation = searchSymbolTable(operands, variableType);
		if (operandLocation == -1){
			/*TableEntry tableentry(operands, variableType, dataCounter);
			SymbolTable.at(symbolTableIndex++) = tableentry;*/
			addVarOrConst(operands);
		}	
		
		memoryCheck(instructionCounter, dataCounter);
		sml = (10*100) + (dataCounter--);
		outputArray[instructionCounter++] = sml;
	}
	#pragma endregion Input	
	#pragma region Print	
	else if (operation == "print"){
		SMLUtilities::trim(operands); //Remove leading white space left by getline
		operandLocation = searchSymbolTable(operands, variableType);

		if (operandLocation != -1) {
			memoryCheck(instructionCounter, dataCounter);
			sml = (11*100)+operandLocation;
			outputArray[instructionCounter++] = sml;
		}
		else{
			//Fail here. Syntax error, attempting to print undeclared variable
			SMLUtilities::terminate("Runtime error. Attempting to print an undeclared variable.");
		}
	}
	#pragma endregion Print	
	#pragma region Let
	else if (operation == "let"){
		evaluateLetStatement (operands, operandLocation, sml);
	}
	#pragma endregion Let
	#pragma region Goto	
	else if (operation == "goto"){
		operandLocation = searchSymbolTable(operands, lineType);
		if (operandLocation == -1){ //Flag forward reference
			flags[instructionCounter] = stoi(operands);
				operandLocation = 0;
		}
		
		memoryCheck(instructionCounter, dataCounter);
		sml = (40*100)+operandLocation; //Branch to location
		outputArray[instructionCounter++] = sml;
	}
	#pragma endregion Goto
	#pragma region If...Goto	
	else if (operation == "if"){
		/*Take the rest of the string, break it at the comparison sign, convert to int,
		Generate SML accordingly*/
		stringstream expression(operands);
		string firstOperand, compareOperator, secondOperand, branchCommand, branchLocation;
		expression >> firstOperand;
		expression >> compareOperator;
		expression >> secondOperand;
		expression >> branchCommand;
		expression >> branchLocation;

		if (branchCommand != "goto" || !SMLUtilities::IsIntConstant(branchLocation)){
			SMLUtilities::terminate("Syntax error: \n'If' statement must include 'goto' command and a valid line number.");
		}

		string ltOper = "<";
		string gtOper = ">";
		string lteOper = "<=";
		string gteOper = ">=";
		string eqOper = "==";

		/*Generate SML to Load a and Subtract b. 
		Then complete the SML based on which comparison operator is used.*/

		//Search for first operand. If it doesnt exist, then create it.
		operandLocation = searchSymbolTable(firstOperand);
		if (operandLocation == -1){
			addVarOrConst(firstOperand);
			operandLocation = (dataCounter--);
		}
		memoryCheck(instructionCounter, dataCounter);
		sml = (20*100)+operandLocation; //Load firstOperand
		outputArray[instructionCounter++] = sml;

		//Search for second operand. If it doesnt exist, then create it.
		operandLocation = searchSymbolTable(secondOperand);
		if (operandLocation == -1){
			addVarOrConst(secondOperand);
			operandLocation = (dataCounter--);
		}
		memoryCheck(instructionCounter, dataCounter);
		sml = (31*100)+operandLocation; //Subtract secondOperand
		outputArray[instructionCounter++] = sml;

		/*Generate SML to Branch 0*/
		if (compareOperator == eqOper){
			//Search for branch location or forward reference.
			operandLocation = searchSymbolTable(branchLocation, lineType);
			if (operandLocation == -1){ //Record the forward reference
				flags[instructionCounter] = stoi(branchLocation);
				operandLocation = 0;
			}
			memoryCheck(instructionCounter, dataCounter);
			sml = (42*100)+operandLocation; //Branch zero to location
			outputArray[instructionCounter++] = sml;

		}
		/*Generate SML to Branch negative*/
		else if (compareOperator == ltOper || compareOperator == lteOper){

			/*BEGIN lteOper Addition*/
			/*If operator is lteOper, then generate an extra SML line to Branch 0*/
			//Search for branch location
			if (compareOperator == lteOper)
				operandLocation = searchSymbolTable(branchLocation, lineType);{
				if (operandLocation == -1){ //Record the forward reference
					flags[instructionCounter] = stoi(branchLocation);
					operandLocation = 0;
				}
			
				memoryCheck(instructionCounter, dataCounter);
				sml = (42*100)+operandLocation; //Branch zero to location
				outputArray[instructionCounter++] = sml;
			}
			/*END lteOper Addition*/

			operandLocation = searchSymbolTable(branchLocation, lineType);
			if (operandLocation == -1){ //Record the forward reference
				flags[instructionCounter] = stoi(branchLocation);
				operandLocation = 0;
			}

			memoryCheck(instructionCounter, dataCounter);
			sml = (41*100)+operandLocation; //Branch neg to location
			outputArray[instructionCounter++] = sml;
		}
		/*Generate SML to Branch negative and Branch */
		else if (compareOperator == gtOper || compareOperator == gteOper){

			/*BEGIN >= Addition*/
			/*If operator is >=, then generate an extra SML line to Branch 0*/
			//Search for branch location
			if (compareOperator == gteOper){
				operandLocation = searchSymbolTable(branchLocation, lineType);
				if (operandLocation == -1){ //Record the forward reference
					flags[instructionCounter] = stoi(branchLocation);
					operandLocation = 0;
				}
			
				memoryCheck(instructionCounter, dataCounter);
				sml = (42*100)+operandLocation; //Branch zero to location
				outputArray[instructionCounter++] = sml;
			}
			/*END <= Addition*/
			
			memoryCheck(instructionCounter, dataCounter);
			sml = (41*100)+0; //Branch neg to location 2 places away (instructionCounter+2)
			outputArray[instructionCounter++] = sml;

			//Search for branch location
			operandLocation = searchSymbolTable(branchLocation, lineType);
			if (operandLocation == -1){ //Record the forward reference
				flags[instructionCounter] = stoi(branchLocation);
				operandLocation = 0;
			}

			memoryCheck(instructionCounter, dataCounter);
			sml = (40*100)+operandLocation; //Branch neg to location
			outputArray[instructionCounter++] = sml;

			outputArray[instructionCounter-2] += instructionCounter; //Setting branch location for branch neg command
		}
		else{
			//Fail here. Syntax error, invalid operator. Consider adding >= and <=
			SMLUtilities::terminate("Invalid comparison operator. Only operators < <= == >= > are accepted");
		}
	}
	#pragma endregion If...Goto
	#pragma region End	
	else if (operation == "end"){
		memoryCheck(instructionCounter, dataCounter);
		sml = (43*100);
		outputArray[instructionCounter++] = sml;
	}
	#pragma endregion End	
	else {//fail here
		SMLUtilities::terminate("Invalid instruction.");
	}
}


int SMLGenerator::searchSymbolTable(const string& str){
	size_t stSize = SymbolTable.size();
	for (size_t i = 0; i < stSize; i++){
		if (SymbolTable.at(i).getSymbol() == str){
			return SymbolTable.at(i).getLocation();
		}
	}
	return -1;
}

/*Overloaded to specify the symbol type*/
int SMLGenerator::searchSymbolTable(const string& str, const char& type){
	size_t stSize = SymbolTable.size();
	for (size_t i = 0; i < stSize; i++){
		if (SymbolTable.at(i).getSymbol() == str && SymbolTable.at(i).getType() == type){
			return SymbolTable.at(i).getLocation();
		}
	}
	return -1;
}

/*Overloaded to specify the symbol type*/
int SMLGenerator::searchSymbolTable(const string& str, const char& typeA, const char& typeB){
	size_t stSize = SymbolTable.size();
	for (size_t i = 0; i < stSize; i++){
		if (SymbolTable.at(i).getSymbol() == str && (SymbolTable.at(i).getType() == typeA || SymbolTable.at(i).getType() == typeB)){
			return SymbolTable.at(i).getLocation();
		}
	}
	return -1;
}

void SMLGenerator::addVarOrConst(string& operand){
	memoryCheck(instructionCounter+1, dataCounter);

	if (SMLUtilities::IsIntConstant(operand)){ //String is a constant
		TableEntry tableentry(operand, constType, dataCounter);
		SymbolTable.at(symbolTableIndex++) = tableentry;
	}
	else{ //String is a variable
		TableEntry tableentry(operand, variableType, dataCounter);
		SymbolTable.at(symbolTableIndex++) = tableentry;
	}
}

//If instructionCounter is equal to dataCounter, then no more data can be written to
//the output array, and the program must be terminated.
void SMLGenerator::memoryCheck(const int& iCtr, const int& dCtr){
	if (iCtr >= dCtr){
		SMLUtilities::terminate("Generated SML instructions exceed the allocated buffer size. \nConsider shortening the program.");
	}
}

void postfixEval(string& postfix){
}

#pragma region EvalLet
void SMLGenerator::evaluateLetStatement (string& operands, int& operandLocation, int& sml){
	/*Add spaces to the rest of the string so that stringstream has a delimeter*/
		operands = SMLUtilities::addSpace(operands);

		/*Take the string, break it at the assignment operator (=), convert the remainder from infix to postfix 
		notation, and generate SML accordingly*/
		stringstream expression(operands);

		string lVar, eqOper, arithmeticExp;
		expression >> lVar; //Get the left variable
		expression >> eqOper; 
		getline(expression,  arithmeticExp); //Get remainder of input string

		if (SMLUtilities::IsIntConstant(lVar)){
			SMLUtilities::terminate("'Let' statement must use a valid l-variable.");
		}
		if (eqOper != "="){
			SMLUtilities::terminate("Syntax error. Assignment operator must follow the l-variable in 'let' statements.");
		}

		try{
			arithmeticExp = Infix_to_Postfix::generatePostfix(arithmeticExp); //Convert the expression from infix to postfix
			SMLUtilities::trim(arithmeticExp); //Snip off the trailing space
		}
		catch(exception& e){
			SMLUtilities::terminate(e.what());
		}
			
		/*Variable Declarations*/
		stack< string, vector< string > > postfixStack;
		stringstream exp(arithmeticExp);
		stringstream varorconst(arithmeticExp);
		string word = "";
		int accumulator = 0;
		int tempOpLocation = 0;
		int operatorCount = 0;
		string firstOperand = "";
		
		/*Confirm if lVar exists. If not, then add it to SymbolTable*/
		operandLocation = searchSymbolTable(lVar, variableType);
		if (operandLocation == -1){
			addVarOrConst(lVar);
			operandLocation = (dataCounter--);
		}

		/*Scan through the arithmetic string and confirm whether the variables or constants exist in
		the symbol table, or need to be added.*/
		while (!varorconst.eof()){
			varorconst >> word;

			if (isdigit(word.at(0)) || isalpha(word.at(0))){
				tempOpLocation = searchSymbolTable(word, variableType, constType);
				if (tempOpLocation == -1){
					addVarOrConst(word);
					--dataCounter;
				}
			}
			else{
				++operatorCount;
			}
			
		}//End while (!varorconst.eof())

		/*It was discovered that the amount of SML Store commands (code 21) required for a given expression
		was equal to the number of operators in the expression.
		Now that we have the count of operators, we can ensure that there is enough room to store the 
		temporary variables needed to evaluate the expression in SML.
		Since the instructionCounter will be incrementing through this evaluation, it is still possible to 
		run out of memory, but this is a good check to do anyway to make sure we can proceed at the moment.
		Use operatorCount-1 because the final store command will be done against the variable and not a temporary memory location.*/
		memoryCheck(instructionCounter, dataCounter - (operatorCount-1));
		
		/*If no operators, then just evaluate the assignment. Example: let a = 5*/
		if (operatorCount == 0) {
			exp >> word;
			if (isdigit(word.at(0)) || isalpha(word.at(0))){
				//Load the variable or constant 
				tempOpLocation = searchSymbolTable(word, variableType, constType);
				memoryCheck(instructionCounter, dataCounter);
				sml = (20*100)+tempOpLocation;
				outputArray[instructionCounter++] = sml;

				//Then Store it in the l-variable
				memoryCheck(instructionCounter, dataCounter);
				sml = (21*100)+operandLocation;
				outputArray[instructionCounter++] = sml;
			}
			else{
				SMLUtilities::terminate("Invalid syntax in let statement. Operator is not followed by a valid constant or variable.");
			}

			
		}
		/*Else, evaluate the expression. Example: let a = 5 + b*/
		else{
			while (!exp.eof()){
				exp >> word;
			
				//Is word a digit or variable?
				if (isdigit(word.at(0)) || isalpha(word.at(0))){
					tempOpLocation = searchSymbolTable(word, variableType, constType);
					stringstream s;
					s << tempOpLocation;
					postfixStack.push(s.str());
				}
				else{ //word is an operator
					firstOperand = postfixStack.top();
					postfixStack.pop();
								
					//Generate load instruction
					memoryCheck(instructionCounter, dataCounter);
					sml = (20*100)+stoi(postfixStack.top()); 
					outputArray[instructionCounter++] = sml;
					postfixStack.pop();				

					//Generate the arithmetic command
					int operationCode = operatorMap[word.at(0)];
					memoryCheck(instructionCounter, dataCounter);
					sml = (operationCode*100)+stoi(firstOperand);
					outputArray[instructionCounter++] = sml;

					/*Push the temporary variable on to the stack and store it in a temporary variable location, 
					unless this is the final instruction from the stream.
					If that is the case, then store the result in the variable's memory location.*/
					if (exp.peek() == EOF){
						//Last SML statement should be 'Store (21) accumulator into operandLocation'
						memoryCheck(instructionCounter, dataCounter);
						sml = (21*100)+operandLocation;
						outputArray[instructionCounter++] = sml;
					}
					else{
						//Generate Store instruction
						memoryCheck(instructionCounter, dataCounter);
						sml = (21*100) + dataCounter;
						outputArray[instructionCounter++] = sml;

						stringstream s;
						s << dataCounter--;
						postfixStack.push(s.str());
					}
				}			
			}
		}

		/*Reclaim the memory space used for the temporary variables. For expressions with 1 or 0 operators,
		no temporary memory slots were used during evaluation, so there is nothing to reclaim.*/
		if (operatorCount > 1){
			dataCounter = dataCounter + (operatorCount-1);
		}
		
		
}
#pragma endregion EvalLet