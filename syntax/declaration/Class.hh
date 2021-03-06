#pragma once

#include <ymir/syntax/Declaration.hh>
#include <ymir/syntax/Expression.hh>
#include <ymir/syntax/declaration/Function.hh>
#include <ymir/lexing/Word.hh>
#include <ymir/syntax/visitor/Keys.hh>
#include <vector>

namespace syntax {

    class Class : public IDeclaration {

	/** The type overriden by the class (can be empty ()) */
	Expression _over;

	/**
	 * The inner declaration of the class
	 * It could be : 
	 * - function 
	 * - vardecl (inside a wrapper, as VarDecl is an expression)
	 * constructor and destructor are function with keyword name (self, or self~), that return void
	 */
	std::vector <Declaration> _innerDeclaration;

	/**
	 * The attributes of the class
	 * Must be empty, or only one word named /abs/
	 */
	std::vector <lexing::Word> _attributes;

	bool _isAbstract = false;
	
    private :

	friend Declaration;

	Class ();

	Class (const lexing::Word & name, const std::string & comment, const Expression & over, const std::vector <Declaration> & decls, const std::vector <lexing::Word> & attributes);
	
    public :

	/** 
	 * \brief Create a class from copy
	 * \param use the class to copy
	*/
	static Declaration init (const Class & cl);

	/**
	 * \brief Create a new trait
	 * \param name the name of the class
	 * \param over the type that this class override (may be empty)
	 * \param decls the inner declaration
	 */
	static Declaration init (const lexing::Word & name, const std::string & comment, const Expression & over, const std::vector <Declaration> & decls, const std::vector <lexing::Word> & attributes);
	
	/**
	 * \return the expression defining the ancestor of the class definition
	 */
	const Expression & getAncestor () const;

	/**
	 * \return all the declaration inside the class 
	 */
	const std::vector <Declaration> & getDeclarations () const;

	/**
	 * \return the list of attributes of the class
	 */
	const std::vector<lexing::Word> & getAttributes () const;

	/**
	 * I this class abstract
	 */
	bool isAbstract () const;
    };
    


}
