#include <ymir/semantic/validator/MatchVisitor.hh>
#include <ymir/semantic/validator/BinaryVisitor.hh>
#include <ymir/semantic/validator/SubVisitor.hh>
#include <ymir/syntax/visitor/Keys.hh>
#include <ymir/global/State.hh>
#include <ymir/global/Core.hh>
using namespace global;

namespace semantic {

    namespace validator {

	using namespace generator;
	using namespace Ymir;
	using namespace syntax;

	MatchVisitor::MatchVisitor (Visitor & context) :
	    _context (context)
	{}

	MatchVisitor MatchVisitor::init (Visitor & context) {
	    return MatchVisitor {context};
	}
	
	generator::Generator MatchVisitor::validate (const syntax::Match & expression) {
	    auto value = this-> _context.validateValue (expression.getContent ());
	    // The value will be used in multiple test, we don't want to generate it multiple time though
	    if (!value.is<Referencer> ())
		value = UniqValue::init (value.getLocation (), value.to <Value> ().getType (), value);
	       	    
	    Generator result (Generator::empty ());
	    
	    auto & matchers = expression.getMatchers ();
	    auto & actions  = expression.getActions ();
	    Generator type (Generator::empty ());
	    bool isMandatory = false;
	    std::vector <std::string> errors;
	    for (auto it_ : Ymir::r (0, matchers.size ())) {
		auto it = matchers.size () - 1 - it_; // We get them in the reverse order to have the tests in the right order
		Generator test (Generator::empty ());
		bool local_mandatory = false;
		this-> _context.enterBlock ();
		{
		    TRY (
			test = this-> validateMatch (value, matchers [it], local_mandatory);
			if (local_mandatory) isMandatory = true;
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;
		}
		
		if (!test.isEmpty ()) { // size == 2, if succeed
		    auto content = this-> _context.validateValue (actions [it]);
		    auto local_type = content.to <Value> ().getType ();
		    
		    if (type.isEmpty () && (!content.to <Value> ().isReturner () && !content.to <Value> ().isBreaker ())) {
			// We don't take the type into account, if the content is throwing or returning or something like that
			type = local_type;
		    } else {
			if (!content.to <Value> ().isReturner () && !content.to<Value> ().isBreaker ()) // If it is a breaker or a returner the value won't be evaluated anyway
			    this-> _context.verifyCompatibleType (content.getLocation (), type, local_type);
		    }
			
		    if (type.isEmpty ())		       			
			result = Conditional::init (matchers [it].getLocation (), local_type, test, content, result, local_mandatory); 
		    else
			result = Conditional::init (matchers [it].getLocation (), type, test, content, result, local_mandatory); 
		}
		
		{
		    TRY (
			if (errors.size () != 0)
			    this-> _context.discardAllLocals ();
			
			this-> _context.quitBlock ();
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;
		}
	    }

	    if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);
	    
	    if (!isMandatory && (!type.is<Void> () || expression.isFinal ())) {
		if (!type.is<Void> ()) {
		    Ymir::Error::occur (expression.getLocation (), ExternalError::get (MATCH_NO_DEFAULT), type.prettyString ());
		} else {
		    Ymir::Error::occur (expression.getLocation (), ExternalError::get (MATCH_FINAL_NO_DEFAULT));
		}
	    }
	    
	    return result;
	}

       	

	Generator MatchVisitor::validateMatch (const Generator & value, const Expression & matcher, bool & isMandatory) {
	    match (matcher) {
		of (syntax::Var, var,		    
		    if (var.getName () == Keys::UNDER) {
			isMandatory = true;
			return BoolValue::init (value.getLocation (), Bool::init (value.getLocation ()), true);
		    }
		);
				
		of (syntax::Binary, bin,
		    return validateMatchBinary (value, bin, isMandatory);
		);
		
		of (syntax::VarDecl, decl,
		    return validateMatchVarDecl (value, decl, isMandatory);
		);

		of (syntax::List, lst,
		    return validateMatchList (value, lst, isMandatory);
		);
				
		of (syntax::MultOperator, call, {
			if (call.getEnd () == Token::RPAR) 
			    return validateMatchCall (value, call, isMandatory);
		    }
		);
	    }
	    return validateMatchAnything (value, matcher, isMandatory);
	}

	Generator MatchVisitor::validateMatchVarDecl (const Generator & value_, const syntax::VarDecl & var, bool & isMandatory) {
	    std::vector <std::string> errors;
	    Generator test (Generator::empty ());
	    Generator varDecl (Generator::empty ());
	    TRY (
		Generator value (value_);
		if (var.getName () != Keys::UNDER)
		    this-> _context.verifyShadow (var.getName ());


		Generator varType (Generator::empty ());
		if (!var.getType ().isEmpty ())		
		    varType = this-> _context.validateType (var.getType ());
		else {
		    varType = value.to <Value> ().getType (); // to have a vardecl, we must at least have a type or a value
		    varType.to <Type> ().isRef (false);
		    varType.to <Type> ().isMutable (false);
		}	    

		bool isMutable = false;
		bool isRef = false;
		this-> _context.applyDecoratorOnVarDeclType (var.getDecorators (), varType, isRef, isMutable);
		// The type checking is made in reverse
		// We want to be able to get an inherit class, from an ancestor class
		// That is exactly the reverse of function call, or var affectation
		this-> _context.verifyCompatibleType (var.getLocation (), value.to <Value> ().getType (), varType);
		Generator type_test (Generator::empty ());
		if (!value.to <Value> ().getType ().to <Type> ().isCompatible (varType)) { // If we have passed the test, but still not compatible means it is a class
		    auto loc = var.getLocation ();
		    auto bin = syntax::Binary::init ({loc, Token::DCOLON},
						     TemplateSyntaxWrapper::init (loc, value),
						     syntax::Var::init ({loc, SubVisitor::__TYPEINFO__}),
						     syntax::Expression::empty ()
		    );
		    
		    auto call = syntax::MultOperator::init ({loc, Token::LPAR}, {loc, Token::RPAR},
							    syntax::Var::init ({loc, global::CoreNames::get (global::TYPE_INFO_EQUAL)}),
							    {bin, TemplateSyntaxWrapper::init (loc, this-> _context.validateTypeInfo (var.getLocation (), varType))}							    
		    );
		    type_test = this-> _context.validateValue (call);
		}
		
		if (!var.getValue ().isEmpty ()) {
		    test = validateMatch (value, var.getValue (), isMandatory);			
		} else if (type_test.isEmpty ()) { // If is not a class, maybe we need a check and it is not mandatory
		    isMandatory = true;
		    // We already checked the types, and we want to check in reverse anyway
		    this-> _context.verifyMemoryOwner (var.getLocation (), varType, value, true, false, true);
		    test = BoolValue::init (value.getLocation (), Bool::init (value.getLocation ()), true);
		} else
		    test = BoolValue::init (value.getLocation (), Bool::init (value.getLocation ()), true);
		
		if (!type_test.isEmpty ()) {
		    test = BinaryBool::init (var.getLocation (),
					     Binary::Operator::AND,
					     Bool::init (var.getLocation ()),
					     test, type_test);
		}

		varDecl = generator::VarDecl::init (var.getLocation (), var.getName ().str, varType, value, isMutable);
		if (var.getName () != Keys::UNDER) {
		    this-> _context.insertLocal (var.getName ().str, varDecl);
		}
				
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors = msgs;
		errors.insert (errors.begin (), Ymir::Error::createNote (var.getLocation (), ExternalError::get (IN_MATCH_DEF)));
	    } FINALLY;

	    if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);
	    
	    return Set::init (var.getLocation (), Bool::init (value_.getLocation ()), {varDecl, test});
	}

