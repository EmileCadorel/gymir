#pragma once

#include <ymir/syntax/Expression.hh>

namespace syntax {

    /**
     * \struct StructVar
     */
    class StructVar : public IExpression {	
    private :

	friend Expression;

	StructVar ();
	
	StructVar (const lexing::Word & loc);

    public :

	static Expression init (const lexing::Word & token);

	bool isOf (const IExpression * type) const override;

	void treePrint (Ymir::OutBuffer & stream, int i) const override;

	std::string prettyString () const override;
	
    };    

}
