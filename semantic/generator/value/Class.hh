#pragma once

#include <ymir/semantic/generator/Value.hh>
#include <ymir/semantic/generator/type/ClassRef.hh>

#include <ymir/semantic/Symbol.hh>

namespace semantic {
    namespace generator {

	class Class : public Value {
	public :
	    
	    enum MethodProtection {
		PRV_PARENT = 0,
		PRV,
		PROT,
		PUB
	    };
	    
	private : 
	    
	    std::weak_ptr<ISymbol> _ref;

	    std::vector <generator::Generator> _fields;

	    std::vector <generator::Generator> _localFields;

	    std::vector <generator::Generator> _vtable;

	    std::vector <MethodProtection> _prots;

	    Generator _classRef;

	    generator::Generator _destructor;

	    bool _finalized;
	    
	private :

	    friend Generator;

	    friend ClassRef;
	    
	    Class ();

	    Class (const lexing::Word & loc, const Symbol & ref, const Generator & classRef, bool finalized);

	public : 
	    
	    static Generator init (const lexing::Word & loc, const Symbol & ref, const Generator & classRef);

	    static Generator initFields (const Class & other, const std::vector <generator::Generator> & fields, const std::vector <generator::Generator> & localFields, bool finalized);

	    static Generator initVtable (const Class & other, const std::vector <generator::Generator> & vtable, const std::vector <MethodProtection> & prots, const generator::Generator & destructor, bool finalized);
	    
	    
	    Generator clone () const override;
	    
	    /**
	     * \return is this symbol the same as other (no only address, or type)
	     */
	    bool equals (const Generator & other) const override;

	    /**
	     * \return the name of the structure (the symbolic one, meaning its space name)
	     */
	    std::string getName () const;

	    /**
	     * \return the fields of the class
	     */
	    const std::vector <generator::Generator> & getFields () const;


	    /**
	     * \return the fields that have been declared in the class and not inside ancestors
	     */
	    const std::vector <generator::Generator> & getLocalFields () const;
	    
	    /**
	     * \return the field type of the field named name, or empty if this class does not have a field named name
	     * \warning this is only applicable for fields not for methods
	     * \warning and this will only search within the fields of the class, not within the fields of its ancestor
	     */
	    generator::Generator getFieldType (const std::string & name) const;
	    
	    /**
	     * \brief the fields that are mark as protected (default field protection, it will remove the private ones from the search)
	     * \brief this is used to access the field of an ancestor class
	     * \brief Same as getFieldType, this will perform a search only on the fields of this class, and not on its parent class
	     */
	    generator::Generator getFieldTypeProtected (const std::string & name) const;

	    /**
	     * \brief the fields that are mark as public 
	     * \brief this is used to access the field of a class from extern context
	     * \brief Same as getFieldType, this will perform a search only on the fields of this class, and not on its parent class
	     */
	    generator::Generator getFieldTypePublic (const std::string & name) const;

	    /**
	     * \brief Set pub and prot according to the protection of the field str
	     */
	    void getFieldProtection (const std::string & str, bool & pub, bool & prot) const;	    
	    
	    /**
	     * \return the vtable of the class
	     */
	    const std::vector <generator::Generator> & getVtable () const;

	    /**
	     * \return the protection of the vtable 
	     */
	    const std::vector <MethodProtection> & getProtectionVtable () const;

	    /**
	     * \return the destructor of the class 
	     */
	    const generator::Generator & getDestructor () const;
	    
	    /**
	     * 
	     */
	    std::string prettyString () const override;

	    /**
	     * \return the symbol responsible of the declaration of this structure prototype
	     */
	    Symbol getRef () const;
	    
	    /**
	     * \return a classRef, containing the information about this class and ancestor
	     */
	    const Generator & getClassRef () const;

	    /**
	     * The validation of the class is finished
	     */
	    bool isFinalized () const;
	    
	};
    }
    
}