	Generator MatchVisitor::validateMatchBinary (const Generator & value, const syntax::Binary & bin, bool & isMandatory) {
	    if (bin.getLocation () == Token::PIPE) {
		auto testLeft = this-> validateMatch (value, bin.getLeft (), isMandatory);
		auto testRight = this-> validateMatch (value, bin.getRight (), isMandatory);
		
		return BinaryBool::init (bin.getLocation (), Binary::Operator::OR, Bool::init (bin.getLocation ()), testLeft, testRight);					 
	    } else if (bin.getLocation () == Token::DDOT || bin.getLocation () == Token::TDOT) {
		auto loc = lexing::Word{bin.getLocation (), Keys::IN};		
		auto  templ = syntax::TemplateCall::init (
		    loc,
		    {syntax::String::init (loc, loc, loc, lexing::Word::eof ())},
		    syntax::Var::init ({loc, CoreNames::get (BINARY_OP_OVERRIDE)})
		);
		
		auto call = syntax::MultOperator::init (
		    {loc, Token::LPAR}, {loc, Token::RPAR},
		    templ,
		    {TemplateSyntaxWrapper::init (value.getLocation (), value), bin.clone ()}
		);
		
		return this-> _context.validateValue (call);
	    }
	    
	    return validateMatchAnything (value, bin.clone (), isMandatory);	    
	}

