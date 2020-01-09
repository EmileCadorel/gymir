#pragma once

#include <ymir/syntax/Declaration.hh>
#include <ymir/syntax/Expression.hh>
#include <ymir/lexing/Word.hh>
#include <vector>

namespace syntax {

    class Mixin : public IDeclaration {
    	
	/** the name of the vars */
	Expression _mixin;

	/** The name of the trait */
	lexing::Word _location;

	std::vector <Declaration> _declarations;
	
    private :

	friend Declaration;

	Mixin ();

    public :

	/** 
	 * \brief Create an empty use
	 */
	static Declaration init ();

	/** 
	 * \brief Create a use from copy
	 * \param mixin the mixin to copy
	*/
	static Declaration init (const Mixin & mixin);

	/**
	 * \brief Create a new trait
	 * \param location the location of the use
	 * \param mixin the mixin to use
	 */
	static Declaration init (const lexing::Word & location, const Expression & mixin, const std::vector <Declaration> & declarations);

	/**
	 * Mandatory function for proxy polymoprhism system
	 */
	Declaration clone () const override;

	/**
	 * Mandatory function for dynamic casting
	 */
	bool isOf (const IDeclaration * type) const override;

	const lexing::Word & getLocation () const;

	const Expression & getMixin () const;

	const std::vector <Declaration> & getDeclarations () const;
	
	void treePrint (Ymir::OutBuffer & stream, int i) const override;
	
    };
    


}
