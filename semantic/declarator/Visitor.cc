#include <ymir/semantic/declarator/Visitor.hh>
#include <ymir/semantic/symbol/_.hh>
#include <ymir/utils/Path.hh>
#include <ymir/syntax/visitor/Keys.hh>


using namespace Ymir;

namespace semantic {

    namespace declarator {
	
	Visitor::Visitor () {}
    
	Visitor Visitor::init () {
	    return Visitor ();
	}

	semantic::Symbol Visitor::visit (const syntax::Declaration & ast) {
	    match (ast) {
		of (syntax::Module, mod, 
		    return visitModule (mod);		
		);
	    
		of (syntax::Function, func,
		    return visitFunction (func);
		);

		of (syntax::Struct, str,
		    return visitStruct (str);
		);

		of (syntax::Alias, al,
		    return visitAlias (al);
		);

		of (syntax::DeclBlock, bl,
		    return visitBlock (bl);
		);

		of (syntax::Class, cls,
		    return visitClass (cls);
		);

		of (syntax::Global, glb,
		    return visitGlobal (glb);
		);
	    
		of (syntax::ExpressionWrapper, wrap, {
			match (wrap.getContent ()) {
			    of (syntax::VarDecl, decl,
				return visitVarDecl (decl);
			    );		    
			}
		
			Error::halt ("%(r) - reaching impossible point", "Critical");
			return Symbol::empty ();
		    }
		);
	    }

	    Error::halt ("%(r) - reaching impossible point", "Critical");
	    return Symbol::empty ();
	}    

	semantic::Symbol Visitor::visitModule (const syntax::Module & mod) {
	    if (mod.isGlobal () && !mod.getIdent ().isEof ()) {
		auto file_location = Path {mod.getIdent ().locFile}.stripExtension ();
		if (!Path {mod.getIdent ().str, "::"}.isRelativeFrom (file_location)) {
		    Ymir::Error::occur (mod.getIdent (), ExternalError::get (WRONG_MODULE_NAME), mod.getIdent ().str, Path {mod.getIdent ().str, "::"}.toString () + ".yr");
		}
	    }
	
	    pushReferent (Module::init (mod.getIdent ()));	
	    for (auto & it : mod.getDeclarations ()) {
		visit (it);
	    }

	    auto ret = popReferent ();
	    getReferent ().insert (ret);
	
	    return ret;
	}

	semantic::Symbol Visitor::visitFunction (const syntax::Function & func) {
	    auto function = Function::init (func.getName (), func);
	
	    auto symbol = getReferent ().getLocal (func.getName ().str);
	
	    if (symbol.is <Function> () && func.getName () != Keys::SELF_TILDE) {
		auto vec = symbol.to <Function> ().getOverloading ();
		vec.push_back (symbol);
		function.to<Function> ().setOverloading (vec);
	    } else if (!symbol.isEmpty ()) {	    
		auto note = Ymir::Error::createNote (symbol.getName ());
		Ymir::Error::occurAndNote (func.getName (), note, Ymir::ExternalError::get (Ymir::SHADOWING_DECL), func.getName ().str);
	    }
	
	    for (auto & ca : func.getCustomAttributes ()) {
		if (ca == Keys::INLINE) function.to <Function> ().isInline (true);
		else if (ca == Keys::SAFE) function.to <Function> ().isSafe (true);
		else if (ca == Keys::PURE) function.to <Function> ().isPure (true);
		else {
		    Ymir::Error::occur (ca, Ymir::ExternalError::get (Ymir::UNDEFINED_CA), ca.str);
		}
	    }	


	    pushReferent (function);
	    for (auto & it : func.getPrototype ().getParameters ()) {
		visit (syntax::ExpressionWrapper::init (it));
	    }
	
	    auto ret = popReferent ();	
	    getReferent ().insert (ret);
	    return ret;
	}        

	semantic::Symbol Visitor::visitStruct (const syntax::Struct & str) {
	    auto structure = Struct::init (str.getName ());
	
	    auto symbol = getReferent ().getLocal (str.getName ().str);	
	    if (symbol.is <Struct> ()) {
		auto vec = symbol.to <Struct> ().getOverloading ();
		vec.push_back (symbol);
		structure.to <Struct> ().setOverloading (vec);
	    } else if (!symbol.isEmpty ()) {
		auto note = Ymir::Error::createNote (symbol.getName ());
		Ymir::Error::occurAndNote (str.getName (), note, Ymir::ExternalError::get (Ymir::SHADOWING_DECL), str.getName ().str);
	    }
	
	    for (auto & ca : str.getCustomAttributes ()) {
		if (ca == Keys::PACKED) {
		    structure.to<Struct> ().isPacked (true);
		} else if (ca == Keys::UNION) {
		    structure.to<Struct> ().isUnion (true);
		} else Ymir::Error::occur (ca, Ymir::ExternalError::get (Ymir::UNDEFINED_CA), ca.str);

		if (structure.to <Struct> ().isUnion () && structure.to <Struct> ().isPacked ()) {
		    Ymir::Error::occur (ca, Ymir::ExternalError::get (Ymir::PACKED_AND_UNION));
		}
	    }	

	    pushReferent (structure);
	    for (auto & it : str.getDeclarations ()) {
		visit (syntax::ExpressionWrapper::init (it));
	    }
	
	    auto ret = popReferent ();	
	    getReferent ().insert (ret);
	    return ret;
	}