	Generator MatchVisitor::validateMatchList (const Generator & value, const syntax::List & lst, bool & isMandatory) {
	    if (value.to <Value> ().getType ().is <Tuple> () && lst.getLocation () == Token::LPAR) {
		if (value.to <Value> ().getType ().to <Type> ().getInners ().size () == lst.getParameters ().size ()) {
		    isMandatory = true;
		    Generator globTest (Generator::empty ());
		    for (auto it : Ymir::r (0, value.to <Value> ().getType ().to <Type> ().getInners ().size ())) {
			bool loc_mandatory = false;
			auto acc = TupleAccess::init (lst.getLocation (), value.to <Value> ().getType ().to <Type> ().getInners ()[it], value, it);
			auto loc_test = validateMatch (acc, lst.getParameters () [it], loc_mandatory);
			isMandatory = isMandatory && loc_mandatory;
			if (it == 0)
			    globTest = loc_test;
			else globTest = BinaryBool::init (lst.getLocation (),
							  Binary::Operator::AND,
							  Bool::init (lst.getLocation ()),
							  globTest, loc_test);
		    }
		    
		    if (globTest.isEmpty () && lst.getParameters ().size () == 0) // No params
			return BoolValue::init (value.getLocation (), Bool::init (value.getLocation ()), true);
		    return globTest;
		}
	    } else if (value.to <Value> ().getType ().is <Array> () && lst.getLocation () == Token::LCRO) {
		if (value.to <Value> ().getType ().to <Array> ().getSize () == lst.getParameters ().size ()) {
		    isMandatory = true;
		    Generator globTest (Generator::empty ());
		    auto innerType = value.to <Value> ().getType ().to <Array> ().getInners ()[0];
		    for (auto it : Ymir::r (0, value.to <Value> ().getType ().to <Type> ().getInners ().size ())) {
			bool loc_mandatory = false;
			auto acc = ArrayAccess::init (lst.getLocation (), innerType, value, ufixed (lst.getLocation (), it));
			auto loc_test = validateMatch (acc, lst.getParameters ()[it], loc_mandatory);
			isMandatory = isMandatory && loc_mandatory;
			if (it == 0)
			    globTest = loc_test;
			else globTest = BinaryBool::init (lst.getLocation (),
							  Binary::Operator::AND,
							  Bool::init (lst.getLocation ()),
							  globTest, loc_test);
			if (globTest.isEmpty () && lst.getParameters ().size () == 0) // No params
			    return BoolValue::init (value.getLocation (), Bool::init (value.getLocation ()), true);
			return globTest;
		    }
		}
	    }
	    
	    return validateMatchAnything (value, lst.clone (), isMandatory);
	}

	Generator MatchVisitor::validateMatchCall (const Generator & value, const syntax::MultOperator & call, bool & isMandatory) {
	    if (value.to <Value> ().getType ().is <StructRef> ()) {
		return validateMatchCallStruct (value, call, isMandatory);
	    } else if (value.to <Value> ().getType ().is <ClassRef> ()) {
		return validateMatchCallClass (value, call, isMandatory);
	    }
	    
	    auto note = Ymir::Error::createNote (call.getLocation (), ExternalError::get (IN_MATCH_DEF));		
	    Ymir::Error::occurAndNote (value.getLocation (), note, ExternalError::get (MATCH_CALL), value.to <Value> ().getType ().prettyString ());
	    return Generator::empty ();
	}

