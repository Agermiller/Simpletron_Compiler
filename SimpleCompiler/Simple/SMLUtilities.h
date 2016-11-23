#ifndef SMLUTILITIES
#define SMLUTILITIES

#include <string>

class SMLUtilities{
public:
	static bool IsIntConstant(const std::string&);
	static bool IsStrAlpha(const std::string&);
	static void trim(std::string&);
	static std::string addSpace(std::string&);
	static bool isoperator (const char&, const bool&);
	static void terminate(const std::string&);
};

#endif
