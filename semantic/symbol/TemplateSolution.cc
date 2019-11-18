#include <ymir/semantic/symbol/TemplateSolution.hh>
#include <ymir/semantic/generator/value/TemplateSyntaxWrapper.hh>
#include <ymir/utils/OutBuffer.hh>
#include <ymir/semantic/generator/Mangler.hh>

namespace semantic {

    TemplateSolution::TemplateSolution () :
	ISymbol (lexing::Word::eof ()),
	_table (this)
    {}
    
    TemplateSolution::TemplateSolution (const TemplateSolution & mod) :
	ISymbol (mod),
	_table (mod._table.clone (this)),
	_params (mod._params)
    {}

    TemplateSolution::TemplateSolution (const lexing::Word & loc, const std::vector<syntax::Expression> & templs, const std::map<std::string, syntax::Expression> & params, const std::vector <std::string> & nameOrders) :
	ISymbol (loc),
	_table (this),
	_templs (templs), 
	_params (params),
	_nameOrder (nameOrders)
    {}
    
    Symbol TemplateSolution::init (const lexing::Word & name, const std::vector <syntax::Expression> & templs, const std::map <std::string, syntax::Expression> & mapping, const std::vector <std::string> & nameOrders) {
	return Symbol {new (Z0) TemplateSolution (name, templs, mapping, nameOrders)};
    }

    Symbol TemplateSolution::clone () const {
	return Symbol {new TemplateSolution (*this)};
    }

    bool TemplateSolution::isOf (const ISymbol * type) const {
	auto vtable = reinterpret_cast <const void* const *> (type) [0];
	TemplateSolution thisType; // That's why we cannot implement it for all class
	if (reinterpret_cast <const void* const *> (&thisType) [0] == vtable) return true;
	return ISymbol::isOf (type);
    }

    void TemplateSolution::insert (const Symbol & sym) {
	this-> _table.insert (sym);
    }

    void TemplateSolution::replace (const Symbol & sym) {
	this-> _table.replace (sym);
    }

    std::vector <Symbol> TemplateSolution::get (const std::string & name) const {
	auto vec = getReferent ().get (name);
	auto local = this-> _table.get (name);
	
	vec.insert (vec.begin (), local.begin (), local.end ());
	return vec;
    }

    std::vector <Symbol> TemplateSolution::getPublic (const std::string & name) const {
	auto vec = getReferent ().getPublic (name);
	auto local = this-> _table.getPublic (name);
	
	vec.insert (vec.begin (), local.begin (), local.end ());
	return vec;
    }
    
    std::vector<Symbol> TemplateSolution::getLocal (const std::string & name) const {
	return this-> _table.get (name);
    }

    const std::vector <Symbol> & TemplateSolution::getAllLocal () const {
	return this-> _table.getAll ();
    }
    
    bool TemplateSolution::equals (const Symbol & other) const {
	if (!other.is<TemplateSolution> ()) return false;
	if (this-> getName ().isSame (other.getName ())) {
	    auto & ot = other.to<TemplateSolution> ();	    
	    for (auto & it : _params) {
		auto _it = ot._params.find (it.first);
		if (_it == ot._params.end ()) return false;
		else if (_it-> second.is <generator::TemplateSyntaxWrapper> () && it.second.is <generator::TemplateSyntaxWrapper> ()) {
		    if (!_it-> second.to <generator::TemplateSyntaxWrapper> ().getContent ().equals (it.second.to <generator::TemplateSyntaxWrapper> ().getContent ()))
			return false;
		}
	    }
	    if (_nameOrder.size () != ot._nameOrder.size ()) return false;
	    for (auto it : Ymir::r (0, _nameOrder.size ())) {
		if (_nameOrder [it] != ot._nameOrder [it]) return false;
	    }
	    return this-> getReferent ().equals (other.getReferent ());
	} else 
	    return false;
    }    

    std::string TemplateSolution::formatTree (int i) const {
	Ymir::OutBuffer buf;
	buf.writefln ("%*- %", i, "|\t", this-> getName ());
	for (auto & it : this-> _table.getAll ()) {
	    buf.write (it.formatTree (i + 1));
	}
	return buf.str ();
    }
    
    std::string TemplateSolution::getRealName () const {
	Ymir::OutBuffer buf;
	buf.writef ("%", this-> getName ().str);
	int i = 0;
	buf.write ("(");
	for (auto & it : _nameOrder) {
	    if (i != 0)
		buf.write (",");
	    buf.write (this-> _params.find (it)-> second.to <generator::TemplateSyntaxWrapper> ().getContent ().prettyString ()); // [] on map discard const qualifier ?!!
	    i += 1;
	}
	buf.write (")");
	
	if (this-> getReferent ().isEmpty ()) return buf.str ();
	else {
	    auto ft = this-> getReferent ().getRealName ();
	    if (ft != "") return ft + "::" + buf.str ();
	    else return buf.str ();
	}
    }

    std::string TemplateSolution::getMangledName () const {
	Ymir::OutBuffer buf;
	buf.write (this-> getName ().str);
	generator::Mangler mangler = generator::Mangler::init ();
	for (auto & it : _nameOrder) {
	    buf.write (Ymir::format ("N%", mangler.mangle (this-> _params.find (it)-> second.to<generator::TemplateSyntaxWrapper> ().getContent ()))); // [] on map discard const qualifier ?!!
	}

	if (this-> getReferent ().isEmpty ()) return buf.str ();
	else {
	    auto ft = this-> getReferent ().getMangledName ();
	    if (ft != "") return ft + "::" + buf.str ();
	    else return buf.str ();
	}
    }

    const std::vector <syntax::Expression> & TemplateSolution::getTempls () const {
	return this-> _templs;
    }

    const std::map <std::string, syntax::Expression> & TemplateSolution::getParams () const {
	return this-> _params;
    }
    
}