	Generator MatchVisitor::validateMatchCallClass (const Generator & value, const syntax::MultOperator & call, bool & isMandatory) {
	    std::vector <std::string> errors;
	    Generator globTest (Generator::empty ());
	    auto loc = call.getLocation ();
	    Generator type (Generator::empty ());
	    
	    TRY (
		if (!call.getLeft ().is <Var> () || call.getLeft ().to <Var> ().getName () != Keys::UNDER) {
		    type = this-> _context.validateType (call.getLeft ());
		    this-> _context.verifyCompatibleType (value.getLocation (), value.to <Value> ().getType (), type);
		    if (!value.to <Value> ().getType ().to <Type> ().isCompatible (type)) {
			// If we have passed the test, but still not compatible means it is a class
			auto bin = syntax::Binary::init ({loc, Token::DCOLON},
							 TemplateSyntaxWrapper::init (loc, value),
							 syntax::Var::init ({loc, SubVisitor::__TYPEINFO__}),
							 syntax::Expression::empty ()
			);
		    
			auto call = syntax::MultOperator::init ({loc, Token::LPAR}, {loc, Token::RPAR},
								syntax::Var::init ({loc, global::CoreNames::get (global::TYPE_INFO_EQUAL)}),
								{bin, TemplateSyntaxWrapper::init (loc, this-> _context.validateTypeInfo (loc, type))}							    
			);
			globTest = this-> _context.validateValue (call);
		    } else {			
			isMandatory = true;
			globTest = BoolValue::init (value.getLocation (), Bool::init (value.getLocation ()), true);
		    }
		}
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors.insert (errors.end (), msgs.begin (), msgs.end ());
		errors.insert (errors.begin (), Ymir::Error::createNote (call.getLocation (), ExternalError::get (IN_MATCH_DEF)));		
	    } FINALLY;

	    if (errors.size () != 0) {
		THROW (ErrorCode::EXTERNAL, errors);
	    }
	    
	    type.to <Type> ().isMutable (value.to <Value> ().getType ().to <Type> ().isMutable ());
	    type.to <Type> ().isRef (false);
	    
	    // Simple affectation is sufficiant, since it is a Class, and therefore already a ref
	    auto castedValue = UniqValue::init (call.getLocation (), type, Cast::init (call.getLocation (), type, value));
	    globTest = Set::init (call.getLocation (), Bool::init (call.getLocation ()), {castedValue, globTest});
	    for (auto & it : call.getRights ()) {		
		if (!it.is <NamedExpression> ())
		    Ymir::Error::occur (it.getLocation (), ExternalError::get (MATCH_PATTERN_CLASS));
		auto name = it.to <NamedExpression> ().getLocation ().str;
		auto loc = it.getLocation ();
		auto bin = syntax::Binary::init ({loc, Token::DOT},
						 TemplateSyntaxWrapper::init (loc, castedValue),
						 syntax::Var::init ({loc, name}),
						 syntax::Expression::empty ());
		auto innerVal = this-> _context.validateValue (bin);		
		if (value.is <Referencer> ()) {
		    auto type = innerVal.to <Value> ().getType ();
		    type.to <Type> ().isRef (true);
		    innerVal = Referencer::init (innerVal.getLocation (), type, innerVal);
		}
		
		auto loc_mandatory = false;		
		auto loc_test = validateMatch (innerVal, it.to <NamedExpression> ().getContent (), loc_mandatory);
		isMandatory = loc_mandatory && isMandatory;
		globTest = BinaryBool::init (value.getLocation (),
					     Binary::Operator::AND,
					     Bool::init (value.getLocation ()),
					     globTest, loc_test);
	    }
	    return globTest;
	}
	
	Generator MatchVisitor::validateMatchCallStruct (const Generator & value, const syntax::MultOperator & call, bool & isMandatory) {	    
	    std::vector <std::string> errors;
	    TRY (
		if (!call.getLeft ().is <Var> () || call.getLeft ().to <Var> ().getName () != Keys::UNDER) {
		    auto type = this-> _context.validateType (call.getLeft ());
		    this-> _context.verifyCompatibleTypeWithValue (value.getLocation (), type, value);
		}
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors.insert (errors.end (), msgs.begin (), msgs.end ());
		errors.insert (errors.begin (), Ymir::Error::createNote (call.getLocation (), ExternalError::get (IN_MATCH_DEF)));		
	    } FINALLY;
	    
	    if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);
	    
	    auto & str = value.to <Value> ().getType ().to <StructRef> ().getRef ().to <semantic::Struct> ().getGenerator ().to <generator::Struct> ();
	    	    
	    isMandatory = true;
	    std::vector <Expression> rights = call.getRights ();
	    std::vector <Expression> params;
	    for (auto it : str.getFields ()) {
		auto param = findParameterStruct (rights, it.to <generator::VarDecl> ());		    		
		params.push_back (param); // We add empties, to have the same amount of params as fields
		// The pattern can be successful even if there is some fields that are not tested
	    }
	    
