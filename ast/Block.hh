#pragma once

#include "Instruction.hh"
#include <ymir/utils/Array.hh>
#include <stdio.h>
#include "../errors/_.hh"
#include "../syntax/Word.hh"

namespace syntax {

    class IDeclaration;
    typedef IDeclaration* Declaration;
    
    class IBlock : public IInstruction {

	Word ident;
	std::vector <Declaration> decls;
	std::vector <Instruction> insts;

    public :

	IBlock (Word word, std::vector <Declaration> decls, std::vector <Instruction> insts) :
	    IInstruction (word),
	    decls (decls),
	    insts (insts)
	{
	    this-> ident.setEof ();
	}

	Word& getIdent ();

	Instruction instruction () override;
	
	IBlock* block ();

	IBlock* blockWithoutEnter ();
	
	void print (int nb = 0) override; 	
    };

    typedef IBlock* Block;
    
}
