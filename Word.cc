#include "syntax/Word.hh"
#include <sstream>


std::string Word::toString () {
    if (this-> isEof ()) return ":\u001B[32meof\u001B[0m()";
    auto reset = "\u001B[0m";
    auto purple = "\u001B[36m";
    auto green = "\u001B[32m";
    std::stringstream out;
    out << ":" << green << this-> str << reset << "(" 
	<< LOCATION_FILE (this-> locus) << " -> "
	<< purple<< LOCATION_LINE (this-> locus)
	<< reset << ", "
	<< purple << LOCATION_COLUMN (this-> locus) << reset << ")";
    return out.str ();
}

   