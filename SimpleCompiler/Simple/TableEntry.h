#ifndef TABLEENTRY_H
#define TABLEENTRY_H
#include <string>

class TableEntry{
public:
	TableEntry();
	TableEntry(const std::string&, const char&, const int&);
	TableEntry(const TableEntry&);
	void setSymbol(const std::string&);
	void setType(const char&);
	void setLocation(const int&);
	std::string getSymbol() const;
	char getType() const;
	int getLocation() const;
private:
	std::string symbol;
	char type;
	int location;
};

#endif
