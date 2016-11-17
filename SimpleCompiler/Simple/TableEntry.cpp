#include "TableEntry.h"
#include <string>

TableEntry::TableEntry(const std::string& sym, const char& t,const int& loc)
	: symbol(sym), type(t), location(loc)
{
	//setSymbol(sym); setType(t); setLocation(loc);
}

TableEntry::TableEntry()
	: symbol(" "), type('\0'), location(0)
{
}

//TableEntry::TableEntry( TableEntry const& teCopy)
TableEntry::TableEntry(const TableEntry& teCopy)
	:symbol(teCopy.symbol), type(teCopy.type), location(teCopy.location)
{
	/*setSymbol(teCopy.symbol);
	setType(teCopy.type);
	setLocation(teCopy.location);*/
}

void TableEntry::setSymbol(const std::string& s){
	symbol = s;
}

void TableEntry::setType(const char& t){
	type = t;
}

void TableEntry::setLocation(const int& l){
	location = l;
}

std::string TableEntry::getSymbol() const{
	return symbol;
}

char TableEntry::getType() const{
	return type;
}

int TableEntry::getLocation() const{
	return location;
}