	    if (rights.size () != 0) {
		std::vector <std::string> names;
		for (auto & it : rights)
		    names.push_back (it.prettyString ());
		
		Ymir::Error::occur (call.getLocation (), call.getEnd (), ExternalError::get (UNUSED_MATCH_CALL_OP), names);				    
	    }		

	    Generator globTest (Generator::empty ());
	    for (auto it : Ymir::r (0, params.size ())) {
		if (!params [it].isEmpty ()) {
		    auto loc_mandatory = false;
		    auto acc = StructAccess::init (value.getLocation (), str.getFieldType (str.getFields() [it].to <generator::VarDecl> ().getName ()), value, str.getFields() [it].to <generator::VarDecl> ().getName ());
		    if (value.is<Referencer> ()) {
			auto type = acc.to <Value> ().getType ();
			type.to <Type> ().isRef (true);
			acc = Referencer::init (acc.getLocation (), type, acc);
		    }
		    
		    auto loc_test = validateMatch (acc, params [it], loc_mandatory);		    
		    isMandatory = loc_mandatory && isMandatory;
		    
		    if (globTest.isEmpty ()) // Not necessarily when it == 0, as params [0] can be empty
			globTest = loc_test;
		    else globTest = BinaryBool::init (value.getLocation (),
						      Binary::Operator::AND,
						      Bool::init (value.getLocation ()),
						      globTest, loc_test);
		}
	    }

	    return globTest;		
	}

	syntax::Expression MatchVisitor::findParameterStruct (std::vector <Expression> & params, const generator::VarDecl & var) {
	    // Cf CallVisitor::findParameterStruct (), it is the same function but on expression instead of Generators
	    for (auto it : Ymir::r (0, params.size ())) {
		if (params [it].is <NamedExpression> ()) {
		    auto name = params [it].to <NamedExpression> ().getLocation ();
		    if (name.str == var.getLocation ().str) {
			auto toRet = params [it].to <NamedExpression> ().getContent ();
			params.erase (params.begin () + it);
			return toRet;
		    }		    
		}
	    }
	    
	    if (!var.getVarValue ().isEmpty ()) return Expression::empty ();
	    for (auto it : Ymir::r (0, params.size ())) {
		if (!params [it].is <NamedExpression> ()) {
		    auto toRet = params [it];
		    params.erase (params.begin () + it);
		    return toRet;
		}
	    }
	    return Expression::empty ();
	}
	
	
	Generator MatchVisitor::validateMatchAnything (const Generator & value, const syntax::Expression & matcher, bool & isMandatory) {	    
	    auto binVisitor = BinaryVisitor::init (this-> _context);
	    auto fakeBinary = syntax::Binary::init ({matcher.getLocation (), Token::DEQUAL}, TemplateSyntaxWrapper::init (value.getLocation (), value), matcher, Expression::empty ());
	    auto ret = binVisitor.validate (fakeBinary.to <syntax::Binary> ());
	    Generator retValue (Generator::empty ());
	    TRY (
		retValue = this-> _context.retreiveValue (ret);
	    ) CATCH (ErrorCode::EXTERNAL) {
	    	GET_ERRORS_AND_CLEAR (msgs);		
	    } FINALLY;
	    
	    if (!retValue.isEmpty ()) {
		isMandatory = retValue.is <BoolValue> () && retValue.to <BoolValue> ().getValue ();
		return value;
	    } else return ret;
	}	    

