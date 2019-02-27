#include <ymir/semantic/generator/value/ArrayAccess.hh>

namespace semantic {

    namespace generator {

	ArrayAccess::ArrayAccess () :
	    Value (),
	    _array (Generator::empty ()),
	    _index (Generator::empty ())
	{
	    this-> isLvalue (true);
	}

	ArrayAccess::ArrayAccess (const lexing::Word & loc, const Generator & type, const Generator & array, const Generator & index) :
	    Value (loc, type),
	    _array (array),
	    _index (index)
	{
	    this-> isLvalue (true);
	}
	
	Generator ArrayAccess::init (const lexing::Word & loc, const Generator & type, const Generator & array, const Generator & index) {
	    return Generator {new ArrayAccess (loc, type, array, index)};
	}
    
	Generator ArrayAccess::clone () const {
	    return Generator {new (Z0) ArrayAccess (*this)};
	}

	bool ArrayAccess::isOf (const IGenerator * type) const {
	    auto vtable = reinterpret_cast <const void* const *> (type) [0];
	    ArrayAccess thisValue; // That's why we cannot implement it for all class
	    if (reinterpret_cast <const void* const *> (&thisValue) [0] == vtable) return true;
	    return Value::isOf (type);	
	}

	bool ArrayAccess::equals (const Generator & gen) const {
	    if (!gen.is <ArrayAccess> ()) return false;
	    auto bin = gen.to<ArrayAccess> ();	    
	    return bin._array.equals (this-> _array) && bin._index.equals (this-> _index);
	}

	const Generator & ArrayAccess::getArray () const {
	    return this-> _array;
	}

	const Generator & ArrayAccess::getIndex () const {
	    return this-> _index;
	}
	
    }
    
}
