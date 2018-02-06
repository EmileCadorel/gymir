#include "syntax/Word.hh"
#include <sstream>
#include <algorithm>

Word::Word (location_t locus, std::string str) :
    str (str)
{
    if (locus != UNKNOWN_LOCATION) {
	this-> locFile = LOCATION_FILE (locus);
	this-> line = LOCATION_LINE (locus);
	this-> column = LOCATION_COLUMN (locus);
    }
}

Word::Word (location_t locus, const char* str) :
    str (str)
{
    if (locus != UNKNOWN_LOCATION) {
	this-> locFile = LOCATION_FILE (locus);
	this-> line = LOCATION_LINE (locus);
	this-> column = LOCATION_COLUMN (locus);
    }
}

Word::Word (const Word & other, std::string str) :
    str (str),
    locFile (other.locFile),
    line (other.line),
    column (other.column)
{}


Word::Word (const Word & other) :
    str (other.str),
    locFile (other.locFile),
    line (other.line),
    column (other.column)
{}

Word::Word () :	
    str (""),
    locFile (""),
    line (0),
    column (0)
{}

void Word::setLocus (location_t locus) {
    if (locus != UNKNOWN_LOCATION) {
	this-> locFile = LOCATION_FILE (locus);
	this-> line = LOCATION_LINE (locus);
	this-> column = LOCATION_COLUMN (locus);
    }
}

void Word::setLocus (std::string filename, ulong line, ulong column) {
    this-> locFile = filename;
    this-> line = line;
    this-> column = column;
}

location_t Word::getLocus () const {
    linemap_add (line_table, LC_ENTER, 0, this-> locFile.c_str (), this-> line);	
    linemap_line_start (line_table, this-> line, 0);
    auto ret = linemap_position_for_column (line_table, this-> column);
    linemap_add (line_table, LC_LEAVE, 0, NULL, 0);
    return ret;
}


std::string Word::toString () const {
    if (this-> isEof ()) return ":\u001B[32meof\u001B[0m()";
    auto reset = "\u001B[0m";
    auto purple = "\u001B[36m";
    auto green = "\u001B[32m";
    std::stringstream out;
    out << ":" << green << this-> str << reset << "(" 
	<< this-> locFile.c_str () << " -> "
	<< purple<< this-> line
	<< reset << ", "
	<< purple << this-> column << reset << ")";
    return out.str ();
}

std::string Word::getFile () const {
    return this-> locFile;
}

bool Word::isToken () const {
    auto mem = Token::members ();
    return std::find (mem.begin (), mem.end (), this-> str) != mem.end ();
}
