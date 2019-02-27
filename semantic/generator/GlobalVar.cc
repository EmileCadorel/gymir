#include <ymir/semantic/generator/GlobalVar.hh>

namespace semantic {
    namespace generator {

	GlobalVar::GlobalVar () :
	    IGenerator (lexing::Word::eof (), ""),
	    _type (Generator::empty ()),
	    _value (Generator::empty ())
	{}

	GlobalVar::GlobalVar (const lexing::Word & location, const std::string & name, const Generator & type, const Generator & value) :
	    IGenerator (location, name),
	    _type (type),
	    _value (value)
	{}

	Generator GlobalVar::init (const lexing::Word & location, const std::string & name, const Generator & type, const Generator & value) {
	    return Generator {new (Z0) GlobalVar (location, name, type, value)};
	}

	Generator GlobalVar::clone () const {
	    return Generator {new (Z0) GlobalVar (*this)};
	}

	const Generator & GlobalVar::getType () const {
	    return this-> _type;
	}
	
	const Generator & GlobalVar::getValue () const {
	    return this-> _value;
	}
		
	bool GlobalVar::isOf (const IGenerator * type) const {
	    auto vtable = reinterpret_cast <const void* const *> (type) [0];
	    GlobalVar thisGlobalVar; // That's why we cannot implement it for all class
	    if (reinterpret_cast <const void* const *> (&thisGlobalVar) [0] == vtable) return true;
	    return IGenerator::isOf (type);	
	}

	bool GlobalVar::equals (const Generator & other) const {
	    if (!other.is <GlobalVar> ()) return false;
	    else 
		return other.getName () == this-> getName ();
	}

    }
}
