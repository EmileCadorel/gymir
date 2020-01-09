#include <ymir/syntax/declaration/Mixin.hh>

namespace syntax {

    Mixin::Mixin () : _mixin (Expression::empty ())
    {}

    Declaration Mixin::init () {
	return Declaration {new (Z0) Mixin ()};
    }

    Declaration Mixin::init (const Mixin & mixin) {
	auto ret = new (Z0) Mixin ();
	ret-> _location = mixin._location;
	ret-> _mixin = mixin._mixin;
	ret-> _declarations = mixin._declarations;
	return Declaration {ret};
    }

    Declaration Mixin::init (const lexing::Word & loc, const Expression & mixin, const std::vector<Declaration> & decls) {
	auto ret = new (Z0) Mixin ();
	ret-> _mixin = mixin;
	ret-> _location = loc;
	ret-> _declarations = decls;
	return Declaration {ret};
    }

    Declaration Mixin::clone () const {
	return Mixin::init (*this);
    }

    const lexing::Word & Mixin::getLocation () const {
	return this-> _location;
    }

    const Expression & Mixin::getMixin () const {
	return this-> _mixin;
    }
    
    bool Mixin::isOf (const IDeclaration * type) const {
	auto vtable = reinterpret_cast <const void* const *> (type) [0];
	Mixin thisType; // That's why we cannot implement it for all class
	if (reinterpret_cast <const void* const *> (&thisType) [0] == vtable) return true;
	return IDeclaration::isOf (type);
    }

    const std::vector <Declaration> & Mixin::getDeclarations () const {
	return this-> _declarations;
    }
    
    void Mixin::treePrint (Ymir::OutBuffer & stream, int i) const {
	stream.writefln ("%*<Mixin> ", i, '\t');
	this-> _mixin.treePrint (stream, i + 1);
    }
    

}