	Generator MatchVisitor::validateCatcher (const Generator & excp, const std::vector <Generator> & possibleTypes, const syntax::Catch & expression) {
	    Generator result (Generator::empty ());
	    auto & matchers = expression.getMatchs ();
	    auto & actions = expression.getActions ();

	    Generator type (Generator::empty ());
	    bool isMandatory = false;
	    std::vector <std::string> errors;
	    std::vector <std::pair <lexing::Word, Generator> > usedTypes;
	    
	    for (auto it : Ymir::r (0, matchers.size ())) {
		// We are going in the right order, but the insertion will be performed at the end of the else
		// this-> _context.addElseToConditional ()
		
		Generator test (Generator::empty ());
		std::vector <Generator> local_types;
		bool all_mandatory = false;
		
		this-> _context.enterBlock ();

		{
		    std::vector <std::pair <lexing::Word, Generator> > founds; // Outside the try, because there is a coma inside this declaration, C++ sucks sometimes
		    TRY (
			test = this-> validateMatchForCatcher (excp, matchers [it], possibleTypes, local_types, all_mandatory);
			if (all_mandatory) {isMandatory = true;} // Keys::UNDER, we caught all
			bool once = false;
			for (auto &lt : local_types) {
			    bool found = false;
			    for (auto &it : usedTypes) {
				if (it.second.to<Type> ().isCompatible (lt)) {
				    found = true;
				    founds.push_back ({it.first, it.second});
				    break;   
				}
			    }
			    
			    if (!found) {
				once = true;
				usedTypes.push_back ({matchers [it].getLocation (), lt});
			    }
			}

			if (!once) { // If this catch has inserted nothing
			    for (auto & jt : founds) {
				auto note = Ymir::Error::createNote (jt.first);
				errors.push_back (Error::makeOccurAndNote (matchers [it].getLocation (), note, ExternalError::get (MULTIPLE_CATCH), jt.second.prettyString ()));
			    }
			}
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;
		}

		if (!test.isEmpty ()) { // size == 2, if succeed
		    auto content = this-> _context.validateValue (actions [it]);
		    auto local_type = content.to <Value> ().getType ();
		    
		    if (type.isEmpty () && (!content.to <Value> ().isReturner () && !content.to <Value> ().isBreaker ())) {
			// We don't take the type into account, if the content is throwing or returning or something like that
			type = local_type;
		    } else {
			if (!content.to <Value> ().isReturner () && !content.to<Value> ().isBreaker ()) // If it is a breaker or a returner the value won't be evaluated anyway
			    this-> _context.verifyCompatibleType (content.getLocation (), type, local_type);
		    }

		    Generator cond (Generator::empty ());
		    if (type.isEmpty ())		       			
			cond = Conditional::init (matchers [it].getLocation (), local_type, test, content, Generator::empty (), all_mandatory); 
		    else
			cond = Conditional::init (matchers [it].getLocation (), type, test, content, Generator::empty (), all_mandatory);

		    if (result.isEmpty ()) {
			result = cond;
		    } else result = this-> _context.addElseToConditional (result, cond);		    
		}
		
		{
		    TRY (
			if (errors.size () != 0)
			    this-> _context.discardAllLocals ();
			
			this-> _context.quitBlock ();
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;
		}		
	    }

	    if (!isMandatory) { // If there is no all catcher
		for (auto & it : possibleTypes) {
		    bool found = false;
		    for (auto & jt : usedTypes) {
			if (it.to <Type> ().isCompatible (jt.second)) {
			    found = true;
			    break;
			}
		    }
		    
		    if (!found)
			errors.push_back (Error::makeOccur (expression.getLocation (), ExternalError::get (NOT_CATCH), it.prettyString ()));
		}
	    }

	    if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);
	    
	    return result;
	}

	Generator MatchVisitor::validateMatchForCatcher (const Generator & value, const Expression & matcher, const std::vector <Generator> & possibleTypes, std::vector<Generator> & catchingTypes, bool & all) {
	    bool isMandatory = false;
	    match (matcher) {
		of (syntax::Var, var,
		    if (var.getName () == Keys::UNDER) {
			all = true;
			return BoolValue::init (value.getLocation (), Bool::init (value.getLocation ()), true);
		    }
		);

		of (syntax::VarDecl, decl,
		    return validateMatchVarDeclForCatcher (value, decl, possibleTypes, catchingTypes, all);
		);

		of (syntax::MultOperator, call, {
			if (call.getEnd () == Token::RPAR)
			    return validateMatchCallForCatcher (value, call, possibleTypes, catchingTypes, all);
		    }
		);

		// All the following won't pass but we wan't to get a valid error
		of (syntax::Binary, bin,
		    return validateMatchBinary (value, bin, isMandatory); // Normal test
		);

		of (syntax::List, lst,
		    return validateMatchList (value, lst, isMandatory); // Normal test
		);
	    }
	    
	    // Idem for that
	    return validateMatchAnything (value, matcher, isMandatory);
	}

	Generator MatchVisitor::validateMatchVarDeclForCatcher (const Generator & value_, const syntax::VarDecl & var, const std::vector <Generator> & possibleTypes, std::vector<Generator> & catchingTypes, bool & all) {
	    std::vector <std::string> errors;
	    Generator test (Generator::empty ());
	    Generator varDecl (Generator::empty ());

	    TRY (
		Generator value (value_);
		if (var.getName () != Keys::UNDER) this-> _context.verifyShadow (var.getName ());

		Generator varType (Generator::empty ());
		if (!var.getType ().isEmpty ())		
		    varType = this-> _context.validateType (var.getType ());
		else {
		    all = true; // No type, so every type will pass there
		    varType = value.to <Value> ().getType (); // to have a vardecl, we must at least have a type or a value
		    varType.to <Type> ().isRef (false);
		    varType.to <Type> ().isMutable (false);
		}

		bool isMutable = false;
		bool isRef = false;
		this-> _context.applyDecoratorOnVarDeclType (var.getDecorators (), varType, isRef, isMutable);
		// value is typed exception, this will pass if varType is an heir 
		this-> _context.verifyCompatibleType (var.getLocation (), value.to <Value> ().getType (), varType);

		auto loc = var.getLocation ();
		auto bin = syntax::Binary::init ({loc, Token::DCOLON},
						 TemplateSyntaxWrapper::init (loc, value),
						 syntax::Var::init ({loc, SubVisitor::__TYPEINFO__}),
						 syntax::Expression::empty ()
		);
		    
		auto call = syntax::MultOperator::init ({loc, Token::LPAR}, {loc, Token::RPAR},
							syntax::Var::init ({loc, global::CoreNames::get (global::TYPE_INFO_EQUAL)}),
							{bin, TemplateSyntaxWrapper::init (loc, this-> _context.validateTypeInfo (var.getLocation (), varType))}							    
		);
		
		auto type_test = this-> _context.validateValue (call);
		
		bool isMandatory = false;
		
		if (!var.getValue ().isEmpty ()) {
		    test = validateMatchForCatcher (value, var.getValue (), possibleTypes, catchingTypes, isMandatory);
		} else isMandatory = true;

		if (!test.isEmpty () && !type_test.isEmpty ()) {
		    test = BinaryBool::init (var.getLocation (), Binary::Operator::AND, Bool::init (var.getLocation ()), test, type_test);
		} else test = type_test;
		

		varDecl = generator::VarDecl::init (var.getLocation (), var.getName ().str, varType, value, isMutable);
		if (var.getName () != Keys::UNDER) {
		    this-> _context.insertLocal (var.getName ().str, varDecl);
		}

		if (isMandatory) { // If the test will always pass
		    catchingTypes = {}; // Prune to avoid conflict
		    for (auto & it : possibleTypes) {
			if (varType.to <Type> ().isCompatible (it) || this-> _context.isAncestor (varType, it))
			    catchingTypes.push_back (it);
		    }
		} else { // We need to verify that we didn't let pass some type that will not pass this test
		    std::vector <Generator> realCatch;
		    for (auto & it : catchingTypes) {
			if (varType.to <Type> ().isCompatible (it) || this-> _context.isAncestor (varType, it))
			    realCatch.push_back (it);
		    }
		    catchingTypes = realCatch;
		}
		
		// If no possibleType is compatible with varType, then this is an useless catcher
		if (catchingTypes.size () == 0) {
		    bool found = false;
		    for (auto & it : possibleTypes) {
			if (varType.to <Type> ().isCompatible (it) || this-> _context.isAncestor (varType, it)) {
			    found = true;
			    break;
			}
		    }
		    
		    if (!found) Ymir::Error::occur (var.getLocation (), ExternalError::get (USELESS_CATCH), varType.prettyString ());;		    
		}
		
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors = msgs;
		errors.back () = Ymir::Error::addNote (var.getLocation (), errors.back (), Error::createNote (var.getLocation (), ExternalError::get (IN_MATCH_DEF)));
	    } FINALLY;

	    if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);

	    
	    return Set::init (var.getLocation (), Bool::init (value_.getLocation ()), {varDecl, test});	    
	}

