#pragma once

#include "Ast.hh"
#include "Tree.hh"

namespace Syntax {

    struct Instruction : Ast {

	Instruction (Lexical::TokenPtr ptr, AstEnums::AstEnum type)
	    : Ast (ptr, type)
	{}

	Instruction (Lexical::TokenPtr ptr)
	    : Ast (ptr, AstEnums::INSTRUCTION)
	{}

	virtual Instruction* instruction();

	virtual Ymir::Tree statement ();
	
	virtual void print (int nb = 0);
	
	virtual ~Instruction () {}
    };    

    typedef Instruction* InstructionPtr;

};
