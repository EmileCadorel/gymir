#pragma once

#include <ymir/semantic/generator/Type.hh>

namespace semantic {

    namespace generator {

	class Pointer : public Type {
	public : 

	    static ulong INIT;
	    
	    static std::string INIT_NAME;
	    
	    static std::string INNER_NAME;
	    
	private : 

	    friend Generator;

	    Pointer ();

	    Pointer (const lexing::Word & loc, const Generator & innerType);

	public : 


	    static Generator init (const lexing::Word & loc, const Generator & innerType);
	    
	    Generator clone () const override;

	    /**
	     * \brief Mandatory function used inside proxy design pattern for dynamic casting
	     */
	    bool isOf (const IGenerator * type) const override;	    

	    /**
	     * \return is this symbol the same as other (no only address, or type)
	     */
	    bool equals (const Generator & other) const override;

	    /**
	     * \return is this type compatible with other (assuming other is a type)
	     */
	    bool isCompatible (const Generator & other) const override;

	    /**
	     * \return true, pointer points to data in foreign place, and need to give the ownership
	     */
	    bool needExplicitAlias () const override;
	    
	    /** 
	     * \return the name of the type formatted
	     */
	    std::string typeName () const override;

	    
	};

    }
    
}
