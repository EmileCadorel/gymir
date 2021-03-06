#pragma once

#include <ymir/syntax/Expression.hh>
#include <ymir/semantic/generator/Value.hh>

namespace semantic {

    namespace generator {

	/**
	 * \struct TemplateSyntaxWrapper
	 * Intermediate generator created by template specialisation
	 * It is a syntaxic expression but a fake one, as it only store a semantic generator
	 */
	class TemplateSyntaxWrapper : public syntax::IExpression {

	    Generator _content;
	    
	private :

	    friend syntax::Expression;

	    TemplateSyntaxWrapper ();

	    TemplateSyntaxWrapper (const lexing::Word& loc, const Generator& gen);

	public : 

	    static syntax::Expression init (const lexing::Word & location, const Generator& gen);

	    void treePrint (Ymir::OutBuffer & stream, int i) const override;
	    

	    /**
	     * \return the content wrapped inside this fake syntax expression
	     */
	    const Generator& getContent () const;

	    std::string prettyString () const override;
	    
	};
	
    }    

}
