#include "errors/Error.hh"
#include "errors/Languages.hh"
#include <sstream>
#include <iostream>
#include <string>

namespace Ymir {    	
    using namespace Private;
    
    const char* Error::RESET = "\u001B[0m";
    const char* Error::PURPLE = "\u001B[1;35m";
    const char* Error::BLUE = "\u001B[1;36m";
    const char* Error::YELLOW = "\u001B[1;33m";
    const char* Error::RED = "\u001B[1;31m";
    const char* Error::GREEN = "\u001B[1;32m";
    const char* Error::BOLD = "\u001B[1;50m";
    
    unsigned long Error::nb_errors (0);
    
    Error Error::__instance__;

    std::string substr (std::string& x, ulong beg, ulong end) {
	return x.substr (beg, end - beg);
    }
    
    std::string readln (FILE * i) {
	unsigned long max = 255;
	std::string final = "";
	while (1) {
	    char * buf = new char[max];
	    char * aux = fgets(buf, max, i);
	    if (aux == NULL) return "";
	    std::string ret = std::string (buf);
	    final += ret;
	    free (buf);
	    if (ret.size () != max - 1) return final;
	    else max *= 2;      
	}
    }  
    
    std::string getLine (location_t locus) {
	auto file = fopen (LOCATION_FILE (locus), "r");
	std::string cline;
	for (auto it = 0 ; it < LOCATION_LINE (locus) ; it ++) {
	    cline = readln (file);
	}
	return cline;
    }
	
    ulong computeMid (std::string& mid, std::string word, std::string line, ulong begin, ulong end) {
	auto end2 = begin;
	for (auto it = 0 ; it < (int) std::min (word.size (), end - begin); it ++) {
	    if (word [it] != line [begin + it]) break;
	    else end2 ++;
	}
	mid = line.substr (begin, end2);
	return end2;
    }	
    
    std::string center (std::string toCenter, ulong nb, char concat) {
	std::ostringstream ss;
	nb = nb - toCenter.length ();
	for (int i = 0 ; i < (int) nb / 2 ; i++) {
	    ss << (char) concat;
	}
	ss << toCenter;
	for (int i = 0 ; i < (int) (nb - nb / 2) ; i++)
	    ss << (char) concat;
	return ss.str ();
    }

    std::string rightJustify (std::string toJustify, ulong nb, char concat) {
	nb = nb - toJustify.length ();
	std::ostringstream ss;
	for (int i = 0 ; i < (int) nb; i++) ss << concat;
	ss << toJustify;
	return ss.str ();
    }
    
    void addLine (std::ostringstream &ss, Word word) {
	auto locus = word.getLocus ();
	auto line = getLine (locus);
	if (line.length () > 0) {
	    std::string leftLine = center (format ("%d", LOCATION_LINE (locus)), 3, ' ');
	    auto padd = center ("", leftLine.length (), ' ');
	    ss << format ("\n%s --> %s:(%d,%d)%s\n%s%s | %s\n", 
			  Error::BOLD,
			  LOCATION_FILE (locus),
			  LOCATION_LINE (locus),
			  LOCATION_COLUMN (locus),
			  Error::RESET,
			  Error::BOLD,
			  padd.c_str (),
			  Error::RESET
	    );

	    auto column = LOCATION_COLUMN (locus);
	    ss << format ("%s%s | %s%s%s%s%s%s",
			  Error::BOLD,
			  leftLine.c_str (),
			  Error::RESET,
			  line.substr (0, column - 1).c_str (),
			  Error::YELLOW,
			  substr (line, column - 1, column + word.getStr ().length () - 1).c_str (),
			  Error::RESET,
			  substr (line, column + word.getStr ().length () - 1, line.length ()).c_str ()
	    );

	    if (line [line.length () - 1] != '\n') ss << "\n";	    	    
	    ss << format ("%s%s | %s",
			  Error::BOLD,
			  padd.c_str (),
			  Error::RESET
	    );
	    for (auto it = 0 ; it < column - 1 ; it ++) {
		if (line [it] == '\t') ss << "\t";
		else ss << ' ';
	    }
	    
	    ss << rightJustify ("", word.getStr ().length (), '^');
	    ss << "\n";
	} else {
	    ss << format ("\n%sError%s : %s\n",
			  Error::RED,
			  Error::RESET,
			  getString (EndOfFile)
	    );
	}
    }

    void addLine (std::ostream &ss, Word word, Word word2) {}

    void addLine (std::ostream &ss, Word word, ulong index, ulong index2) {}

    std::string addLine (std::string in, Word word) {
	std::ostringstream ss;
	ss << in;
	addLine (ss, word);
	return ss.str ();
    }    


    namespace Private {
	const char * getString (ErrorType type, Language ln) {
	    auto strings = Languages::getLanguage (ln);
	    return strings [type];	
	}
    }
    

    void Error::activeError (bool isActive) {
	__isEnable__ = isActive;
	if (isActive) {
	    __caught__.clear ();
	}
    }

