#include <ymir/semantic/symbol/Impl.hh>

namespace semantic {

    Impl::Impl () :
	ISymbol (lexing::Word::eof (), false),
	_trait (syntax::Expression::empty ())
    {}
    
    Impl::Impl (const lexing::Word & name, const syntax::Expression & trait, bool isWeak) :
	ISymbol (name, isWeak),
	_trait (trait)
    {}

    Symbol Impl::init (const lexing::Word & name, const syntax::Expression & trait, bool isWeak) {
	auto ret = Symbol {new (Z0) Impl (name, trait, isWeak)};
	ret.to <Impl> ()._table = Table::init (ret.getPtr ());
	return ret;
    }

    bool Impl::isOf (const ISymbol * type) const {
	auto vtable = reinterpret_cast <const void* const *> (type) [0];
	Impl thisType; // That's why we cannot implement it for all class
	if (reinterpret_cast <const void* const *> (&thisType) [0] == vtable) return true;
	return ISymbol::isOf (type);
    }

    void Impl::insert (const Symbol & sym) {
	this-> _table-> insert (sym);
    }
   

    void Impl::replace (const Symbol & sym) {
	this-> _table-> replace (sym);
    }

    std::vector <Symbol> Impl::get (const std::string & name) const {
	auto vec = getReferent ().get (name);
	auto local = this-> _table-> get (name);
	
	vec.insert (vec.begin (), local.begin (), local.end ());
	return vec;
    }

    std::vector <Symbol> Impl::getPublic (const std::string & name) const {
	auto vec = getReferent ().getPublic (name);
	auto local = this-> _table-> getPublic (name);
	
	vec.insert (vec.begin (), local.begin (), local.end ());
	return vec;
    }
    
    std::vector<Symbol> Impl::getLocal (const std::string & name) const {
	return this-> _table-> get (name);
    }

    std::vector<Symbol> Impl::getLocalPublic (const std::string & name) const {
	return this-> _table-> getPublic (name);
    }

    const std::vector <Symbol> & Impl::getAllInner () const {
	return this-> _table-> getAll ();
    }
    
    bool Impl::equals (const Symbol & other, bool parent) const {
	if (!other.is<Impl> ()) return false;
	if (this-> getName () == other.getName ()) {
	    if (parent)
		return this-> getReferent ().equals (other.getReferent ());
	    else return true;
	} else 
	    return false;
    }

    std::string Impl::getRealName () const {
	return this-> getReferent ().getRealName ();
    }
    
    const syntax::Expression & Impl::getTrait () const {
	return this-> _trait;
    }    

    std::string Impl::formatTree (int i) const {
	Ymir::OutBuffer buf;
	buf.writefln ("%*- %", i, "|\t", this-> getName ());
	for (auto & it : this-> _table-> getAll ()) {
	    buf.write (it.formatTree (i + 1));
	}
	return buf.str ();
    }
    
}
