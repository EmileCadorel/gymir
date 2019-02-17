#include <ymir/semantic/generator/type/Slice.hh>
#include <ymir/utils/OutBuffer.hh>
#include <ymir/semantic/generator/type/Array.hh>

namespace semantic {
    namespace generator {
	
	Slice::Slice () :
	    Type ()
	{
	    this-> isComplex (true);
	}

	Slice::Slice (const lexing::Word & loc, const Generator & inner) :
	    Type (loc, loc.str)
	{
	    this-> isComplex (true);
	    this-> setInners ({inner});
	}

	Generator Slice::init (const lexing::Word & loc, const Generator & inner) {
	    return Generator {new (Z0) Slice (loc, inner)};
	}

	Generator Slice::clone () const {
	    return Generator {new (Z0) Slice (*this)};
	}
		
	bool Slice::isOf (const IGenerator * type) const {
	    auto vtable = reinterpret_cast <const void* const *> (type) [0];
	    Slice thisSlice; // That's why we cannot implement it for all class
	    if (reinterpret_cast <const void* const *> (&thisSlice) [0] == vtable) return true;
	    return Type::isOf (type);	
	}

	bool Slice::equals (const Generator & gen) const {
	    if (!gen.is<Slice> ()) return false;
	    auto array = gen.to <Slice> ();
	    return this-> getInners () [0].equals (array.getInners () [0]);
	}

	bool Slice::isCompatible (const Generator & gen) const {
	    if (this-> equals (gen)) return true;
	    if (!gen.is <Array> ()) return false;
	    auto array = gen.to <Array> ();
		
	    return this-> getInners () [0].to<Type> ().isCompatible (array.getInners () [0]);
	}	
	
	std::string Slice::typeName () const {
	    Ymir::OutBuffer buf;
	    buf.write ("[", this-> getInners () [0].to<Type> ().getTypeName (this-> isMutable ()), "]");
	    return buf.str ();
	}
		
    }
}