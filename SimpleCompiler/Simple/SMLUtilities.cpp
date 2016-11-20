#include <string>
#include <iostream>
#include <Windows.h>
#include "SMLUtilities.h"

using namespace std;

/*Could use isdigit, but this function handles the whole string rather than just one character*/
bool SMLUtilities::IsIntConstant(string& str){
	int strLength = str.length();
	for (int i = 0; i < strLength; ++i){
		if (!isdigit(str.at(i))){
			return false;
		}
	}
	return true;
}

bool SMLUtilities::IsStrAlpha(const string& str){
	int strLength = str.length();
	for (int i = 0; i < strLength; ++i){
		if (!isalpha(str.at(i))){
			return false;
		}
	}
	return true;
}

void SMLUtilities::trim(string& str){
    size_t fStr = str.find_first_not_of(' ');
    size_t lStr = str.find_last_not_of(' ');
	str = str.substr(fStr, (lStr-fStr+1));
}

string SMLUtilities::addSpace(string& inString){

	string outputString = "";
	size_t stringLength = inString.length();
	outputString.push_back(inString.at(0));
	
	for (size_t i = 0; i < stringLength-1; ++i){
		if ((isalpha(inString.at(i)) && (isdigit(inString.at(i+1)) || isoperator(inString.at(i+1), false)))
			|| (isdigit(inString.at(i)) && (isalpha(inString.at(i+1)) || isoperator(inString.at(i+1), false)))
			|| (isoperator(inString.at(i), false) && (isdigit(inString.at(i+1)) || isalpha(inString.at(i+1))))
			|| (isoperator(inString.at(i), false) && isoperator(inString.at(i+1), false))) {
				outputString.push_back(' ');
		}
		outputString.push_back(inString.at(i+1));
	}

	return outputString;
}

bool SMLUtilities::isoperator (const char& c, const bool& nonPar){
	if (nonPar){
		switch(c){
	case '+':
		return true;
		break;
	case '-':
		return true;
		break;
	case '*':
		return true;
		break;
	case '/':
		return true;
		break;
	//case '^': //Currently Not Supported
	//	return true;
	//	break;
	//case '%': //Currently Not Supported
	//	return true;
	//	break;
	default:
		return false;
		break;
	}
	}
	else{
		switch(c){
	case '+':
		return true;
		break;
	case '-':
		return true;
		break;
	case '*':
		return true;
		break;
	case '/':
		return true;
		break;
	//case '^': //Currently Not Supported
	//	return true;
	//	break;
	//case '%': //Currently Not Supported
	//	return true;
	//	break;
	case '(':
		return true;
		break;
	case ')':
		return true;
		break;
	default:
		return false;
		break;
	}
	}
}

void SMLUtilities::terminate(const string& message){
	cerr << message << endl;
	Sleep(10000);
	exit(1);
}
