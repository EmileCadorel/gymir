#pragma once

#include "Declaration.hh"
#include "../errors/_.hh"
#include "../semantic/_.hh"
#include "../syntax/Word.hh"

namespace syntax {

    class IImport : public IDeclaration {
	Word ident;
	std::vector <Word> params;

    public:

	IImport (Word ident, std::vector <Word> params) :
	    ident (ident),
	    params (params)
	{}

	void print (int nb = 0) override {
	    printf ("\n%*c<Import> %s",
		    nb, ' ',
		    this-> ident.toString ().c_str ()
	    );
	    for (auto it : this-> params) {
		printf ("%s", it.toString ().c_str ());
	    }
	}
	
    };

    typedef IImport* Import;
}