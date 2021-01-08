#include <ymir/semantic/symbol/MacroRule.hh>

namespace semantic {

    MacroRule::MacroRule () :
	ISymbol (lexing::Word::eof (), "", false),
	_constr (syntax::Declaration::empty ())
    {}
    
    MacroRule::MacroRule (const lexing::Word & name, const std::string & comments, const syntax::Declaration & constr) :
	ISymbol (name, comments, false),
	_constr (constr)
    {}

    Symbol MacroRule::init (const lexing::Word & name, const std::string & comments, const syntax::Declaration & constr) {
	return Symbol {new (NO_GC) MacroRule (name, comments, constr)};
    }

    bool MacroRule::equals (const Symbol & other, bool parent) const {
	if (!other.is <MacroRule> ()) return false;
	if (other.getName () == this-> getName ()) {
	    if (parent)
		return this-> getReferent ().equals (other.getReferent ());
	    else return true;
	} else return false;
    }

    const syntax::Declaration & MacroRule::getContent () const {
	return this-> _constr;
    }
    
    std::string MacroRule::formatTree (int i) const {
	Ymir::OutBuffer buf;
	buf.writefln ("%*- %", i, "|\t", this-> getName ());
	return buf.str ();
    }    

}