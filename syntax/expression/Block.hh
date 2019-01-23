#pragma once

#include <ymir/syntax/Expression.hh>
#include <ymir/syntax/Declaration.hh>

namespace syntax {

    /**
     * \struct Block
     * Representation of an binary operator 
     * \verbatim
     binary := expression op(':' expression)? expression 
     \endverbatim 
     */
    class Block : public IExpression {

	/** The location of the allocation */
	lexing::Word _begin;

	/** The end of the block */
	lexing::Word _end;

	/** The content of the block */
	std::vector <Expression> _content;

	/** The declaration done inside the block */
	std::vector <Declaration> _decls;
	
    private :

	friend Expression;

	Block ();

    public :

	static Expression init (const Block & blk);

	static Expression init (const lexing::Word & op, const lexing::Word & end, const std::vector <Declaration> & decls, const std::vector <Expression> & conetent);

	Expression clone () const override;

	void treePrint (Ymir::OutBuffer & stream, int i) const override;

	bool isOf (const IExpression * type) const override;
	
    };    

}