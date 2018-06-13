#include <ymir/semantic/pack/FrameProto.hh>
#include <ymir/errors/Error.hh>
#include <ymir/utils/Range.hh>
#include <ymir/semantic/types/InfoType.hh>
#include <ymir/ast/ParamList.hh>
#include <ymir/semantic/pack/Symbol.hh>
#include <ymir/ast/Var.hh>
#include <ymir/semantic/tree/Tree.hh>
#include <ymir/utils/Mangler.hh>
#include <ymir/semantic/value/_.hh>
#include <ymir/semantic/types/CharInfo.hh>
#include <ymir/semantic/tree/Generic.hh>
#include <ymir/semantic/pack/FinalFrame.hh>
#include <ymir/semantic/types/RefInfo.hh>
#include <ymir/semantic/types/TupleInfo.hh>
#include <ymir/syntax/Keys.hh>

namespace semantic {
    using namespace syntax;    
    
    IFrameProto::IFrameProto (std::string name, Namespace space, Symbol type, const std::vector <syntax::Var>& vars, const std::vector <syntax::Expression>& tmps, std::vector <Word> attributes) :
	_space (space),
	_name (name),
	_type (type),
	_vars (vars),
	_tmps (tmps),
	_attributes (attributes),
	_externLang (""),
	_externLangSpace (""),
	_fn ()
    {}

    std::string& IFrameProto::name () {
	return this-> _name;
    }

    Namespace IFrameProto::space () {
	return this-> _space;
    }

    Symbol& IFrameProto::type () {
	return this-> _type;
    }

    std::vector <syntax::Var> & IFrameProto::vars () {
	return this-> _vars;
    }

    std::vector<syntax::Var>& IFrameProto::closure () {
	return this-> _closure;
    }
    
    std::vector <syntax::Expression>& IFrameProto::tmps () {
	return this-> _tmps;
    }

    std::vector <Word> & IFrameProto::attributes () {
	return this-> _attributes;
    }
    
    bool IFrameProto::isDelegate () {
	return this-> _closure.size () != 0;
    }
    
    std::string& IFrameProto::externLang () {
	return this-> _externLang;
    }

    std::string& IFrameProto::externLangSpace () {
	return this-> _externLangSpace;
    }

    bool& IFrameProto::isCVariadic () {
	return this-> _isCVariadic;
    }

    FinalFrame & IFrameProto::attached () {
	return this-> _attached;
    }

    bool& IFrameProto::isLvalue () {
	return this-> _isLvalue;
    }

    bool& IFrameProto::isMoved () {
	return this-> _isMoved;
    }
    
    bool IFrameProto::has (std::string attrs) {
	for (auto it : this-> _attributes)
	    if (it == attrs) return true;
	return false;
    }
    
    void IFrameProto::isForcedDelegate () {
	this-> _isForced = true;
	if (this-> _attached)
	    this-> _attached-> isForcedDelegate ();
    }
    