	Generator MatchVisitor::validateMatchCallForCatcher (const Generator & value, const syntax::MultOperator & call, const std::vector <Generator> & possibleTypes, std::vector<Generator> & catchingTypes, bool & all) {	    
	    std::vector <std::string> errors;
	    Generator globTest (Generator::empty ());
	    auto loc = call.getLocation ();
	    Generator type (Generator::empty ());
	    bool isMandatory = true;
	    all = false;
	    
	    TRY (
		if (!call.getLeft ().is <Var> () || call.getLeft ().to <Var> ().getName () != Keys::UNDER) {
		    type = this-> _context.validateType (call.getLeft ());		    
		    this-> _context.verifyCompatibleType (value.getLocation (), value.to <Value> ().getType (), type);
		   
		    // If we have passed the test, but still not compatible means it is a class
		    auto bin = syntax::Binary::init ({loc, Token::DCOLON},
						     TemplateSyntaxWrapper::init (loc, value),
						     syntax::Var::init ({loc, SubVisitor::__TYPEINFO__}),
						     syntax::Expression::empty ()
		    );
		    
		    auto call = syntax::MultOperator::init ({loc, Token::LPAR}, {loc, Token::RPAR},
							    syntax::Var::init ({loc, global::CoreNames::get (global::TYPE_INFO_EQUAL)}),
							    {bin, TemplateSyntaxWrapper::init (loc, this-> _context.validateTypeInfo (loc, type))}							    
		    );
		    globTest = this-> _context.validateValue (call);		   
		}
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors.insert (errors.end (), msgs.begin (), msgs.end ());
		errors.back () = Ymir::Error::addNote (call.getLocation (), errors.back (), Ymir::Error::createNote (call.getLocation (), ExternalError::get (IN_MATCH_DEF)));
	    } FINALLY;
	    
	    if (errors.size () != 0) {
		THROW (ErrorCode::EXTERNAL, errors);
	    }
	    
	    type.to <Type> ().isMutable (false);
	    type.to <Type> ().isRef (false);

	    auto castedValue = UniqValue::init (call.getLocation (), type, Cast::init (call.getLocation (), type, value));
	    globTest = Set::init (call.getLocation (), Bool::init (call.getLocation ()), {castedValue, globTest});
	    
	    for (auto & it : call.getRights ()) {		
		if (!it.is <NamedExpression> ())
		    Ymir::Error::occur (it.getLocation (), ExternalError::get (MATCH_PATTERN_CLASS));
		auto name = it.to <NamedExpression> ().getLocation ().str;
		auto loc = it.getLocation ();
		auto bin = syntax::Binary::init ({loc, Token::DOT},
						 TemplateSyntaxWrapper::init (loc, castedValue),
						 syntax::Var::init ({loc, name}),
						 syntax::Expression::empty ());
		auto innerVal = this-> _context.validateValue (bin);		
		if (value.is <Referencer> ()) {
		    auto type = innerVal.to <Value> ().getType ();
		    type.to <Type> ().isRef (true);
		    innerVal = Referencer::init (innerVal.getLocation (), type, innerVal);
		}
		
		auto loc_mandatory = false;		
		auto loc_test = validateMatch (innerVal, it.to <NamedExpression> ().getContent (), loc_mandatory);
		isMandatory = loc_mandatory && isMandatory;
		globTest = BinaryBool::init (value.getLocation (),
					     Binary::Operator::AND,
					     Bool::init (value.getLocation ()),
					     globTest, loc_test);
	    }

	    if (isMandatory) { // If the test will always pass
		catchingTypes = {}; // Prune to avoid conflict
		for (auto & it : possibleTypes) {
		    if (type.to <Type> ().isCompatible (it) || this-> _context.isAncestor (type, it))
			catchingTypes.push_back (it);
		}
	    } else { // We need to verify that we didn't let pass some type that will not pass this test
		std::vector <Generator> realCatch;
		for (auto & it : catchingTypes) {
		    if (type.to <Type> ().isCompatible (it) || this-> _context.isAncestor (type, it))
			realCatch.push_back (it);
		}
		catchingTypes = realCatch;
	    }
		
	    // If no possibleType is compatible with type, then this is an useless catcher
	    if (catchingTypes.size () == 0) {
		bool found = false;
		for (auto & it : possibleTypes) {
		    if (type.to <Type> ().isCompatible (it) || this-> _context.isAncestor (type, it)) {
			found = true;
			break;
		    }
		}
		    
		if (!found) Ymir::Error::occur (call.getLocation (), ExternalError::get (USELESS_CATCH), type.prettyString ());;		    
	    }
	    
	    return globTest;
	}
	
    }    
}
