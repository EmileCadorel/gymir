#include <ymir/semantic/validator/PragmaVisitor.hh>
#include <ymir/semantic/validator/BinaryVisitor.hh>
#include <ymir/semantic/validator/UnaryVisitor.hh>
#include <ymir/semantic/validator/DotVisitor.hh>
#include <ymir/semantic/generator/Mangler.hh>
#include <ymir/errors/_.hh>


namespace semantic {

    namespace validator {

	using namespace generator;
	using namespace Ymir;

	const std::string PragmaVisitor::COMPILE = "compile";
	const std::string PragmaVisitor::MANGLE = "mangle";
	const std::string PragmaVisitor::OPERATOR = "operator";
	const std::string PragmaVisitor::FIELD_NAMES = "field_names";
	const std::string PragmaVisitor::FIELD_ADDRESS = "field_addrs";
	const std::string PragmaVisitor::TUPLEOF = "tupleof";
	const std::string PragmaVisitor::LOCAL_TUPLEOF = "local_tupleof";
	const std::string PragmaVisitor::HAS_DEFAULT = "has_default";
	
	PragmaVisitor::PragmaVisitor (Visitor & context) :
	    _context (context)
	{}

	PragmaVisitor PragmaVisitor::init (Visitor & context) {
	    return PragmaVisitor (context);
	}

	Generator PragmaVisitor::validate (const syntax::Pragma & prg) {	    
	    if (prg.getLocation ().getStr () == PragmaVisitor::COMPILE) {
		return this-> validateCompile (prg);	       		
	    } else if (prg.getLocation ().getStr () == PragmaVisitor::MANGLE) {
		return this-> validateMangle (prg);		
	    } else if (prg.getLocation ().getStr () == PragmaVisitor::OPERATOR) {
		return this-> validateOperator (prg);
	    } else if (prg.getLocation ().getStr () == PragmaVisitor::FIELD_NAMES) {
		return this-> validateFieldNames (prg);
	    } else if (prg.getLocation ().getStr () == PragmaVisitor::FIELD_ADDRESS) {
		return this-> validateFieldAddress (prg);
	    } else if (prg.getLocation ().getStr () == PragmaVisitor::TUPLEOF) {
		return this-> validateTupleOf (prg);
	    } else if (prg.getLocation ().getStr () == PragmaVisitor::LOCAL_TUPLEOF) {
		return this-> validateLocalTupleOf (prg);
	    } else if (prg.getLocation ().getStr () == PragmaVisitor::HAS_DEFAULT) {
		return this-> validateHasDefault (prg);
	    } else {
		Ymir::Error::occur (prg.getLocation (), ExternalError::get (UNKOWN_PRAGMA), prg.getLocation ().getStr ());
	    }
	    
	    return Generator::empty ();
	}


	/**
	 * ========================================================
	 *                       COMPILE
	 * ========================================================
	 */

	Generator PragmaVisitor::validateCompile (const syntax::Pragma & prg) {
	    try {
		for (auto & it : prg.getContent ()) {
		    this-> _context.validateValue (it);
		}
		    
		return BoolValue::init (prg.getLocation (), Bool::init (prg.getLocation ()), true);
	    } catch (Error::ErrorList list) {
		return BoolValue::init (prg.getLocation (), Bool::init (prg.getLocation ()), false);
	    }
	}

	/**
	 * ========================================================
	 *                       MANGLE
	 * ========================================================
	 */
	
	Generator PragmaVisitor::validateMangle (const syntax::Pragma & prg) {
	    if (prg.getContent ().size () != 1) {
		Ymir::Error::occur (prg.getLocation (), ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());
	    }
		
	    std::string res;
	    auto mangler = Mangler::init (false);
	    try {
		auto val = this-> _context.validateValue (prg.getContent ()[0], true, false);
		if (val.is <StringValue> ()) { // assumed to be a path
		    res = mangler.manglePath (Ymir::format ("%", val.to <StringValue> ().getValue ()));
		} else {
		    res = mangler.mangle (val);
		}


	    } catch (Error::ErrorList list) {
		try {
		    auto val = this-> _context.validateType (prg.getContent ()[0], true);
		    res = mangler.mangle (val);
		} catch (Error::ErrorList list) {
		    Ymir::Error::occurAndNote (prg.getLocation (), list.errors, ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());
		}
	    }

	    return this-> _context.validateValue (
		syntax::String::init (
		    prg.getLocation (),
		    prg.getLocation (),
		    lexing::Word::init (prg.getLocation (), res),
		    lexing::Word::eof ()
		    )
		);		    		   
	}