	semantic::Symbol Visitor::visitAlias (const syntax::Alias & stal) {
	    auto alias = Alias::init (stal.getName (), stal.getValue ());

	    auto symbol = getReferent ().getLocal (stal.getName ().str);
	    if (!symbol.isEmpty ()) {
		auto note = Ymir::Error::createNote (symbol.getName ());
		Ymir::Error::occurAndNote (stal.getName (), note, Ymir::ExternalError::get (Ymir::SHADOWING_DECL), stal.getName ().str);
	    }

	    getReferent ().insert (alias);
	    return alias;
	}    

	semantic::Symbol Visitor::visitBlock (const syntax::DeclBlock & block) {
	    // A declaration block is just a list of declaration, we do not enter a new referent
	    for (const syntax::Declaration & decl : block.getDeclarations ()) {
		visit (decl);
	    }

	    return Symbol::empty ();
	}

	semantic::Symbol Visitor::visitClass (const syntax::Class & stcls) {
	    auto cls = Class::init (stcls.getName (), stcls.getAncestor ());
	
	    auto symbol = getReferent ().getLocal (stcls.getName ().str);	
	    if (symbol.is <Class> ()) {
		auto vec = symbol.to <Class> ().getOverloading ();
		vec.push_back (symbol);
		cls.to <Class> ().setOverloading (vec);
	    } else if (!symbol.isEmpty ()) {
		auto note = Ymir::Error::createNote (symbol.getName ());
		Ymir::Error::occurAndNote (stcls.getName (), note, Ymir::ExternalError::get (Ymir::SHADOWING_DECL), stcls.getName ().str);
	    }

	    pushReferent (cls);
	    for (auto & it : stcls.getDeclarations ()) {
		visit (it);
	    }

	    auto ret = popReferent ();
	    getReferent ().insert (ret);
	    return ret;
	}

	semantic::Symbol Visitor::visitEnum (const syntax::Enum & stenm) {
	    auto enm = Enum::init (stenm.getName (), stenm.getType ());
	    auto symbol = getReferent ().getLocal (stenm.getName ().str);
	    if (!symbol.isEmpty ()) {
		auto note = Ymir::Error::createNote (symbol.getName ());
		Ymir::Error::occurAndNote (stenm.getName (), note, Ymir::ExternalError::get (Ymir::SHADOWING_DECL), stenm.getName ().str);
	    }

	    pushReferent (enm);
	    for (auto & it : stenm.getValues ()) {
		// Inside an enum the vars are declared using a vardecl expression
		visit (syntax::ExpressionWrapper::init (it));
	    }

	    auto ret = popReferent ();
	    getReferent ().insert (ret);
	    return ret;
	}    

	semantic::Symbol Visitor::visitVarDecl (const syntax::VarDecl & stdecl) {
	    auto decl = VarDecl::init (stdecl.getName (), stdecl.getDecos (), stdecl.getType (), stdecl.getValue ());
	    auto symbol = getReferent ().getLocal (stdecl.getName ().str);
	    if (!symbol.isEmpty ()) {
		auto note = Ymir::Error::createNote (symbol.getName ());
		Ymir::Error::occurAndNote (stdecl.getName (), note, Ymir::ExternalError::get (Ymir::SHADOWING_DECL), stdecl.getName ().str);
	    }

	    getReferent ().insert (decl);	
	    return decl;
	}
    
	semantic::Symbol Visitor::visitGlobal (const syntax::Global & stglob) {
	    return visit (syntax::ExpressionWrapper::init (stglob.getContent ()));	
	}

	void Visitor::pushReferent (const Symbol & sym) {
	    println ("Pushing ref");
	    this-> _referent.push_front (sym);
	}

	Symbol Visitor::popReferent () {
	    if (this-> _referent.size () == 0)
		Ymir::Error::halt ("%(r) - poping a symbol referent, while there is none", "Critical");

	    // We can't get a reference since the pop_front will erase the data
	    auto /** & */ ret = this-> _referent.front ();

	    this-> _referent.pop_front ();
	    return ret;
	}

	Symbol & Visitor::getReferent () {
	    if (this-> _referent.size () != 0)
		return this-> _referent.front ();
	    else return Symbol::__empty__;
	}

    }
    
}