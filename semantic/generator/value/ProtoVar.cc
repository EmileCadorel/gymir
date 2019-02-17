#include <ymir/semantic/generator/value/ProtoVar.hh>

namespace semantic {
    namespace generator {

	ProtoVar::ProtoVar () :
	    Value (lexing::Word::eof (), Generator::empty ()),
	    _value (Generator::empty ())
	{
	    this-> isLocal (false);
	}

	ProtoVar::ProtoVar (const lexing::Word & location, const Generator & type, const Generator & value, bool isMutable) :
	    Value (location, type),
	    _value (value),
	    _isMutable (isMutable)
	{
	    this-> isLocal (false);
	    this-> isLvalue (isMutable);
	}

	Generator ProtoVar::init (const lexing::Word & location, const Generator & type, const Generator & value, bool isMutable) {
	    return Generator {new (Z0) ProtoVar (location, type, value, isMutable)};
	}

	Generator ProtoVar::clone () const {
	    return Generator {new (Z0) ProtoVar (*this)};
	}
			
	bool ProtoVar::isOf (const IGenerator * type) const {
	    auto vtable = reinterpret_cast <const void* const *> (type) [0];
	    ProtoVar thisProtoVar; // That's why we cannot implement it for all class
	    if (reinterpret_cast <const void* const *> (&thisProtoVar) [0] == vtable) return true;
	    return Value::isOf (type);	
	}

	bool ProtoVar::equals (const Generator & other) const {
	    if (!other.is <ProtoVar> ()) return false;
	    else 
		return other.getName () == this-> getName () &&
		    this-> getType ().equals (other.to<ProtoVar> ().getType ()) &&
		    this-> _value.equals (other.to<ProtoVar> ().getValue ());
	}


	bool ProtoVar::isMutable () const {
	    return this-> _isMutable;
	}


	const Generator & ProtoVar::getValue () const {
	    return this-> _value;
	}
	
    }
}