	/**
	 * ========================================================
	 *                       OPERATOR 
	 * ========================================================
	 */

	Generator PragmaVisitor::validateOperator (const syntax::Pragma & prg) {
	    if (prg.getContent ().size () != 2 && prg.getContent ().size () != 3) {		    
		Ymir::Error::occur (prg.getLocation (), ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());
	    }
		
	    auto op = prg.getContent ()[0];
	    if (!op.is <syntax::String> ())
		Ymir::Error::occur (prg.getLocation (), ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());

	    syntax::Expression syntOp (syntax::Expression::empty ());
		
	    if (prg.getContent ().size () == 2) {
		if (UnaryVisitor::toOperator (op.to<syntax::String> ().getSequence ()) == generator::Unary::Operator::LAST_OP) {
		    Ymir::Error::occur (prg.getLocation (), ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());
		}
		auto type = this-> _context.validateType (prg.getContent () [1]);
		auto value = FakeValue::init (prg.getLocation (), type);
		syntOp = syntax::Unary::init (op.to <syntax::String> ().getSequence (),
					      TemplateSyntaxWrapper::init (value.getLocation (), value));
	    } else if (prg.getContent ().size () == 3) {
		bool af = false;
		if (BinaryVisitor::toOperator (op.to<syntax::String> ().getSequence (), af) == generator::Binary::Operator::LAST_OP) {
		    Ymir::Error::occur (prg.getLocation (), ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());
		}
		auto left = this-> _context.validateType (prg.getContent () [1]);
		auto right = this-> _context.validateType (prg.getContent () [2]);
		auto lValue = FakeValue::init (prg.getLocation (), left);
		auto rValue = FakeValue::init (prg.getLocation (), right);
		syntOp = syntax::Binary::init (op.to <syntax::String> ().getSequence (),
					       TemplateSyntaxWrapper::init (lValue.getLocation (), lValue),
					       TemplateSyntaxWrapper::init (rValue.getLocation (), rValue),
					       syntax::Expression::empty ());
	    }
		
	    try {
		this-> _context.validateValue (syntOp);
		return BoolValue::init (prg.getLocation (), Bool::init (prg.getLocation ()), true);
	    } catch (Error::ErrorList list) {
		return BoolValue::init (prg.getLocation (), Bool::init (prg.getLocation ()), false);
	    }
	}

	/**
	 * ========================================================
	 *                       FIELD NAMES
	 * ========================================================
	 */
	
	Generator PragmaVisitor::validateFieldNames (const syntax::Pragma & prg) {
	    if (prg.getContent ().size () != 1) {		    
		Ymir::Error::occur (prg.getLocation (), ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());
	    }

	    auto type = this-> _context.validateType (prg.getContent ()[0]);
	    match (type) {
		of (generator::Enum, en, return validateEnumFieldNames (prg, en));
		of (generator::Struct, str, return validateStructFieldNames (prg, str));
		of (generator::Class, cl, return validateClassFieldNames (prg, cl));
		of (generator::EnumRef, eref, return validateEnumFieldNames (prg, eref.getRef ().to <semantic::Enum> ().getGenerator ().to <generator::Enum> ()));
		of (generator::StructRef, sref, return validateStructFieldNames (prg, sref.getRef ().to <semantic::Struct> ().getGenerator ().to <generator::Struct> ()));
		of (generator::ClassRef, cref, return validateClassFieldNames (prg, cref.getRef ().to <semantic::Class> ().getGenerator ().to <generator::Class> ()));
		of (generator::ClassPtr, cptr, return validateClassFieldNames (prg, cptr.getClassRef ().getRef ().to <semantic::Class> ().getGenerator ().to <generator::Class> ()));
		of (generator::Type, t, {
			if (t.getProxy ().is <generator::Enum> ()) return validateEnumFieldNames (prg, t.getProxy ().to <generator::Enum> ());
			if (t.getProxy ().is <generator::EnumRef> ()) return validateEnumFieldNames (prg, t.getProxy ().to <generator::EnumRef> ().getRef ().to <semantic::Enum> ().getGenerator ().to <generator::Enum> ());
		    }
		);
	    }
	    
	    Ymir::Error::occur (prg.getLocation (), ExternalError::get (TYPE_NO_FIELD), type.prettyString ());
	    return Generator::empty ();
	}

