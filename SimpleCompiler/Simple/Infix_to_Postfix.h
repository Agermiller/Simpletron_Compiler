#ifndef INFIX_TO_POSTFIX
#define INFIX_TO_POSTFIX
#include <string>

class Infix_to_Postfix{
public:
	static std::string generatePostfix(std::string);
private:
	static const std::string LPAR;
	static const std::string RPAR;
	static const std::string SPACE;

	static void infixSyntaxCheck(std::string&);
};

#endif
