#pragma once

#include "Expression.hh"
#include "../errors/_.hh"
#include "../semantic/_.hh"
#include "../syntax/Word.hh"
#include <cmath>

namespace syntax {

    enum class FixedConst {
	BYTE, UBYTE,
	SHORT, USHORT,
	INT, UINT,
	LONG, ULONG
    };

    bool isSigned (FixedConst ct);
    
    class IFixed : public IExpression {
	FixedConst type;
	
    public:
	
	IFixed (Word word, FixedConst type) :
	    IExpression (word),
	    type (type)
	{}

	void print (int nb = 0) override {
	    printf ("\n%*c<Fixed> %s",
		    nb, ' ',
		    this-> token.toString ().c_str ()
	    );	    
	}	
    };

    typedef unsigned char ubyte;
    
    class IChar : public IExpression {
	ubyte code;

    public:

	IChar (Word word, ubyte code) :
	    IExpression (word),
	    code (code) {
	}

	void print (int nb = 0) override {
	    printf ("\n%*c<Char> %s %d:(%c)",
		    nb, ' ',
		    this-> token.toString ().c_str (),
		    this-> code, (char) (this-> code)
	    );
	}
	
    };

    class IFloat : public IExpression {

	std::string suite;
	std::string totale;

    public:

	IFloat (Word word) : IExpression (word) {
	    this-> totale = "0." + this-> token.getStr ();
	}
	
	IFloat (Word word, std::string suite) :
	    IExpression (word),
	    suite (suite) {
	    this-> totale = this-> token.getStr () + "." + suite;
	}

	void print (int nb = 0) override {
	    printf ("\n%*c<Float> %s [%s]",
		    nb, ' ', this-> token.toString ().c_str (),
		    this-> totale.c_str ()
	    );
	}
	
    };


    class IString : public IExpression {

	std::string content;

    public :

	IString (Word word, std::string content) :
	    IExpression (word),
	    content (content)
	{}

	void print (int nb = 0) override {
	    printf ("\n%*c<String> %s : [%s]",
		    nb, ' ',
		    this-> token.toString ().c_str (),
		    this-> content.c_str ()
	    );	    
	}
	
    private:

	static short getFromOc (std::string elem, ulong & index) {
	    auto fst = elem [index + 1];
	    index += 2;
	    printf ("%c\n", fst);
	    if (fst < '0' || fst > '7') return -1;
	    int size = 1;
	    for (int i = index ; i < (int) elem.size (); i++) {
		if (elem [i] < '0' ||  elem [i] > '7') break;
		else size ++;
	    }
	    size = size > 5 ? 5 : size;

	    short total = 0;
	    int current = size - 1;	    
	    for (int i = index - 1 ; i < (int) (index + size - 1) ; i ++, current --) {
		total += pow (8, current) * (elem [i] - '0');
	    }

	    index += size - 1;
	    return total;
	}

	static short getFromLX (std::string elem, ulong & index) {
	    index += 2;
	    int size = 0;
	    for (int i = index ; i < (int) elem.size (); i++) {
		if ((elem [i] < '0' ||  elem [i] > '9') &&
		    (elem [i] < 'a' || elem [i] > 'f')
		) break;
		else size ++;
	    }
	    
	    size = size > 2 ? 2 : size;
	    short total = 0;
	    int current = size - 1;	    
	    for (int i = index ; i < (int) (index + size) ; i ++, current --) {
		if (elem [i] > 'a')
		    total += pow (16, current) * (elem [i] - 'a' + 10);
		else
		    total += pow (16, current) * (elem [i] - '0');
	    }
	    index += size;
	    return total;
	}

	static int find (const char* where, char what) {
	    int i = 0;
	    while (*where != '\0') {
		if (what == *where) return i;
		i ++;
		where++;
	    }
	    return -1;
	}

    public:
	
	static short isChar (std::string &current, ulong &index) {
	    static char escape[] = {'a', 'b', 'f', 'n', 'r', 't', 'v', '\\', '\'', '"', '?', '\0'};
	    static short values[] = {7, 8, 12, 10, 13, 9, 11, 92, 39, 34, 63};
	    if (current [index] != '\\') {
		index ++;
		return current [index - 1];
	    } else {
		if (index + 1 >= current.size ()) return -1;
		auto next = current [index + 1];
		auto p = find (escape, next);
		if (p != -1) {
		    index += 2;
		    return values [p];
		} else if (next == 'x') {
		    return getFromLX (current, index);		    
		} else return getFromOc (current, index);
	    }
	}
	
	static std::string validate (Word, std::string content) {
	    std::stringstream buf;
	    for (ulong i = 0 ; i < content.size ();) {
		auto c = isChar (content, i);
		if (c != -1) buf << (char) c;
		else {
		    //TODO error
		}
	    }
	    return buf.str ();	    
	}
	
    };
        
    class IBool : public IExpression {
    public :
	IBool (Word token) : IExpression (token) {
	}

	void print (int nb = 0) override {
	    printf ("\n%*c<Bool> %s",
		    nb, ' ',
		    this-> token.toString ().c_str ()
	    );
	}	
    };    

    class INull : public IExpression {
    public:

	INull (Word token) : IExpression (token) {
	}

	void print (int nb = 0) override {
	    printf ("\n%*c<Null> %s",
		    nb, ' ',
		    this-> token.toString ().c_str ()
	    );
	}
	
    };
    
    typedef IFixed* Fixed;
    typedef IChar* Char;
    typedef IFloat* Float;
    typedef IString* String;
    typedef IBool* Bool;
    typedef INull* Null;
}