	Generator PragmaVisitor::validateEnumFieldNames (const syntax::Pragma & expression, const generator::Enum & en) {
	    auto inners = en.getFields ();
	    std::vector <Generator> values;
	    for (auto & it : inners) values.push_back (
		this-> _context.validateValue (syntax::String::init (it.getLocation (),
								     it.getLocation (),
								     it.getLocation (),
								     lexing::Word::eof ()))
		);
		    
	    auto innerType = values [0].to <Value> ().getType (); // there is always at least one element in tuple
		    
	    auto type = Array::init (expression.getLocation (), innerType, values.size ());	    
	    return ArrayValue::init (expression.getLocation (), type, values);
	}

	Generator PragmaVisitor::validateStructFieldNames (const syntax::Pragma & expression, const generator::Struct & str) {
	    auto & fields = str.getFields ();
	    std::vector <Generator> params;
	    Generator innerType (Void::init (expression.getLocation ()));
	    std::list <Ymir::Error::ErrorMsg> errors;
		    		    
	    for (auto & field : fields) {
		auto name = field.to <generator::VarDecl> ().getLocation ();
		params.push_back (this-> _context.validateValue (syntax::String::init (
								     name, 
								     name,
								     name,
								     lexing::Word::eof ())));
		innerType = params.back ().to <Value> ().getType ();
	    }			

	    auto arrType = Array::init (expression.getLocation (), innerType, params.size ());		    
	    return ArrayValue::init (expression.getLocation (), arrType, params);
	}

	Generator PragmaVisitor::validateClassFieldNames (const syntax::Pragma & expression, const generator::Class & cl) {
	    bool prv = false, prot = false;	    
	    this-> _context.getClassContext (cl.getRef (), prv, prot);

	    auto value = FakeValue::init (expression.getLocation (), ClassPtr::init (expression.getLocation (), cl.getClassRef ()));
	    std::vector <Generator> params;	    
	    Generator innerType (Void::init (expression.getLocation ()));
	    
	    std::list <Ymir::Error::ErrorMsg> errors;
	    auto dot_visit = DotVisitor::init (this-> _context);
	    for (auto & field : cl.getLocalFields ()) {			
		auto name = field.to <generator::VarDecl> ().getName ();
		auto field_access = dot_visit.validateClassFieldAccess (expression.getLocation (), value, name, prv, prot, errors);
		if (!field_access.isEmpty ()) {
		    auto name = field.to <generator::VarDecl> ().getLocation ();
		    params.push_back (this-> _context.validateValue (syntax::String::init (
									 name, 
									 name,
									 name,
									 lexing::Word::eof ())));
		    innerType = params.back ().to <Value> ().getType ();
		}
	    }
	    
	    auto arrType = Array::init (expression.getLocation (), innerType, params.size ());		    
	    return ArrayValue::init (expression.getLocation (), arrType, params);
	}

	
	/**
	 * ========================================================
	 *                       FIELD ADDRESS
	 * ========================================================
	 */

	Generator PragmaVisitor::validateFieldAddress (const syntax::Pragma & prg) {
	    if (prg.getContent ().size () != 1) {		    
		Ymir::Error::occur (prg.getLocation (), ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());
	    }

	    auto value = this-> _context.validateValue (prg.getContent ()[0]);
	    match (value.to <Value> ().getType ()) {
		of (StructRef, str ATTRIBUTE_UNUSED, return validateStructFieldAddress (prg, value));
		of (ClassPtr, cptr ATTRIBUTE_UNUSED, return validateClassFieldAddress (prg, value));
		of (ClassRef, cref ATTRIBUTE_UNUSED, return validateClassFieldAddress (prg, value));
	    }

	    Ymir::Error::occur (prg.getLocation (), ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());
	    return Generator::empty ();
	}

