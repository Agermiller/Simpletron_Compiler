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

const std::string Infix_to_Postfix::LPAR = "(";
const std::string Infix_to_Postfix::RPAR = ")";
const std::string Infix_to_Postfix::SPACE = " ";

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

	//Add spaces so that stringstream has a delimeter to break up the string
	SMLUtilities::trim(infix);
	infix = SMLUtilities::addSpace(infix);
	infixSyntaxCheck(infix);
	postfixStack.push(LPAR);

	/*Append a right parenthesis to the end of infix*/
	infix.push_back(SPACE.at(0));
	infix.push_back(RPAR.at(0));


	stringstream ssInput(infix); 
	while (!ssInput.eof()) {
		ssInput >> token;

		//Token is a digit
		if (isdigit(token.at(0)) || isalpha(token.at(0))){
			postfix.append(token + SPACE);
		}
		//Token is an operator
		else if (SMLUtilities::isoperator(token.at(0), false)){
			if (token == LPAR){
				postfixStack.push(token);
			}
			else if (token == RPAR){
				 //Pop and append all in stack until ( is detected. then pop/discard (
				while (!postfixStack.empty() && postfixStack.top().at(0) != LPAR.at(0)){
					postfix.append(postfixStack.top() + SPACE);
					postfixStack.pop();
				}
				postfixStack.pop();
			}
			else{
				//Pop operator (if exists) off the top of stack if prec is >= token & append to postfix
				//push token on to stack
				if (SMLUtilities::isoperator(postfixStack.top().at(0), true)){ 
					if (precedence[postfixStack.top().at(0)] >= precedence[token.at(0)]){
						postfix.append(postfixStack.top() + SPACE);
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

void Infix_to_Postfix::infixSyntaxCheck(string& infix){
	int operatorCount = 0;
	int varOrConstCount = 0;
	int parCount = 0;
	string str_cur = "";
	string str_plusone = "";
	stringstream ss_cur(infix);
	stringstream ss_plusone(infix);

	/*This string stream should always be one position ahead of ss_cur. Using this technique because I was relying on 
	s_cur.peek to return the next non-whitespace character, but it was returning the next character even if it was whitespace. 
	This method will show the next non whitespace character in the string relative to str_cur.*/
	ss_plusone >> str_plusone; 

	if (SMLUtilities::isoperator(infix.at(0), false) && infix.at(0) != LPAR.at(0)) {
		SMLUtilities::terminate("Syntax error. \nExpression cannot begin with any operator, other than '('.");
	}

	/*For any given arithmetic expression, the number of variables or constants is always 1 greater
	than the number of non-parenthetical operators, multiple non-parenthetical operators can not exist sequentially,
	and the number of parenthesis in an expression must be even.*/
	while (!ss_cur.eof()){
		ss_cur >> str_cur;
		ss_plusone >> str_plusone;

		/*Count the number of variables & constants*/
		if (isalpha(str_cur.at(0)) || isdigit(str_cur.at(0))){
			++varOrConstCount;
		}

		/*Count the number of parenthesis*/
		if (str_cur == LPAR || str_cur == RPAR){
			++parCount;
		}

		/*Count the number of non-parenthetical operators*/
		if (SMLUtilities::isoperator(str_cur.at(0), true)){
			++operatorCount;
		}
	
		/*Confirm that multiple non-parenthetic operators are not located sequentially in the string.*/
		if (ss_cur.peek() != EOF && SMLUtilities::isoperator(str_cur.at(0), true) &&
			SMLUtilities::isoperator(str_plusone.at(0), true)){
			SMLUtilities::terminate("Syntax error. \nMultiple non-parenthetic operators cannot be located sequentially.\n"
				 "Error at '" + str_cur + "', '" + str_plusone + "'");
		}

		/*Confirm that multiple variables/constants are not located sequentially in the string.*/
		if (ss_cur.peek() != EOF && 
			(isalpha(str_cur.at(0)) || isdigit(str_cur.at(0))) && 
			(isalpha(str_plusone.at(0)) || isdigit(str_plusone.at(0))) ){
			SMLUtilities::terminate("Syntax error. \nMultiple variables and/or constants cannot be located sequentially.\n"
				 "Error at '" + str_cur + "', '" + str_plusone + "'");
		}

		/*Confirm that the expression does not end with an operator.*/
		if (ss_cur.peek() == EOF && SMLUtilities::isoperator(str_cur.at(0), true)){
			SMLUtilities::terminate("Syntax error. \nExpression cannot end with an operator other than ')'.");
		}

		/*In an expression, an operator can never precede a right parenthesis:   e = a + )b * c) + d*/ 
		if (ss_cur.peek() != EOF && 
			SMLUtilities::isoperator(str_cur.at(0), true) &&
			str_plusone.at(0) == RPAR.at(0)){
				SMLUtilities::terminate("Syntax error. \nAn operator can not precede a right parenthesis.\n"
					"Error at '" + str_cur + "', '" + str_plusone + "'");
		}

		/*In an expression, a var/constant can never precede a left parenthesis: e = a + (b * c( + d*/
		if (ss_cur.peek() != EOF && 
			(isalpha(str_cur.at(0)) || isdigit(str_cur.at(0))) &&
			str_plusone.at(0) == LPAR.at(0)){
				SMLUtilities::terminate("Syntax error. \nA variable or constant can not precede a left parenthesis.\n"
					"Error at '" + str_cur + "', '" + str_plusone + "'");
		}
	}

	/*Confirm that the number of parenthesis is an even number.*/
	if ((parCount) % 2 != 0){
		SMLUtilities::terminate("Syntax error. \nParenthesis mismatch");
	}

	/*Confirm that the number of variables or constants is 1 greater than operators.
	This test case will probably never fail, since in order for it to fail an expression would need to have two or more
	operators or variables/constants in sequential order; A scenario which is checked in the while loop. In any case, I 
	decided to keep the logic just in case.*/
	if (varOrConstCount != (operatorCount + 1)){
		if ((varOrConstCount - operatorCount) > 0){
			SMLUtilities::terminate("Syntax error. \nToo many variables or constants are found in the expression.");
		}
		else{
			SMLUtilities::terminate("Syntax error. \nToo many operators are found in the expression.");
		}
	}
}
