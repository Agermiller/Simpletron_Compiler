#include <map>
#include <stack>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "SMLUtilities.h"
#include "Infix_to_Postfix.h"

using namespace std;
typedef map<char, int> oMap;

string Infix_to_Postfix::generatePostfix(string infix){
	string token = "";
	string postfix = "";
	stack< string, vector< string > > postfixStack;
	oMap precedence;

	/*Operators (keys) with a higher value have a higher precedence*/
	precedence.insert(oMap::value_type('*',2));
	precedence.insert(oMap::value_type('/',2));
	//precedence.insert(oMap::value_type('%',2)); //Currently Not Supported
	//precedence.insert(oMap::value_type('^',2)); //Currently Not Supported
	precedence.insert(oMap::value_type('+',1));
	precedence.insert(oMap::value_type('-',1));

	/*Append a right parenthesis to the end of infix*/
	infix.push_back(')');

	//Add spaces so that stringstream has a delimeter to break up the string
	SMLUtilities::trim(infix);
	infix = SMLUtilities::addSpace(infix);
	postfixStack.push("(");

	stringstream ssInput(infix); 
	while (!ssInput.eof()) {
		ssInput >> token;

		//Token is a digit
		if (isdigit(token.at(0)) || isalpha(token.at(0))){
			postfix.append(token + " ");
		}
		//Token is an operator
		else if (SMLUtilities::isoperator(token.at(0), false)){
			if (token == "("){
				postfixStack.push(token);
			}
			else if (token == ")"){
				 //Pop and append all in stack until ( is detected. then pop/discard (
				while (!postfixStack.empty() && postfixStack.top().at(0) != '('){
					postfix.append(postfixStack.top() + " ");
					postfixStack.pop();
				}
				postfixStack.pop();
			}
			else{
				//Pop operator (if exists) off the top of stack if prec is >= token & append to postfix
				//push token on to stack
				if (SMLUtilities::isoperator(postfixStack.top().at(0), true)){ 
					if (precedence[postfixStack.top().at(0)] >= precedence[token.at(0)]){
						postfix.append(postfixStack.top() + " ");
						postfixStack.pop();
					}
				}
				postfixStack.push(token);
			}
		}
		else {
			throw("Invalid character detected in the arithmetic string.");
			break;
		}
	}
	return postfix;
}