	Generator PragmaVisitor::validateStructFieldAddress (const syntax::Pragma & expression, const Generator & value) {
	    auto & t = value.to <Value> ().getType ().to <StructRef> ().getRef ().to <semantic::Struct> ().getGenerator ();
	    auto & fields = t.to <generator::Struct> ().getFields ();
	    std::vector <Generator> types;
	    std::vector <Generator> params;
	    std::list <Ymir::Error::ErrorMsg> errors;
	    for (auto & field : fields) {
		auto field_type = field.to <generator::VarDecl> ().getVarType ();
		if (value.to <Value> ().isLvalue () &&
		    value.to <Value> ().getType ().to <Type> ().isMutable () &&
		    field_type.to <Type> ().isMutable ())
				
		    field_type = Type::init (field_type.to <Type> (), true);
		else
		    field_type = Type::init (field_type.to <Type> (), false);
			    
		auto name = field.to <generator::VarDecl> ().getName ();
		auto type = Pointer::init (expression.getLocation (), field_type);
		type = Type::init (type.to <Type> (), field_type.to <Type> ().isMutable ());
			    
		auto field_value = StructAccess::init (expression.getLocation (), type, value, name); 
		params.push_back (Addresser::init (field_value.getLocation (), type, field_value));
		types.push_back (type);
	    }
	    
	    auto tuple = Tuple::init (expression.getLocation (), types);
	    tuple = Type::init (tuple.to <Type> (), true);
		    
	    return TupleValue::init (expression.getLocation (), tuple, params); 
	}

	Generator PragmaVisitor::validateClassFieldAddress (const syntax::Pragma & expression, const Generator & value) {
	    auto cl = value.to <Value> ().getType ().to <ClassPtr> ().getClassRef ().getRef ().to <semantic::Class> ().getGenerator ();
	    bool prv = false, prot = false;

	    if (value.to<Value> ().getType ().is <ClassProxy> ()) {
		this-> _context.getClassContext (value.to <Value> ().getType ().to <ClassProxy> ().getProxyRef ().getRef (), prv, prot);
		prv = false;
	    } else
		this-> _context.getClassContext (value.to <Value> ().getType ().to <ClassPtr> ().getClassRef ().getRef (), prv, prot);
		    
	    std::vector <Generator> types;
	    std::vector <Generator> params;
	    std::list <Ymir::Error::ErrorMsg> errors;
	    auto dot_visit = DotVisitor::init (this-> _context);
	    for (auto & field : cl.to <generator::Class> ().getLocalFields ()) {			
		auto name = field.to <generator::VarDecl> ().getName ();
		auto field_access = dot_visit.validateClassFieldAccess (expression.getLocation (), value, name, prv, prot, errors);
		if (!field_access.isEmpty ()) {
		    auto field_type = field_access.to <Value> ().getType ();		   		    
		    auto type = Pointer::init (field_access.getLocation (), field_type);
		    type = Type::init (type.to <Type> (), field_type.to <Type> ().isMutable ());
		    
		    params.push_back (Addresser::init (field_access.getLocation (), type, field_access));
		    types.push_back (type);
		}
	    }

	    auto tuple = Tuple::init (expression.getLocation (), types);
	    tuple = Type::init (tuple.to <Type> (), true);
		    
	    return TupleValue::init (expression.getLocation (), tuple, params);
	}

	/**
	 * ========================================================
	 *                       TUPLEOF
	 * ========================================================
	 */

