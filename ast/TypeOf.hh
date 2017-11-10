#pragma once

#include "Expression.hh"
#include "../errors/_.hh"
#include "../semantic/_.hh"
#include "../syntax/Word.hh"

namespace syntax {

    class ITypeOf : public IExpression {
	Expression expr;

    public:

	ITypeOf (Word begin, Expression expr) :
	    IExpression (begin),
	    expr (expr)
	{}

	void print (int nb = 0) override {
	    printf ("\n%*c<TypeOf> %s",
		    nb, ' ',
		    this-> token.toString ().c_str ()
	    );
	    this-> expr-> print (nb + 4);
	}
	
    };

    typedef ITypeOf* TypeOf;
    
}
