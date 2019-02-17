#include <ymir/semantic/generator/value/Affect.hh>

namespace semantic {

    namespace generator {

	Affect::Affect () :
	    Value (),
	    _who (Generator::empty ()),
	    _value (Generator::empty ())
	{}

	Affect::Affect (const lexing::Word & loc, const Generator & type, const Generator & who, const Generator & value) :
	    Value (loc, type),
	    _who (who),
	    _value (value)
	{}	
	
	Generator Affect::init (const lexing::Word & loc, const Generator & type, const Generator & who, const Generator & value) {
	    return Generator {new Affect (loc, type, who, value)};
	}
    
	Generator Affect::clone () const {
	    return Generator {new (Z0) Affect (*this)};
	}

	bool Affect::isOf (const IGenerator * type) const {
	    auto vtable = reinterpret_cast <const void* const *> (type) [0];
	    Affect thisValue; // That's why we cannot implement it for all class
	    if (reinterpret_cast <const void* const *> (&thisValue) [0] == vtable) return true;
	    return Value::isOf (type);	
	}

	bool Affect::equals (const Generator & gen) const {
	    if (!gen.is <Affect> ()) return false;
	    auto bin = gen.to<Affect> ();	    
	    return this-> _who.equals (bin._who) &&
		this-> _value.equals (bin._value);
	}

	const Generator & Affect::getWho () const {
	    return this-> _who;
	}

	const Generator & Affect::getValue () const {
	    return this-> _value;
	}
	
    }
    
}