	Generator PragmaVisitor::validateTupleOf (const syntax::Pragma & prg) {
	    if (prg.getContent ().size () != 1) {		    
		Ymir::Error::occur (prg.getLocation (), ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());
	    }

	    auto value = this-> _context.validateValue (prg.getContent ()[0]);
	    match (value.to <Value> ().getType ()) {
		of (StructRef, str ATTRIBUTE_UNUSED, return validateStructTupleOf (prg, value));
		of (ClassPtr, cptr ATTRIBUTE_UNUSED, return validateClassTupleOf (prg, value));
		of (ClassRef, cref ATTRIBUTE_UNUSED, return validateClassTupleOf (prg, value));
		of (Array, arr ATTRIBUTE_UNUSED, return validateArrayTupleOf (prg, value));
	    }

	    Ymir::Error::occur (prg.getLocation (), ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());
	    return Generator::empty ();
	}
	
	Generator PragmaVisitor::validateStructTupleOf (const syntax::Pragma & expression, const Generator & value) {
	    auto & t = value.to <Value> ().getType ().to <StructRef> ().getRef ().to <semantic::Struct> ().getGenerator ();
	    auto & fields = t.to <generator::Struct> ().getFields ();
	    std::vector <Generator> types;
	    std::vector <Generator> params;
	    std::list <Ymir::Error::ErrorMsg> errors;

	    for (auto & field : fields) {
		auto type = field.to <generator::VarDecl> ().getVarType ();
		auto name = field.to <generator::VarDecl> ().getName ();
		type = Type::init (type.to <Type> (), false);
			    
		params.push_back (StructAccess::init (expression.getLocation (), type, value, name));
		types.push_back (type);
	    }

	    auto tuple = Tuple::init (expression.getLocation (), types);
	    tuple = Type::init (tuple.to <Type> (), false); // Impossible to modify a struct via its tupleof
		    
	    return TupleValue::init (expression.getLocation (), tuple, params);        
	}

	Generator PragmaVisitor::validateClassTupleOf (const syntax::Pragma & expression, const Generator & value) {
	    auto cl = value.to <Value> ().getType ().to <ClassPtr> ().getClassRef ().getRef ().to <semantic::Class> ().getGenerator ();
	    bool prv = false, prot = false;

	    if (value.to<Value> ().getType ().is <ClassProxy> ()) {
		this-> _context.getClassContext (value.to <Value> ().getType ().to <ClassProxy> ().getProxyRef ().getRef (), prv, prot);
		prv = false;
	    } else
		this-> _context.getClassContext (value.to <Value> ().getType ().to <ClassPtr> ().getClassRef ().getRef (), prv, prot);
		    
	    std::vector <Generator> types;
	    std::vector <Generator> params;
	    std::list <Ymir::Error::ErrorMsg> errors;
	    auto dot_visit = DotVisitor::init (this-> _context);
	    for (auto & field : cl.to <generator::Class> ().getFields ()) {			
		auto name = field.to <generator::VarDecl> ().getName ();
		auto field_access = dot_visit.validateClassFieldAccess (expression.getLocation (), value, name, prv, prot, errors);
		if (!field_access.isEmpty ()) {
		    auto field_type = field_access.to <Value> ().getType ();
		    auto type = Type::init (field_type.to <Type> (), false);			    
		    params.push_back (field_access);
		    types.push_back (type);
		}
	    }

	    auto tuple = Tuple::init (expression.getLocation (), types);
	    tuple = Type::init (tuple.to <Type> (), false);
		    
	    return TupleValue::init (expression.getLocation (), tuple, params);
	}

	Generator PragmaVisitor::validateArrayTupleOf (const syntax::Pragma & expression, const Generator & value) {
	    std::vector <Generator> params;
	    std::vector <Generator> types;
	    auto inner = value.to <Value> ().getType ().to <Array> ().getInners ()[0];
	    inner = Type::init (inner.to <Type> (), false);
	    for (auto it : Ymir::r (0, value.to <Value> ().getType ().to <Array> ().getSize ())) {
		params.push_back (ArrayAccess::init (expression.getLocation (), inner, value, ufixed (it)));
		types.push_back (inner);
	    }

	    auto tuple = Tuple::init (expression.getLocation (), types);
	    tuple = Type::init (tuple.to <Type> (), false);
	    
	    return TupleValue::init (expression.getLocation (), tuple, params);
	}

	/**
	 * ========================================================
	 *                       LOCAL TUPLEOF
	 * ========================================================
	 */
	