    bool IFrameProto::equals (IFrameProto* scd) {
	if (scd) {
	    if (this-> _space != scd-> _space) return false;
	    if (this-> _name != scd-> _name) return false;
	    if (this-> _tmps.size () != scd-> _tmps.size () ||
		this-> _vars.size () != scd-> _vars.size ()) return false;
	    
	    for (auto it : Ymir::r (0, this-> _tmps.size ())) {
		if (auto params = this-> _tmps [it]-> to <IParamList> ()) {
		    if (auto params2 = scd-> _tmps [it]-> to <IParamList> ()) {
			auto fstP = params-> getParamTypes ();
			auto scdP = params2-> getParamTypes ();
			if (fstP.size () != scdP.size ()) return false;
			for (auto it_ : Ymir::r (0, fstP.size ())) {
			    if (fstP [it_]-> simpleTypeString () != scdP [it_]-> simpleTypeString ())
				return false;
			}
		    } else return false;
		} else if (this-> _tmps [it]-> info == NULL || scd-> _tmps [it]-> info == NULL) {
		    if (this-> _tmps [it]-> prettyPrint () != scd-> _tmps [it]-> prettyPrint ()) return false;
		} else if (this-> _tmps [it]-> info-> isImmutable () != scd-> _tmps [it]-> info-> isImmutable ()) {
		    return false;
		} else if (this-> _tmps [it]-> info-> isImmutable ()) {
		    if (!this-> _tmps [it]-> info-> value ()-> equals (scd-> _tmps [it]-> info-> value ())) {
			return false;
		    }
		} else {
		    if (auto tu = this-> _tmps [it]-> info-> type ()-> to <ITupleInfo> ()) {
		    	if (auto tu2 = scd-> _tmps [it]-> info-> type ()-> to <ITupleInfo> ()) {
		    	    if (tu2-> isFake () != tu-> isFake ()) return false;
			    if (tu-> getParams ().size () != tu2-> getParams ().size ()) return false;
		    	    for (auto it : Ymir::r (0, tu-> getParams ().size ())) {
		    		if (!tu-> getParams () [it]-> value () && tu2-> getParams () [it]-> value ()) return false;
		    		if (tu-> getParams () [it]-> value () && !tu-> getParams () [it]-> value ()-> equals (tu2-> getParams () [it]-> value ()))
		    		    return false;
		    	    }
		    	} else return false;
		    } else if (this-> _tmps [it]-> info-> type ()-> simpleTypeString () != scd-> _tmps [it]-> info-> type ()-> simpleTypeString ()) {
		    	return false;
		    }
		}
	    }

	    for (auto it : Ymir::r (0, this-> _vars.size ())) {		
	    	if (this-> _vars [it]-> info-> type ()-> simpleTypeString () != scd-> _vars  [it]-> info-> type ()-> simpleTypeString ()) {
	    	    return false;
	    	}
	    }
	    return true;
	}
	return false;
    }

    Ymir::Tree IFrameProto::createClosureType () {
	auto name = Namespace (this-> space (), this-> _name).toString () + ".closure";
	if (this-> _closure.size () != 0) {
	    std::vector <InfoType> types;
	    std::vector <std::string> attrs;
	    for (auto it : this-> _closure) {
		if (this-> _isMoved)
		    types.push_back (it-> info-> type ());
		else
		    types.push_back (new (Z0) IRefInfo (false, it-> info-> type ()));
		attrs.push_back (it-> info-> sym.getStr ());
	    }
	    
	    return Ymir::makeTuple (name, types, attrs);
	} else if (this-> _isForced) {
	    return Ymir::makeTuple (name, {new (Z0) ICharInfo (false)});
	}
	return Ymir::Tree ();
    }
    
    Ymir::Tree IFrameProto::toGeneric () {
	std::vector <tree> fndecl_type_params (this-> _vars.size ());
	for (uint i = 0 ; i < this-> _vars.size () ; i++) {
	    fndecl_type_params [i] = this-> _vars [i]-> info-> type ()-> toGeneric ().getTree ();
	}

	Ymir::Tree closureType = createClosureType ();
	if (!closureType.isNull ()) {
	    fndecl_type_params.insert (
		fndecl_type_params.begin (),
		closureType.getTree ()
	    );	    
	}
	
	std::string ident = Namespace (this-> space (), this-> _name).toString ();
	std::string ident_ASM = Mangler::mangle_function (this-> _name, this);		;
	tree ret = this-> _type-> type ()-> toGeneric ().getTree ();
	tree fndecl_type;
	if (this-> isCVariadic ()) {
	    fndecl_type = build_varargs_function_type_array (ret, 0, fndecl_type_params.data ());
	} else {
	    fndecl_type = build_function_type_array (ret, 0, fndecl_type_params.data ());
	}

	tree fndecl = build_fn_decl (ident.c_str (), fndecl_type);
	SET_DECL_ASSEMBLER_NAME (fndecl, get_identifier (ident_ASM.c_str ()));
	
	if (this->_externLang != "") {
	    DECL_EXTERNAL (fndecl) = 1;
	}
	
	this-> _fn =  build1 (ADDR_EXPR, build_pointer_type (fndecl_type), fndecl);
	return this-> _fn;
    }        

}
