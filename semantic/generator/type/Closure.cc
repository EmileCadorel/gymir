#include <ymir/semantic/generator/type/Closure.hh>
#include <ymir/utils/OutBuffer.hh>

namespace semantic {
    namespace generator {

	std::string Closure::ARITY_NAME = "arity";

	std::string Closure::INIT_NAME = "init";
	
	Closure::Closure () :
	    Type ()
	{
	    this-> isComplex (true);
	}

	Closure::Closure (const lexing::Word & loc, const std::vector <Generator> & inner, const std::vector<std::string> & names, uint index) :
	    Type (loc, loc.str),
	    _names (names),
	    _index (index)
	{
	    this-> isComplex (true);
	    this-> setInners (inner);
	}

	Generator Closure::init (const lexing::Word & loc, const std::vector<Generator> & inner, const std::vector <std::string> & names, uint index) {
	    return Generator {new (Z0) Closure (loc, inner, names, index)};
	}

	Generator Closure::clone () const {
	    return Generator {new (Z0) Closure (*this)};
	}
		
	bool Closure::isOf (const IGenerator * type) const {
	    auto vtable = reinterpret_cast <const void* const *> (type) [0];
	    Closure thisClosure; // That's why we cannot implement it for all class
	    if (reinterpret_cast <const void* const *> (&thisClosure) [0] == vtable) return true;
	    return Type::isOf (type);	
	}

	bool Closure::equals (const Generator & gen) const {
	    if (!gen.is<Closure> ()) return false;
	    auto tu = gen.to <Closure> ();
	    if (tu.getInners ().size () != this-> getInners ().size ()) return false;
	    for (auto it : Ymir::r (0, this-> getInners ().size ()))
		if (!this-> getInners () [it].equals (tu.getInners () [it])) return false;
		
	    return true;
	}

	const Generator & Closure::getField (const std::string & name) const {
	    for (auto it : Ymir::r (0, this-> _names.size ())) {
		if (this-> _names [it] == name)
		    return this-> getInners () [it];
	    }
	    return Generator::__empty__;
	}
	
	const std::vector <std::string> & Closure::getNames () const {
	    return this-> _names;
	}	

	uint Closure::getIndex () const {
	    return this-> _index;
	}
	
	std::string Closure::typeName () const {
	    Ymir::OutBuffer buf;
	    buf.write ("(");
	    for (auto it  : Ymir::r (0, this-> getInners ().size ())) {
		if (it != 0) buf.write (", ");
		buf.write (this-> getInners () [it].to <Type> ().getTypeName (this-> isMutable ()));
	    }
	    buf.write (")");
	    return buf.str ();
	}
	
    }
}