	Generator PragmaVisitor::validateLocalTupleOf (const syntax::Pragma & prg) {
	    if (prg.getContent ().size () != 1) {		    
		Ymir::Error::occur (prg.getLocation (), ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());
	    }

	    auto value = this-> _context.validateValue (prg.getContent ()[0]);
	    match (value.to <Value> ().getType ()) {
		of (ClassPtr, cptr ATTRIBUTE_UNUSED, return validateClassLocalTupleOf (prg, value));
		of (ClassRef, cref ATTRIBUTE_UNUSED, return validateClassLocalTupleOf (prg, value));
	    }

	    Ymir::Error::occur (prg.getLocation (), ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());
	    return Generator::empty ();
	}

	Generator PragmaVisitor::validateClassLocalTupleOf (const syntax::Pragma & expression, const Generator & value) {
	    auto cl = value.to <Value> ().getType ().to <ClassPtr> ().getClassRef ().getRef ().to <semantic::Class> ().getGenerator ();
	    bool prv = false, prot = false;

	    if (value.to<Value> ().getType ().is <ClassProxy> ()) {
		this-> _context.getClassContext (value.to <Value> ().getType ().to <ClassProxy> ().getProxyRef ().getRef (), prv, prot);
		prv = false;
	    } else
		this-> _context.getClassContext (value.to <Value> ().getType ().to <ClassPtr> ().getClassRef ().getRef (), prv, prot);
	    if (prv) { // local tupleof exists only inside class definition
		std::vector <Generator> types;
		std::vector <Generator> params;
		std::list <Ymir::Error::ErrorMsg> errors;
		auto dot_visit = DotVisitor::init (this-> _context);
		for (auto & field : cl.to <generator::Class> ().getLocalFields ()) {			
		    auto name = field.to <generator::VarDecl> ().getName ();
		    auto field_access = dot_visit.validateClassFieldAccess (expression.getLocation (), value, name, prv, prot, errors);
		    if (!field_access.isEmpty ()) {
			auto field_type = field_access.to <Value> ().getType ();
			auto type = Type::init (field_type.to <Type> (), false);
			params.push_back (field_access);
			types.push_back (type);
		    }
		}

		auto tuple = Tuple::init (expression.getLocation (), types);
		tuple = Type::init (tuple.to <Type> (), false);
		    
		return TupleValue::init (expression.getLocation (), tuple, params);
	    }
	    
	    Ymir::Error::occur (expression.getLocation (), ExternalError::get (LOCAL_TUPLEOF_OUT_CLASS));
	    return Generator::empty ();
	}

	/**
	 * ========================================================
	 *                       HAS DEFAULT
	 * ========================================================
	 */

	Generator PragmaVisitor::validateHasDefault (const syntax::Pragma & prg) {
	    if (prg.getContent ().size () != 1) {		    
		Ymir::Error::occur (prg.getLocation (), ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());
	    }

	    auto type = this-> _context.validateType (prg.getContent ()[0]);
	    match (type) {
		of (generator::Struct, str, return validateStructHasDefault (prg, str));
		of (generator::StructRef, sref, return validateStructHasDefault (prg, sref.getRef ().to <semantic::Struct> ().getGenerator ().to <generator::Struct> ()));
	    }

	    Ymir::Error::occur (prg.getLocation (), ExternalError::get (MALFORMED_PRAGMA), prg.getLocation ().getStr ());
	    return Generator::empty ();
	}

	Generator PragmaVisitor::validateStructHasDefault (const syntax::Pragma & expression, const generator::Struct & str) {
	    auto & fields = str.getFields ();
	    std::vector <Generator> params;
	    Generator innerType (Bool::init (expression.getLocation ()));
	    std::list <Ymir::Error::ErrorMsg> errors;
		    		    
	    for (auto & field : fields) {
		bool has = !(field.to <generator::VarDecl> ().getVarValue ().isEmpty ());
		params.push_back (BoolValue::init (expression.getLocation (), Bool::init (expression.getLocation ()), has));
	    }			

	    auto arrType = Array::init (expression.getLocation (), innerType, params.size ());		    
	    return ArrayValue::init (expression.getLocation (), arrType, params);
	}
	
    }

    
}