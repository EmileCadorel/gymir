#pragma once

#include <ymir/semantic/generator/Value.hh>

namespace semantic {
    namespace generator {

	class ProtoVar : public Value {	    

	    Generator _value;
	    
	    bool _isMutable;

	    int _nbConsume;

	    bool _isSelf;
	    
	private :

	    friend Generator;
	    
	    ProtoVar ();

	    ProtoVar (const lexing::Word & location, const Generator & type, const Generator & value, bool isMutable, int nb_consume, bool is_self);

	public :

	    /**
	     * \brief Create a param var 
	     * \param location the location of the param var (for debug info)
	     * \param type the type of the var
	     */
	    static Generator init (const lexing::Word & location, const Generator & type, const Generator & value, bool isMutable, int nb_consume, bool is_self);
	    
	    /** 
	     * \brief Mandatory function used inside proxy design pattern
	     */
	    virtual Generator clone () const ;

	    /**
	     * \brief Mandatory function used inside proxy design pattern for dynamic casting
	     */
	    virtual bool isOf (const IGenerator * type) const ;	    

	    /**
	     * \return is this symbol the same as other (no only address, or type)
	     */
	    virtual bool equals (const Generator & other) const ;	    
	    
	    /**
	     * \return has this var been declared mutable ?
	     */
	    bool isMutable () const;

	    /** 
	     * \return the value of the proto var
	     */
	    const Generator & getValue () const;

	    /**
	     * \return the number of arguments this proto var consume (for variadic templates)
	     */
	    int getNbConsume () const;

	    /**
	     * this protovar define a self class var
	     */
	    bool isSelf () const;
	    
	    std::string prettyString () const override;
	};
	
    }
}
