#pragma once

#include "Expression.hh"
#include "../errors/_.hh"
#include "../semantic/_.hh"
#include "../syntax/Word.hh"

namespace semantic {
    class IInfoType;
    typedef IInfoType* InfoType;
}

namespace syntax {

    class IUnary : public IExpression {
	Expression elem;
	semantic::InfoType type;
	
    public:

	IUnary (Word word, Expression elem);

	Expression expression () override;

	Ymir::Tree toGeneric () override;
	
	void print (int nb = 0) override;	
    };

    typedef IUnary* Unary;

}