    void Error::notATemplate (Word word) {
	auto str = getString (NotATemplate);
	std::string msg = format (str, YELLOW, word.getStr ().c_str, RESET);
	msg = std::string (RED) + "Error" + std::string (RESET) + " : " + msg;
	msg = addLine (msg, word);
	ErrorMsg errorMsg (msg, false, false);
	if (__isEnable__) {
	    Error::instance ().nb_errors ++;
	    printf ("%s", msg.c_str ());
	} else __caught__.push_back (errorMsg);
    }
    
    void Error::takeATypeAsTemplate (Word word) {
	auto msg = getString (TakeAType);
	msg = std::string (RED) + "Error" + std::string (RESET) + " : " + std::string (msg);
	msg = addLine (msg, word);
	ErrorMsg errorMsg (msg, false, false);
	if (__isEnable__) {
	    Error::instance ().nb_errors ++;
	    printf ("%s", msg.c_str ());
	} else __caught__.push_back (errorMsg);
    }

    void Error::syntaxError (Word word) {
	auto str = getString (SyntaxError2);
	auto msg = format (str, YELLOW, word.getStr ().c_str, RESET);
	msg = std::string (RED) + "Error" + std::string (RESET) + " : " + std::string (msg);
	msg = addLine (msg, word);
	ErrorMsg errorMsg (msg, false, false);
	if (__isEnable__) {
	    Error::instance ().nb_errors ++;
	    printf ("%s", msg.c_str ());
	} else __caught__.push_back (errorMsg);
    }

    void Error::syntaxError (Word word, const char * expected) {
	auto str = getString (SyntaxError);
	auto msg = format (str, expected, YELLOW, word.getStr ().c_str, RESET);
	msg = std::string (RED) + "Error" + std::string (RESET) + " : " + std::string (msg);
	msg = addLine (msg, word);
	ErrorMsg errorMsg (msg, false, false);
	if (__isEnable__) {
	    Error::instance ().nb_errors ++;
	    printf ("%s", msg.c_str ());
	} else __caught__.push_back (errorMsg);
    }

    void Error::syntaxError (Word word, Word word2) {
	auto str = getString (SyntaxError);
	auto msg = format (str, word2.getStr().c_str (),
			   YELLOW, word.getStr ().c_str, RESET);
	
	msg = std::string (RED) + "Error" + std::string (RESET) + " : " + std::string (msg);
	msg = addLine (msg, word);
	ErrorMsg errorMsg (msg, false, false);
	if (__isEnable__) {
	    Error::instance ().nb_errors ++;
	    printf ("%s", msg.c_str ());
	} else __caught__.push_back (errorMsg);
    }
    
    void Error::escapeError (Word word) {
	auto msg = std::string (getString (EscapeChar));	
	msg = std::string (RED) + "Error" + std::string (RESET) + " : " + std::string (msg);
	msg = addLine (msg, word);
	ErrorMsg errorMsg (msg, false, false);
	if (__isEnable__) {
	    Error::instance ().nb_errors ++;
	    printf ("%s", msg.c_str ());
	} else __caught__.push_back (errorMsg);
    }
    
    void Error::endOfFile () {
	auto msg = std::string (getString (EndOfFile));
	
	msg = std::string (RED) + "Error" + std::string (RESET) + " : " + std::string (msg);
	msg = addLine (msg, word);
	ErrorMsg errorMsg (msg, false, false);
	if (__isEnable__) {
	    Error::instance ().nb_errors ++;
	    printf ("%s", msg.c_str ());
	} else __caught__.push_back (errorMsg);
    }
   
    void Error::unterminated (Word word) {
	auto str = getString (Unterminated);
	auto msg = format (str, YELLOW, word.getStr ().c_str, RESET);	
	msg = std::string (RED) + "Error" + std::string (RESET) + " : " + std::string (msg);
	msg = addLine (msg, word);
	ErrorMsg errorMsg (msg, false, false);
	if (__isEnable__) {
	    Error::instance ().nb_errors ++;
	    printf ("%s", msg.c_str ());
	} else __caught__.push_back (errorMsg);
    }

    void Error::templateSpecialisation (Word word, Word word2) {
	auto str = getString (TemplateSpecialisation);
	auto msg = format (str, YELLOW, word.getStr ().c_str, RESET);	
	msg = std::string (RED) + "Error" + std::string (RESET) + " : " + std::string (msg);
	msg = addLine (msg, word);

	auto msg2 = std::string (getString (And));
	auto aux = std::string (BLUE) + "Note" + std::string (RESET) + " : " + aux;
	aux = addLine (aux, word2);
	
	ErrorMsg errorMsg (msg + aux, false, false);
	if (__isEnable__) {
	    Error::instance ().nb_errors ++;
	    printf ("%s", msg.c_str ());
	} else __caught__.push_back (errorMsg);
    }


    
    
    

    
}
