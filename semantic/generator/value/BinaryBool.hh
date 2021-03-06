#pragma once

#include <ymir/semantic/generator/value/Binary.hh>

namespace semantic {

    namespace generator {

	/**
	 * \struct Binary 
	 * Binary operations on ints
	 */
	class BinaryBool : public Binary {
	private :

	    friend Generator;
	    
	    BinaryBool ();

	    BinaryBool (const lexing::Word & loc, Operator op, const Generator & type, const Generator & left, const Generator & right);

	public :

	    /**
	     * \brief Generate a new Binary on int
	     * \warning left and right operand must generate int typed values
	     */
	    static Generator init (const lexing::Word & loc, Operator op, const Generator & type, const Generator & left, const Generator & right);
	    
	    /** 
	     * \brief Mandatory function used inside proxy design pattern
	     */
	    Generator clone () const  override;

	};
	
    }

}
