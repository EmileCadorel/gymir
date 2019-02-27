#pragma once

#include <ymir/semantic/generator/Type.hh>

namespace semantic {
    namespace generator {

	class Float : public Type {
	public :

	    static std::vector <std::string> NAMES;

	    /// Value of floating point are encoded in string to prevent approximation
	    static std::string INIT;

	    static std::string NOT_A_NUMBER;
	    
	private : 

	    /** The precision of this float type (32/64/0) */
	    /** 0 means the biggest representable float */
	    int _size; 

	private :

	    friend Generator;

	    Float ();	    

	    Float (const lexing::Word & loc, int size);
	    
	public :

	    static Generator init (const lexing::Word & loc, int size);

	    Generator clone () const override;

	    /**
	     * \brief Mandatory function used inside proxy design pattern for dynamic casting
	     */
	    bool isOf (const IGenerator * type) const override;	    

	    /**
	     * \return is this symbol the same as other (no only address, or type)
	     */
	    bool equals (const Generator & other) const override;
	    
	    std::string typeName () const override;

	    int getSize () const;
	    
	};
       
    }
}
