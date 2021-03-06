#include <ymir/semantic/generator/value/SizeOf.hh>

namespace semantic {

    namespace generator {

	SizeOf::SizeOf () :
	    Value (),
	    _who (Generator::empty ())
	{}

	SizeOf::SizeOf (const lexing::Word & loc, const Generator & type, const Generator & who) :
	    Value (loc, type),
	    _who (who)
	{
	    // Sizeof never evaluate the value, the size is known at compile time
	}
	
	Generator SizeOf::init (const lexing::Word & loc, const Generator & type, const Generator & who) {
	    return Generator {new (NO_GC) SizeOf (loc, type, who)};
	}
    
	Generator SizeOf::clone () const {
	    return Generator {new (NO_GC) SizeOf (*this)};
	}

	bool SizeOf::equals (const Generator & gen) const {
	    if (!gen.is <SizeOf> ()) return false;
	    auto bin = gen.to<SizeOf> ();	    
	    return this-> _who.equals (bin._who);
	}

	const Generator & SizeOf::getWho () const {
	    return this-> _who;
	}

	std::string SizeOf::prettyString () const {
	    return Ymir::format ("sizeof %", this-> _who.prettyString ());
	}
    }
    
}
