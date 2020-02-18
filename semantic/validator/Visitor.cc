#include <ymir/semantic/validator/Visitor.hh>
#include <ymir/semantic/validator/BinaryVisitor.hh>
#include <ymir/semantic/validator/BracketVisitor.hh>
#include <ymir/semantic/validator/CallVisitor.hh>
#include <ymir/semantic/validator/ForVisitor.hh>
#include <ymir/semantic/validator/CompileTime.hh>
#include <ymir/syntax/visitor/Keys.hh>
#include <ymir/semantic/validator/UnaryVisitor.hh>
#include <ymir/semantic/validator/CastVisitor.hh>
#include <ymir/semantic/validator/SubVisitor.hh>
#include <ymir/semantic/validator/DotVisitor.hh>
#include <ymir/semantic/validator/TemplateVisitor.hh>
#include <ymir/semantic/validator/UtfVisitor.hh>
#include <ymir/semantic/validator/MatchVisitor.hh>
#include <ymir/semantic/declarator/Visitor.hh>
#include <ymir/semantic/generator/Mangler.hh>
#include <ymir/utils/map.hh>
#include <ymir/global/Core.hh>
#include <ymir/utils/Path.hh>
#include <string>
#include <algorithm>

using namespace global;

namespace semantic {

    namespace validator {

	using namespace generator;
	using namespace Ymir;       
	
	Visitor::Visitor ()
	{
	    enterForeign ();
	}

	Visitor Visitor::init () {
	    return Visitor ();
	}

	void Visitor::validate (const semantic::Symbol & sym) {	    
	    match (sym) {
		of (semantic::Module, mod, {
			std::vector <std::string> errors;
			pushReferent (sym, "validate::module");			
			TRY (
			    validateModule (mod);
			 ) CATCH (ErrorCode::EXTERNAL) {
			    GET_ERRORS_AND_CLEAR (msgs);
			    errors.insert (errors.end (), msgs.begin (), msgs.end ());
			} FINALLY;
		       			
			popReferent ("validate::module");
			
			if (errors.size () != 0) {
			    THROW (ErrorCode::EXTERNAL, errors);
			}
			return;
		    }
		);

		of (semantic::Function, func, {
			std::vector <std::string> errors;
			pushReferent (sym, "validate::function");			
			TRY (
			    validateFunction (func);
			) CATCH (ErrorCode::EXTERNAL) {
			    GET_ERRORS_AND_CLEAR (msgs);
			    errors.insert (errors.end (), msgs.begin (), msgs.end ());
			} FINALLY;
			
			popReferent ("validate::function");
			
			if (errors.size () != 0) {
			    THROW (ErrorCode::EXTERNAL, errors);
			}
			return;
		    }
		);

		of (semantic::VarDecl, decl ATTRIBUTE_UNUSED, {
			std::vector <std::string> errors;
			pushReferent (sym, "validate::vdecl");			
			TRY (
			    validateVarDecl (sym);
			) CATCH (ErrorCode::EXTERNAL) {
			    GET_ERRORS_AND_CLEAR (msgs);
			    errors.insert (errors.end (), msgs.begin (), msgs.end ());
			} FINALLY;
			
			popReferent ("validate::vdecl");
			
			if (errors.size () != 0) {
			    THROW (ErrorCode::EXTERNAL, errors);
			}

			return;
		    }
		);

		of (semantic::Alias, alias ATTRIBUTE_UNUSED, {
			std::vector <std::string> errors;
			pushReferent (sym, "validate::alias");
			TRY (
			    validateAlias (sym);
			) CATCH (ErrorCode::EXTERNAL) {
			    GET_ERRORS_AND_CLEAR (msgs);
			    errors.insert (errors.end (), msgs.begin (), msgs.end ());			    
			} FINALLY;

			popReferent ("validate::alias");
			if (errors.size () != 0) {
			    THROW (ErrorCode::EXTERNAL, errors);
			}
			return;
		    }
		);
		
		of (semantic::Struct, str ATTRIBUTE_UNUSED, {
			std::vector <std::string> errors;
			pushReferent (sym, "validate::struct");			
			TRY (
			    validateStruct (sym);
			) CATCH (ErrorCode::EXTERNAL) {
			    GET_ERRORS_AND_CLEAR (msgs);
			    errors.insert (errors.end (), msgs.begin (), msgs.end ());
			} FINALLY;
			
			popReferent ("validate::struct");
			
			if (errors.size () != 0) {
			    THROW (ErrorCode::EXTERNAL, errors);
			}

			return;			
		    }
		);
		
		of (semantic::TemplateSolution, sol, {
			if (insertTemplateSolution (sym)) {
			    //println ("No template solution : ", sol.getSolutionName ());
			    std::vector <std::string> errors;			
			    pushReferent (sym, "validate::tmpl");
			    TRY (
				validateTemplateSolution (sol);
			    ) CATCH (ErrorCode::EXTERNAL) {
				GET_ERRORS_AND_CLEAR (msgs);
				errors.insert (errors.end (), msgs.begin (), msgs.end ());
			    } FINALLY;
			
			    popReferent ("validate::tmpl");
			    if (errors.size () != 0) {
				THROW (ErrorCode::EXTERNAL, errors);
			    }
			}
			return;			
		    }
		);

		of (semantic::Enum, en ATTRIBUTE_UNUSED, {
			std::vector <std::string> errors;
			pushReferent (sym, "validate::enum");
			TRY (
			    validateEnum (sym);
			) CATCH (ErrorCode::EXTERNAL) {
			    GET_ERRORS_AND_CLEAR (msgs);
			    errors.insert (errors.end (), msgs.begin (), msgs.end ());
			} FINALLY;
			
			popReferent ("validate::enum");
			if (errors.size () != 0) {
			    THROW (ErrorCode::EXTERNAL, errors);
			}

			return;
		    }
		);

		of (semantic::Class, cl ATTRIBUTE_UNUSED, {
			std::vector <std::string> errors;
			pushReferent (sym, "validate::class");			
			TRY (
			    validateClass (sym, true);
			) CATCH (ErrorCode::EXTERNAL) {
			    GET_ERRORS_AND_CLEAR (msgs);
			    errors.insert (errors.end (), msgs.begin (), msgs.end ());
			} FINALLY;
			
			popReferent ("validate::class");
			
			if (errors.size () != 0) {
			    THROW (ErrorCode::EXTERNAL, errors);
			}

			return;			
		    }		    
		);

		of (semantic::Trait, tr ATTRIBUTE_UNUSED, {			
			std::vector <std::string> errors;
			pushReferent (sym, "validate::trait");			
			TRY (
			    validateTrait (sym);
			) CATCH (ErrorCode::EXTERNAL) {
			    GET_ERRORS_AND_CLEAR (msgs);
			    errors.insert (errors.end (), msgs.begin (), msgs.end ());
			} FINALLY;
			
			popReferent ("validate::trait");
			
			if (errors.size () != 0) {
			    THROW (ErrorCode::EXTERNAL, errors);
			}

			return;			
		    }		    
		);
		
		/** Nothing to do for those kind of symbols */
		of (semantic::ModRef, x ATTRIBUTE_UNUSED, return);		
		of (semantic::Template, x ATTRIBUTE_UNUSED, return);
	    }
	    
	    Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");
	}      
	
	void Visitor::validateModule (const semantic::Module & mod) {
	    if (!mod.isExtern ()) {
		const std::vector <Symbol> & syms = mod.getAllLocal ();
		for (auto & it : syms) {
		    validate (it);
		}
	    }
	}

	Generator Visitor::validateTemplateTest (const Symbol & context, const syntax::Expression & expr) {
	    Generator value (Generator::empty ());
	    std::vector <std::string> errors;
	    pushReferent (context, "validateTemplateTest");
	    enterForeign ();
	    TRY (
		value = this-> validateValue (expr);
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors.insert (errors.end (), msgs.begin (), msgs.end ());
	    } FINALLY;

	    exitForeign ();
	    popReferent ("validateTemplateTest");
	    
	    if (errors.size () != 0) {
		THROW (ErrorCode::EXTERNAL, errors);
	    }
	    return value;
	}
	
	void Visitor::validateTemplateSolution (const semantic::TemplateSolution & sol) {
	    static int nb_recur_template = 0;
	    nb_recur_template += 1;	    
	    if (nb_recur_template >= VisitConstante::LIMIT_TEMPLATE_RECUR) {
		Ymir::Error::occur (sol.getName (), ExternalError::get (TEMPLATE_RECURSION), nb_recur_template);
	    }
	    const std::vector <Symbol> & syms = sol.getAllLocal ();
	    for (auto & it : syms) {
		validate (it);
	    }
	    nb_recur_template -= 1;
	}

	Generator Visitor::validateTemplateSolutionMethod (const semantic::Symbol & sol, const Generator & self) {
	    const std::vector <Symbol> & syms = sol.to <TemplateSolution> ().getAllLocal ();
	    if (syms.size () != 1) Ymir::Error::halt ("", "");
	    if (insertTemplateSolution (sol)) { // If it is the first time, the solution is presented
		std::vector <std::string> errors;
		pushReferent (sol, "validateTemplateSolutionMethod");
		enterForeign ();
		TRY (
		    static int nb_recur_template = 0;
		    nb_recur_template += 1;	    
		    if (nb_recur_template >= VisitConstante::LIMIT_TEMPLATE_RECUR) {
			Ymir::Error::occur (sol.getName (), ExternalError::get (TEMPLATE_RECURSION), nb_recur_template);
		    }

		    this-> validateMethod (syms [0].to <semantic::Function> (), self.to <Value> ().getType ());
		    nb_recur_template -= 1;
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    
		exitForeign ();
		popReferent ("validateTemplateSolutionMethod");
		if (errors.size () != 0) {
		    THROW (ErrorCode::EXTERNAL, errors);
		}
	    }
	    return this-> validateMethodProto (syms [0].to <semantic::Function> (), self.to <Value> ().getType ());
	}
	
	
	void Visitor::validateFunction (const semantic::Function & func) {
	    auto & function = func.getContent ();
	    std::vector <Generator> params;
	    std::vector <std::string> errors;
	    Generator retType (Generator::empty ());
	    
	    enterContext (function.getCustomAttributes ());
	    
	    enterBlock ();
	    {
		TRY (
		    validatePrototypeForFrame (func.getName (), function.getPrototype (), params, retType);	       	
		    if (function.getName () == Keys::MAIN) {		
			if (params.size () > 1) {
			    Ymir::Error::occur (params [1].getLocation (),
						ExternalError::get (MAIN_FUNCTION_ONE_ARG));		    
			} else if (params.size () == 1) {
			    auto argtype =  Slice::init (func.getName (),
							 Slice::init (
							     func.getName (),
							     Char::init (func.getName (), 8) // The input given by the task launcher is in utf8
							 )
			    );
			    
			    verifyCompatibleTypeWithValue (params [0].getLocation (), argtype, params [0]);
			}
		    }
	    
		    if (!retType.isEmpty ()) {
			if (function.getName () == Keys::MAIN) {
			    auto itype       = Integer::init (func.getName (), 32, true);
			    verifyCompatibleType (retType.getLocation (), itype, retType);
			}
		    } else retType = Void::init (func.getName ());
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }
	    
	    this-> setCurrentFuncType (retType);
	    
	    if (!function.getBody ().getBody ().isEmpty () && errors.size () == 0) {
		Generator body (Generator::empty ());
		volatile bool needFinalReturn = false;				
		{
		    TRY (
			body = validateValue (function.getBody ().getBody ());
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;
		}
		if (!body.isEmpty ()) {
		    if (!body.to<Value> ().isReturner ()) {
			verifyMemoryOwner (body.getLocation (), retType, body, true);		    
			needFinalReturn = !retType.is<Void> ();
		    }
		
		    {
			TRY (
			    quitBlock ();
			) CATCH (ErrorCode::EXTERNAL) {
			    GET_ERRORS_AND_CLEAR (msgs);
			    errors.insert (errors.end (), msgs.begin (), msgs.end ());
			} FINALLY;
		    }
		}
		
		exitContext ();
		if (errors.size () != 0)
		    THROW (ErrorCode::EXTERNAL, errors);
		
		auto frame = Frame::init (function.getName (), func.getRealName (), params, retType, body, needFinalReturn);
		auto ln = func.getExternalLanguage ();
		if (ln == Keys::CLANG) 
		    frame.to <Frame> ().setManglingStyle (Frame::ManglingStyle::C);
		else if (ln == Keys::CPPLANG)
		    frame.to <Frame> ().setManglingStyle (Frame::ManglingStyle::CXX);

		frame.to <Frame> ().isWeak (func.isWeak ());
		frame.to <Frame> ().setMangledName (func.getMangledName ());

		insertNewGenerator (frame);		
	    } else {
		// If the function has no body, it is normal that none of the parameters are used
		this-> discardAllLocals ();
		TRY (
		    quitBlock ();
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
		exitContext ();
		if (errors.size () != 0)
		    THROW (ErrorCode::EXTERNAL, errors);
	    }
	}

	void Visitor::validateVarDecl (const semantic::Symbol & sym) {
	    if (sym.to <semantic::VarDecl> ().getGenerator ().isEmpty ()) {
		auto var = sym.to <semantic::VarDecl> ();
		auto elemSym = sym;
		
		Generator type (Generator::empty ());
		Generator value (Generator::empty ());
	    
		if (!var.getType ().isEmpty ()) {
		    type = validateType (var.getType ());
		}

		if (!var.getValue ().isEmpty ()) {
		    value = validateValue (var.getValue ());		
		}

		if (var.getValue ().isEmpty ()) {
		    Error::occur (var.getName (), ExternalError::get (VAR_DECL_WITHOUT_VALUE));
		} 
		
		if (type.isEmpty ()) {
		    type = value.to <Value> ().getType ();
		    type.to<Type> ().isRef (false);
		    type.to <Type> ().isMutable (false); 
		}

		bool isMutable = false;		
		for (auto & deco : var.getDecorators ()) {
		    switch (deco.getValue ()) {
		    case syntax::Decorator::MUT : { type.to <Type> ().isMutable (true); isMutable = true; } break;
		    case syntax::Decorator::DMUT : { type = type.to <Type> ().toDeeplyMutable (); isMutable = true; } break;
		    default :
			Ymir::Error::occur (deco.getLocation (),
					    ExternalError::get (DECO_OUT_OF_CONTEXT),
					    deco.getLocation ().str
			);
		    }
		}
		if (!isMutable) type.to <Type> ().isMutable (false);

		if (!value.isEmpty ()) {
		    if (isMutable || !type.is <LambdaType> ())
			verifyMemoryOwner (var.getName (), type, value, true);
		    // verifyCompatibleType (value.getLocation (), type, value.to<Value> ().getType ());
		}
		
		auto glbVar = GlobalVar::init (var.getName (), var.getRealName (), isMutable, type, value);		
		elemSym.to<semantic::VarDecl> ().setGenerator (glbVar);
	    
		insertNewGenerator (glbVar);
	    }
	}
	
	generator::Generator Visitor::validateAlias (const semantic::Symbol & sym) {
	    if (sym.to <semantic::Alias> ().getGenerator ().isEmpty ()) {
		auto alias = sym.to <semantic::Alias> ();
		auto elemSym = sym;

		Generator elem (Generator::empty ());
		TRY (
		    elem = validateValue (alias.getValue (), true);
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    elem = Generator::empty ();
		} FINALLY;
		
		if (elem.isEmpty ())
		    elem = validateType (alias.getValue (), true);
		
		if (elem.is <Value> ()) {
		    auto type = elem.to <Value> ().getType ();
		    type.to <Type> ().isMutable (false);
		    type.to <Type> ().isRef (false);
		    elem.to <Value> ().setType (type);
		}
		
		elemSym.to <semantic::Alias> ().setGenerator (elem);
	    }

	    return sym.to <semantic::Alias> ().getGenerator ();
	}

	generator::Generator Visitor::validateStruct (const semantic::Symbol & str) {	    
	    if (str.to <semantic::Struct> ().getGenerator ().isEmpty ()) {
		auto sym = str;
		auto gen = generator::Struct::init (sym.getName (), sym);
		sym.to <semantic::Struct> ().setGenerator (gen);
		std::vector <std::string> errors;
		std::map <std::string, generator::Generator> syms;
		enterForeign ();
		pushReferent (sym, "validateStruct");
		TRY (
		    this-> enterBlock ();
		    std::vector <std::string> fields;
		    std::vector <generator::Generator> types;
		    for (auto & it : sym.to<semantic::Struct> ().getFields ()) {
			this-> validateVarDeclValue (it.to <syntax::VarDecl> (), false);
			if (str.to <semantic::Struct> ().isUnion () && !it.to <syntax::VarDecl> ().getValue ().isEmpty ())
			    errors.push_back (Ymir::Error::makeOccur (it.to <syntax::VarDecl> ().getValue ().getLocation (), ExternalError::get (UNION_INIT_FIELD)));
		    }
		    		    
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;		

		{
		    TRY (
			syms = this-> discardAllLocals ();
			this-> quitBlock ();
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;
		}
		
		exitForeign ();		
		popReferent ("validateStruct");
		
		if (errors.size () != 0) {
		    sym.to <semantic::Struct> ().setGenerator (NoneType::init (sym.getName ()));
		    THROW (ErrorCode::EXTERNAL, errors);
		}
		
		std::vector <Generator> fieldsDecl;
		for (auto & it : sym.to <semantic::Struct> ().getFields ()) {
		    auto gen = syms.find (it.to <syntax::VarDecl> ().getName ().str);		    
		    fieldsDecl.push_back (gen-> second);
		}
		
		gen.to <generator::Struct> ().setFields (fieldsDecl);
		for (auto & it : gen.to <generator::Struct> ().getFields ()) {
		    verifyRecursivity (it.getLocation (), it.to <generator::VarDecl> ().getVarType (), sym);
		}
		
 		sym.to <semantic::Struct> ().setGenerator (gen);
		return StructRef::init (str.getName (), sym);
	    }

	    if (str.to <semantic::Struct> ().getGenerator ().is <generator::Struct> ())
		return StructRef::init (str.getName (), str);
	    else {
		Ymir::Error::occur (str.getName (), ExternalError::get (INCOMPLETE_TYPE_CLASS), str.getRealName ());
		return Generator::empty ();
	    }
	}

	void Visitor::validateTrait (const semantic::Symbol & tr) {
	    for (auto & it : tr.to <semantic::Trait> ().getAllInner ()) {
		if (it.to <semantic::Function> ().isOver ())
		    Ymir::Error::occur (it.getName (), ExternalError::get (NOT_OVERRIDE), it.getName ().str);
	    }
	}

	void Visitor::validateInnerClass (const semantic::Symbol & cls) {
	    auto & clRef = cls.to <semantic::Class> ().getGenerator ().to <generator::Class> ().getClassRef ();
	    auto & ancestor = clRef.to <ClassRef> ().getAncestor ();
	    auto & addMethods = cls.to <semantic::Class> ().getAddMethods ();
	    
	    std::vector <Generator> ancestorFields;
	    if (!ancestor.isEmpty ())
		ancestorFields = ancestor.to <ClassRef> ().getRef ().to <semantic::Class> ().getGenerator ().to <generator::Class> ().getFields ();
	    
	    auto allInners = cls.to <semantic::Class> ().getAllInner ();
	    allInners.insert (allInners.end (), addMethods.begin (), addMethods.end ());
	    std::vector <std::string> errors;
	    
	    for (auto & it : allInners) {
		pushReferent (it, "validate::innerClass");
		TRY (
		    match (it) {
			of (semantic::Function, func ATTRIBUTE_UNUSED, {
				if (!func.getContent ().getBody ().getBody ().isEmpty ()) {
				    validateMethod (func, clRef, cls.isWeak ()); // We need to pass weak here
				    // The method could have been imported from a trait that is not weak
				}
			    }
			) else of (semantic::Constructor, cs ATTRIBUTE_UNUSED, {
				validateConstructor (it, clRef, ancestor, ancestorFields);
			    }
			);
		    }
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;

		popReferent ("validate::innerClass");			
	    }
	    
	    if (errors.size () != 0) {
		THROW (ErrorCode::EXTERNAL, errors);
	    }
	    
	}
	
	generator::Generator Visitor::validateClass (const semantic::Symbol & cls, bool inModule) {	    
	    Generator ancestor (Generator::empty ());
	    if (!cls.to <semantic::Class> ().getAncestor ().isEmpty ()) {
		ancestor = this-> validateType (cls.to <semantic::Class> ().getAncestor ());
		if (!ancestor.is <ClassRef> ()) {
		    Ymir::Error::occur (cls.to <semantic::Class> ().getAncestor ().getLocation (),
					ExternalError::get (INHERIT_NO_CLASS),
					ancestor.prettyString ()
		    );
		} else if (ancestor.to <ClassRef> ().getRef ().to <semantic::Class> ().isFinal ()) {
		    Ymir::Error::occur (cls.to <semantic::Class> ().getAncestor ().getLocation (),
					ExternalError::get (INHERIT_FINAL_CLASS),
					ancestor.prettyString ()
		    );
		}
	    }
	    
	    if (cls.to <semantic::Class> ().getGenerator ().isEmpty () || inModule) {

		auto sym = cls;
		auto gen = generator::Class::init (cls.getName (), sym, ClassRef::init (cls.getName (), ancestor, sym));
		// To avoid recursive validation 
		sym.to <semantic::Class> ().setGenerator (gen);
		
		std::vector <std::string> errors;
		std::map <std::string, generator::Generator> syms;
		std::vector <Generator> fieldsDecl;
		std::vector <Generator> ancestorFields;
		std::vector <Symbol> addMethods;
		
		{
		    enterForeign ();
		    pushReferent (sym, "validateClass");
		    TRY (
			this-> enterBlock ();
			std::vector <std::string> fields;
			std::vector <generator::Generator> types;
			for (auto & it : sym.to<semantic::Class> ().getFields ()) {
			    this-> validateVarDeclValue (it.to <syntax::VarDecl> (), false);
			}
		 
			syms = this-> discardAllLocals ();
		    
			this-> quitBlock ();
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;
		    
		    popReferent ("validateClass");
		    exitForeign ();
		}
		
		if (errors.size () != 0) {
		    sym.to <semantic::Class> ().setGenerator (NoneType::init (cls.getName ()));
		    THROW (ErrorCode::EXTERNAL, errors);
		}	       
		
		{
		    TRY (
			if (!ancestor.isEmpty ()) {
			    fieldsDecl = ancestor.to <ClassRef> ().getRef ().to <semantic::Class> ().getGenerator ().to <generator::Class> ().getFields ();
			    ancestorFields = fieldsDecl;
			    for (auto & it : fieldsDecl) { // Verify shadowing 
				auto gen = syms.find (it.to <generator::VarDecl> ().getName ());
				if (gen != syms.end ()) {
				    auto note = Ymir::Error::createNote (it.getLocation ());		
				    Error::occurAndNote (gen-> second.getLocation (), note, ExternalError::get (SHADOWING_DECL), it.to <generator::VarDecl> ().getName ());
				}
			    }
			}
		
			for (auto & it : sym.to <semantic::Class> ().getFields ()) {
			    auto gen = syms.find (it.to <syntax::VarDecl> ().getName ().str);			    
			    fieldsDecl.push_back (gen-> second);
			}

			gen.to <generator::Class> ().setFields (fieldsDecl);
			sym.to <semantic::Class> ().setGenerator (gen);

			std::vector <generator::Class::MethodProtection> protections;
			auto vtable = validateClassDeclarations (cls, ClassRef::init (cls.getName (), ancestor, sym), ancestor, ancestorFields, protections, addMethods);
			gen.to <generator::Class> ().setVtable (vtable);
			gen.to <generator::Class> ().setProtectionVtable (protections);
			sym.to <semantic::Class> ().setGenerator (gen);
			sym.to <semantic::Class> ().setTypeInfo (validateTypeInfo (gen.getLocation (), ClassRef::init (cls.getName (), ancestor, sym)));
			sym.to <semantic::Class> ().setAddMethods (addMethods); // We don't put them in the table of the symbol, because they are not declared in it

		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;
		}
		
		if (errors.size () != 0) {
		    sym.to <semantic::Class> ().setGenerator (NoneType::init (cls.getName ()));
		    THROW (ErrorCode::EXTERNAL, errors);
		}
		
	    }

	    if (inModule) {
		std::vector <std::string> errors;
		TRY (
		    validateInnerClass (cls);
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
		
		if (errors.size () != 0) {
		    auto sym = cls;
		    sym.to <semantic::Class> ().setGenerator (NoneType::init (cls.getName ()));
		    THROW (ErrorCode::EXTERNAL, errors);
		}
	    }
	    
	    if (cls.to <semantic::Class> ().getGenerator ().is <generator::Class> ())
		return ClassRef::init (cls.getName (), ancestor, cls);
	    else {
		Ymir::Error::occur (cls.getName (), ExternalError::get (INCOMPLETE_TYPE_CLASS), cls.getRealName ());
		return Generator::empty ();
	    }
	}

	std::vector<generator::Generator> Visitor::validateClassDeclarations (const semantic::Symbol & cls, const Generator & classType, const Generator & ancestor, const std::vector <Generator> &, std::vector <generator::Class::MethodProtection>  & protection, std::vector <Symbol> & addMethods)  {
	    std::vector <Generator> vtable;
	    std::vector <Generator> ancVtable;
	    std::vector <std::string> errors;
	    if (!ancestor.isEmpty ()) {
		auto & ancClas = ancestor.to <ClassRef> ().getRef ().to <semantic::Class> ().getGenerator ();
		vtable = ancClas.to <generator::Class> ().getVtable ();
		protection = ancClas.to <generator::Class> ().getProtectionVtable ();
		ancVtable = vtable;
		for (auto & it : protection) {
		    if (it == generator::Class::MethodProtection::PRV)
			it = generator::Class::MethodProtection::PRV_PARENT;
		}	       
	    }
	    
	    
	    for (auto it : cls.to <semantic::Class> ().getAllInner ()) {
		TRY (
		    match (it) {
			of (semantic::Function, func ATTRIBUTE_UNUSED, {				
				int ignore = 0;
				validateVtableMethod (func, classType, ancestor, vtable, protection, ancVtable, ignore);
				if (func.getContent ().getBody ().getBody ().isEmpty () && !cls.to <semantic::Class> ().isAbs ()) {
				    Ymir::Error::occur (vtable [ignore].getLocation (), ExternalError::get (NO_BODY_METHOD), vtable [ignore].prettyString ());
				}
			    }
			);
		    }		    		
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }

	    // We want to validate implementation afterwards to have clearer errors
	    // And we don't want to be able to override impl methods outside the impl declaration scope
	    for (auto it : cls.to <semantic::Class> ().getAllInner ()) {
		TRY (
		    match (it) {
			of (semantic::Impl, im, {
				validateVtableImplement (im, classType, ancestor, vtable, protection, ancVtable, addMethods);
			    }
			);
		    }		    		
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }
	    
	    if (errors.size () != 0) 
		THROW (ErrorCode::EXTERNAL, errors);		

	    if (!cls.to <semantic::Class> ().isAbs ()) {
		for (auto & it : vtable) {
		    if (it.to <MethodProto> ().isEmptyFrame ()) {
			auto note = Ymir::Error::createNote (it.getLocation ());
			auto loc = classType.getLocation ();
			Ymir::Error::occurAndNote (loc, note, ExternalError::get (NOT_ABSTRACT_NO_OVER), classType.prettyString (), it.prettyString ());
		    }
		}
	    }
	    
	    if (errors.size () != 0) 
		THROW (ErrorCode::EXTERNAL, errors);
		
	    return vtable;
	}

	Generator Visitor::validateFunctionType (const Generator & proto) {
	    auto params = proto.to <FrameProto> ().getParameters ();
	    auto ret = proto.to <FrameProto> ().getReturnType ();
	    std::vector <Generator> paramTypes;
	    for (auto & it : params) {
		paramTypes.push_back (it.to <generator::ProtoVar> ().getType ());
		if (it.to <generator::ProtoVar> ().isMutable ())
		    paramTypes.back ().to <Type> ().isMutable (true);
		if (it.to <generator::ProtoVar> ().getType ().to <Type> ().isRef ())
		    paramTypes.back ().to <Type> ().isRef (true);
	    }
	    
	    return FuncPtr::init (proto.getLocation (), ret, paramTypes);
	}

	void Visitor::validateVtableImplement (const semantic::Impl & impl, const Generator & classType, const Generator& ancestor, std::vector <Generator> & vtable, std::vector <generator::Class::MethodProtection> & protection, const std::vector <Generator> & ancVtable, std::vector <Symbol> & addMethods) {
	    auto trait = this-> validateType (impl.getTrait ());
	    if (!trait.is <TraitRef> ()) {
		Ymir::Error::occur (impl.getTrait ().getLocation (), ExternalError::get (IMPL_NO_TRAIT), trait.prettyString ());
	    }
	    
	    std::vector <std::string> errors;
	    std::vector <Generator> loc_vtable;
	    std::vector <Symbol> loc_addMethods;
	    
	    std::vector <generator::Class::MethodProtection> loc_protection;
	    for (auto it : trait.to <TraitRef> ().getRef ().to <semantic::Trait> ().getAllInner ()) {
		TRY (
		    match (it) {
			of (semantic::Function, func ATTRIBUTE_UNUSED, {
				int i = 0;
				validateVtableMethod (func, classType, ancestor, loc_vtable, loc_protection, ancVtable, i);
				loc_addMethods.push_back (it);
			    }
			);
		    }
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    errors.push_back (Ymir::Error::createNote (impl.getTrait ().getLocation (), ExternalError::get (IN_TRAIT_VALIDATION)));
		} FINALLY;
	    }

	    std::vector <Generator> loc_vtable_impl = loc_vtable;
	    for (auto it : impl.getAllInner ()) {
		TRY (
		    match (it) {
			of (semantic::Function, func ATTRIBUTE_UNUSED, {
				int i = 0;
				validateVtableMethod (func, classType, ancestor, loc_vtable_impl, loc_protection, loc_vtable, i);
				if (i >= (int) loc_addMethods.size ()) {
				    std::string name;
				    for (auto & it : loc_vtable) {
					if (Ymir::Path (it.to <FrameProto> ().getName (), "::").fileName ().toString () == func.getName ().str)
					    name = name + Ymir::Error::createNoteOneLine (ExternalError::get (CANDIDATE_ARE), it.getLocation (), it.prettyString ());
				    }				
				    Ymir::Error::occurAndNote (func.getName (), name, ExternalError::get (TRAIT_NO_METHOD), trait.prettyString (), loc_vtable_impl [i].prettyString ());
				} else loc_addMethods [i] = it;
			    }
			);
		    }
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    errors.push_back (Ymir::Error::createNote (impl.getTrait ().getLocation (), ExternalError::get (IN_TRAIT_VALIDATION)));
		} FINALLY;
	    }	    
	    
	    {
		for (auto & it : loc_vtable_impl) {
		    auto protoFptr = validateFunctionType (it);
		    TRY (
			for (auto i : Ymir::r (ancVtable.size (), vtable.size ())) { // Verification of the collision (since all reimplemented function are marked override)
			    if (Ymir::Path (vtable[i].to <FrameProto> ().getName (), "::").fileName ().toString () ==
				Ymir::Path (it.to <FrameProto> ().getName (), "::").fileName ().toString ()) {
				auto fptr = validateFunctionType (vtable [i]);
				if (protoFptr.equals (fptr) && vtable [i].to<MethodProto> ().isMutable () == it.to<MethodProto> ().isMutable ()) {
				    auto note = Ymir::Error::createNote (vtable [i].getLocation ());
				    Ymir::Error::occurAndNote (it.getLocation (), note, ExternalError::get (CONFLICTING_DECLARATIONS), vtable [i].prettyString ());
				}
			    }
			}
			vtable.push_back (it);			
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
			errors.push_back (Ymir::Error::createNote (impl.getTrait ().getLocation (), ExternalError::get (IN_TRAIT_VALIDATION)));
		    } FINALLY;
		}
	    }
	    
	    for (auto & it : loc_protection)
		protection.push_back (it);
	    
	    volatile int i = 0;
	    for (auto & it : loc_addMethods) {
		addMethods.push_back (it);
		TRY (
		    if (it.to <semantic::Function> ().getContent ().getBody ().getBody ().isEmpty () && !classType.to <ClassRef> ().getRef ().to <semantic::Class> ().isAbs ()) {
			Ymir::Error::occur (loc_vtable_impl [i].getLocation (), ExternalError::get (NO_BODY_METHOD), loc_vtable_impl [i].prettyString ());
		    }
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    errors.push_back (Ymir::Error::createNote (impl.getTrait ().getLocation (), ExternalError::get (IN_TRAIT_VALIDATION)));
		} FINALLY;
		i += 1;
	    }
	    
	    if (errors.size () != 0) 
		THROW (ErrorCode::EXTERNAL, errors);
	    
	}
	
	void Visitor::validateVtableMethod (const semantic::Function & func, const Generator & classType, const Generator &, std::vector <Generator> & vtable, std::vector <generator::Class::MethodProtection> & protection, const std::vector <Generator> & ancVtable, int & index) {
	    
	    auto proto = validateMethodProto (func, classType);
	    auto protoFptr = validateFunctionType (proto);
	    bool over = false;
	    for (auto i : Ymir::r (0, ancVtable.size ())) {
		if (Ymir::Path (ancVtable[i].to <FrameProto> ().getName (), "::").fileName ().toString () == func.getName ().str) {
		    auto fptr = validateFunctionType (ancVtable [i]);
		    if (protoFptr.equals (fptr) && ancVtable [i].to<MethodProto> ().isMutable () == proto.to<MethodProto> ().isMutable ()) {
			if (!func.isOver ()) {
			    auto note = Ymir::Error::createNote (ancVtable [i].getLocation ());
			    Ymir::Error::occurAndNote (func.getName (), note, ExternalError::get (IMPLICIT_OVERRIDE), ancVtable[i].prettyString ());
			} else if (protection [i] == generator::Class::MethodProtection::PRV_PARENT) {
			    auto note = Ymir::Error::createNote (ancVtable [i].getLocation ());
			    Ymir::Error::occurAndNote (func.getName (), note, ExternalError::get (OVERRIDE_PRIVATE), ancVtable[i].prettyString ());
			} else if ((func.isPublic () && protection [i] != generator::Class::MethodProtection::PUB) ||
				   (func.isProtected () && protection [i] != generator::Class::MethodProtection::PROT)) {
			    auto note = Ymir::Error::createNote (ancVtable [i].getLocation ());
			    Ymir::Error::occurAndNote (func.getName (), note, ExternalError::get (OVERRIDE_MISMATCH_PROTECTION), proto.prettyString ());
			} else if (!func.isPublic () && !func.isProtected ()) {
			    auto note = Ymir::Error::createNote (ancVtable [i].getLocation ());
			    Ymir::Error::occurAndNote (func.getName (), note, ExternalError::get (CANNOT_OVERRIDE_AS_PRIVATE), proto.prettyString ());
			} else if (ancVtable [i].to <MethodProto> ().isFinal ()) {
			    auto note = Ymir::Error::createNote (ancVtable [i].getLocation ());
			    Ymir::Error::occurAndNote (func.getName (), note, ExternalError::get (CANNOT_OVERRIDE_FINAL), ancVtable [i].prettyString ()); 
			}
			
			over = true;
			vtable [i] = proto; // We do that afterward, when impl a trait the vtable might be empty and always different from ancVtable
			// But we can ensure that an error will be thrown before then, since the func is never marked override in a trait
			index = i;
			
			break;
		    }
		}		
	    }
	    
	    if (!over) { 
		if (func.isOver ()) {
		    std::string name;
		    for (auto & it : ancVtable) {
			if (Ymir::Path (it.to <FrameProto> ().getName (), "::").fileName ().toString () == func.getName ().str)
			    name = name + Ymir::Error::createNoteOneLine (ExternalError::get (CANDIDATE_ARE), it.getLocation (), it.prettyString ());
		    }
		    Ymir::Error::occurAndNote (func.getName (), name, ExternalError::get (NOT_OVERRIDE), proto.prettyString ());
		}

		for (auto i : Ymir::r (ancVtable.size (), vtable.size ())) {
		    if (Ymir::Path (vtable[i].to <FrameProto> ().getName (), "::").fileName ().toString () == func.getName ().str) {
			auto fptr = validateFunctionType (vtable [i]);
			if (protoFptr.equals (fptr) && vtable [i].to<MethodProto> ().isMutable () == proto.to<MethodProto> ().isMutable ()) {
			    auto note = Ymir::Error::createNote (vtable [i].getLocation ());
			    Ymir::Error::occurAndNote (func.getName (), note, ExternalError::get (CONFLICTING_DECLARATIONS), vtable [i].prettyString ());
			}
		    }
		}

		index = vtable.size ();
		vtable.push_back (proto);
		if (func.isPublic ())
		    protection.push_back (generator::Class::MethodProtection::PUB);
		else if (func.isProtected ())
		    protection.push_back (generator::Class::MethodProtection::PROT);
		else
		    protection.push_back (generator::Class::MethodProtection::PRV);
	    }
	}
	
	void Visitor::validateConstructor (const semantic::Symbol & sym, const Generator & classType_, const Generator & ancestor, const std::vector <Generator> & ancestorFields) {
	    auto & cs = sym.to <Constructor> ();
	    auto constr = cs.getContent ();
	    std::vector <Generator> params;
	    Generator retType (Generator::empty ());
	    std::vector <std::string> errors;
	    auto classType = classType_;
	    
	    auto proto = validateConstructorProto (sym);
	    verifyConstructionLoop (proto.getLocation (), proto);
	    
	    enterClassDef (classType_.to <ClassRef> ().getRef ());
	    classType.to <Type> ().isMutable (true);
	    enterForeign ();

	    enterBlock ();
	    TRY (
		validatePrototypeForFrame (cs.getName (), constr.getPrototype (), params, retType);
		retType = classType.to <ClassRef> ().getRef ().to <semantic::Class> ().getGenerator ().to <Value> ().getType ();
		params.insert (params.begin (), ParamVar::init (cs.getName (), classType, true));
		insertLocal (params [0].getName (), params [0]);
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors = msgs;
	    } FINALLY;

	    Generator body (Generator::empty ());
	    {
		TRY (
		    auto preConstruct = validatePreConstructor (cs, classType_, ancestor, ancestorFields);
		    this-> setCurrentFuncType (retType);
		    body = validateValue (constr.getBody ());
		    auto loc = constr.getBody ().getLocation ();
		    auto ret = Return::init (loc,
					     Void::init (loc),
					     classType,
					     VarRef::init (loc, params [0].to <ParamVar> ().getName (), classType, params [0].getUniqId (), true, Generator::empty ())
		    );	    
		    body = Block::init (loc, Void::init (loc), {preConstruct, body, ret});
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }

	    {
		TRY (
		    quitBlock ();
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }
	    
	    exitForeign ();
	    exitClassDef ();
	    
	    if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);
	    
	    auto frame = Frame::init (constr.getName (), cs.getRealName (), params, classType, body, false);
	    frame.to <Frame> ().setMangledName (cs.getMangledName ());
	    frame.to <Frame> ().isWeak (cs.isWeak ());
	    insertNewGenerator (frame);
	}

	void Visitor::validateMethod (const semantic::Function & func, const Generator & classType_, bool isWeak) {
	    auto function = func.getContent ();
	    std::vector <Generator> params;
	    Generator retType (Generator::empty ());
	    std::vector <std::string> errors;
	    
	    auto classType = classType_;
	    auto & cs = classType.to <ClassRef> ().getRef ().to <semantic::Class> ();

	    enterClassDef (classType.to <ClassRef> ().getRef ());
	    classType.to <Type> ().isMutable (true);
	    enterForeign ();	    
	    enterBlock ();
	    
	    TRY (
		bool isMutable = false;
		for (auto & it : function.getPrototype ().getParameters ()[0].to <syntax::VarDecl> ().getDecorators ()) {
		    if (it.getValue () == syntax::Decorator::MUT) isMutable = true;
		    else {
			Ymir::Error::occur (it.getLocation (),
					    ExternalError::get (DECO_OUT_OF_CONTEXT),
					    it.getLocation ().str
			);				
		    }
		}
		
		classType.to <Type> ().isMutable (isMutable);
		params.insert (params.begin (), ParamVar::init ({cs.getName (), Keys::SELF}, classType, isMutable));
		insertLocal (params [0].getName (), params [0]);

		auto & __params = function.getPrototype ().getParameters ();
		auto fakeParams = std::vector <syntax::Expression> (__params.begin () + 1, __params.end ());
		auto proto = syntax::Function::Prototype::init (fakeParams, function.getPrototype ().getType (), false);
		
		validatePrototypeForFrame (cs.getName (), proto, params, retType);
		if (retType.isEmpty ()) retType = Void::init (func.getName ());
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors = msgs;
	    } FINALLY;
	    
	    volatile bool needFinalReturn = false;
	    Generator body (Generator::empty ());
	    if (errors.size () == 0) 
		{
		    TRY (
			this-> setCurrentFuncType (retType);
			body = validateValue (function.getBody ().getBody ());
		
			if (!body.to<Value> ().isReturner ()) {
			    verifyMemoryOwner (body.getLocation (), retType, body, true);		    
			    needFinalReturn = !retType.is<Void> ();
			}
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;
		}
	    
	    {
		TRY (
		    if (errors.size () != 0)
			this-> discardAllLocals ();
		    
		    quitBlock ();
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }
	    
	    exitForeign ();
	    exitClassDef ();

	    if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);
		
	    auto frame = Frame::init (function.getName (), func.getRealName (), params, retType, body, needFinalReturn);
	    frame.to <Frame> ().isWeak (func.isWeak () || isWeak);
	    frame.to <Frame> ().setMangledName (func.getMangledName ());
	    
	    insertNewGenerator (frame);		
	}

	generator::Generator Visitor::getClassConstructors (const lexing::Word & loc, const generator::Generator & cl) {
	    bool prot = false, prv = false;
	    std::vector <std::string> errors;
	    getClassContext (cl.to <generator::Class> ().getRef (), prv, prot);
	    std::vector <Symbol> syms;
	    for (auto & gen : cl.to <generator::Class> ().getRef ().to <semantic::Class> ().getAllInner ()) {
		match (gen) {
		    of (semantic::Constructor, cst ATTRIBUTE_UNUSED, {
			    if (prv || (prot && gen.isProtected ()) || gen.isPublic ()) 
				syms.push_back (gen);
			    else {
				errors.push_back (
				    Ymir::Error::createNoteOneLine (ExternalError::get (PRIVATE_IN_THIS_CONTEXT), gen.getName (), validateConstructorProto (gen).prettyString ())					    
				);
			    }
			});
		}
	    }
	    
	    if (syms.size () != 0) 
		return validateMultSym (loc, syms);
	    else if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);
	    
	    return Generator::empty ();
	}

	std::vector <syntax::Declaration> Visitor::getAllConstructors (const std::vector <syntax::Declaration> & decls) {
	    std::vector <syntax::Declaration> results;
	    for (auto & it : decls) {
		match (it) {
		    of (syntax::Constructor, cs ATTRIBUTE_UNUSED, results.push_back (it))
		    else of (syntax::DeclBlock, dc, {
			    auto inner = getAllConstructors (dc.getDeclarations ());
			    results.insert (results.end (), inner.begin (), inner.end ());
			}
		    ) else of (syntax::CondBlock, cd, {
			    auto inner = getAllConstructors (cd.getDeclarations ());
			    results.insert (results.end (), inner.begin (), inner.end ());
			    if (!cd.getElse ().isEmpty ()) {
				auto inner = getAllConstructors ({cd.getElse ()});
				results.insert (results.end (), inner.begin (), inner.end ());
			    }
			}
		    );
		}
	    }
	    return results;
	}       
	
	generator::Generator Visitor::validatePreConstructor (const semantic::Constructor & cs, const Generator & classType, const Generator & ancestor, const std::vector<Generator> & ancestorFields) {
	    auto & superParams = cs.getContent ().getSuperParams ();
	    auto classR = classType;
	    std::vector <Generator> instructions;
	    std::vector <std::string> errors;
	    
	    if (!cs.getContent ().getExplicitSuperCall ().isEof () && ancestor.isEmpty ())		
		Ymir::Error::occur (cs.getContent ().getExplicitSuperCall (), ExternalError::get (NO_SUPER_FOR_CLASS), classR.prettyString ());
	    
	    if (!cs.getContent ().getExplicitSelfCall ().isEof ()) {
		if (cs.getContent ().getFieldConstruction ().size () != 0)
		    Ymir::Error::occur (cs.getContent ().getFieldConstruction ()[0].first, ExternalError::get (MULTIPLE_FIELD_INIT), cs.getContent ().getFieldConstruction ()[0].first.str);
		
		auto loc = cs.getContent ().getExplicitSelfCall ();
		auto superBin = TemplateSyntaxWrapper::init (loc, getClassConstructors (loc, classR.to <ClassRef> ().getRef ().to <semantic::Class> ().getGenerator ()));				      
		auto call = syntax::MultOperator::init ({loc, Token::LPAR}, {loc, Token::RPAR}, superBin, superParams);
		auto result = validateValue (call);
		result.to <ClassCst> ().setSelf (validateValue (syntax::Var::init ({loc, Keys::SELF})));
		instructions.push_back (result);
	    } else {
		if (!ancestor.isEmpty ()) {
		    auto loc = cs.getContent ().getExplicitSuperCall ();
		    if (loc.isEof ()) loc = cs.getName ();
		    auto superBin = TemplateSyntaxWrapper::init (loc, getClassConstructors (loc, ancestor.to <ClassRef> ().getRef ().to <semantic::Class> ().getGenerator ()));				      

		    auto call = syntax::MultOperator::init ({loc, Token::LPAR}, {loc, Token::RPAR}, superBin, superParams);
		    auto result = validateValue (call);
		    result.to <ClassCst> ().setSelf (validateValue (syntax::Var::init ({loc, Keys::SELF})));
		    instructions.push_back (result);
		}

		std::set <std::string> validated;
		for (auto & it : ancestorFields) validated.emplace (it.to <generator::VarDecl> ().getName ());
		for (auto & it : cs.getContent ().getFieldConstruction ()) {
		    auto name = it.first;
		    auto access = syntax::Binary::init ({name, Token::DOT},
							syntax::Var::init ({name, Keys::SELF}),
							syntax::Var::init (name), syntax::Expression::empty ());
		    TRY (		
			if (validated.find (name.str) != validated.end ()) {
			    Ymir::Error::occur (name, ExternalError::get (MULTIPLE_FIELD_INIT), name.str);
			}
		    
			auto left = this-> validateValue (access);
			auto right = this-> validateValue (it.second);
			verifyMemoryOwner (left.getLocation (), left.to <Value> ().getType (), right, true);
			instructions.push_back (Affect::init (left.getLocation (), left.to <Value> ().getType (), left, right, true));			
			validated.emplace (name.str);
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;		    
		}

		if (errors.size () != 0)
		    THROW (ErrorCode::EXTERNAL, errors);
		
		for (auto & it : classR.to <ClassRef> ().getRef ().to <semantic::Class> ().getFields ()) {
		    if (validated.find (it.to <syntax::VarDecl> ().getName ().str) == validated.end ()) {
			if (it.to <syntax::VarDecl> ().getValue ().isEmpty ()) {
			    auto note = Ymir::Error::createNote (cs.getName ());
			    Error::occurAndNote (it.to <syntax::VarDecl> ().getLocation (), note, ExternalError::get (UNINIT_FIELD), it.to <syntax::VarDecl> ().getName ().str);
			} else {
			    auto name = it.to <syntax::VarDecl> ().getName ();
			    auto access = syntax::Binary::init ({name, Token::DOT},
								syntax::Var::init ({name, Keys::SELF}),
								syntax::Var::init (name), syntax::Expression::empty ());
			    auto left = this-> validateValue (access);
			    auto right = this-> validateValue (it.to <syntax::VarDecl> ().getValue ());
			    instructions.push_back (Affect::init (left.getLocation (), left.to <Value> ().getType (), left, right));
			}
		    }
		}
	    }
	    
	    if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);
	    
	    auto loc = cs.getName ();
	    return Block::init (loc, Void::init (loc), instructions);
	}

	void Visitor::verifyConstructionLoop (const lexing::Word & location, const Generator & call) {
	    static std::vector <Symbol> protos;
	    static std::vector <Generator> gen_protos;
	    static std::vector <lexing::Word> locs;
	    Symbol sym (Symbol::empty ());
	    Generator current_proto (Generator::empty ());
	    Generator clRef (Generator::empty ());
	    
	    if (call.is <Call> () && call.to <Call> ().getFrame ().is <ConstructorProto> ()) {
		sym = call.to <Call> ().getFrame ().to <ConstructorProto> ().getRef ();
		current_proto = call.to <Call> ().getFrame ();
		clRef = call.to <Call> ().getFrame ().to <ConstructorProto> ().getReturnType ();	    
	    } else if (call.is <ConstructorProto> ()) {
		sym = call.to <ConstructorProto> ().getRef ();
		current_proto = call;
		clRef = call.to <ConstructorProto> ().getReturnType ();	    
	    } else if (call.is <ClassCst> ()) {
		sym = call.to <ClassCst> ().getFrame ().to <ConstructorProto> ().getRef ();
		current_proto = call.to <ClassCst> ().getFrame ();
		clRef = call.to <ClassCst> ().getFrame ().to <ConstructorProto> ().getReturnType ();	    
	    } else return; // This is not a class constructor, we can't check that
	    
	    auto & cs = sym.to <semantic::Constructor> ();	    
	    for (auto & it : protos) {
		if (it.equals (sym)) {		    
		    std::string note;
		    for (auto z : Ymir::r (0, locs.size ())) {
			if (z != 0) note += "\n";
			note = note + Ymir::Error::createNote (locs [z], gen_protos [z].prettyString ());
		    }
		    Ymir::Error::occurAndNote (call.getLocation (), note, ExternalError::get (INFINITE_CONSTRUCTION_LOOP));
		}
	    }

	    pushReferent (sym, "verifyConstructionLoop");
	    protos.push_back (sym);
	    gen_protos.push_back (current_proto);
	    locs.push_back (location);
	    
	    std::vector <std::string> errors;
	    std::vector <Generator> params;
	    Generator retType (Generator::empty ());
	    
	    enterClassDef (clRef.to <ClassRef> ().getRef ());
	    enterForeign ();
	    enterBlock ();

	    {
		TRY (
		    validatePrototypeForFrame (cs.getName (), cs.getContent ().getPrototype (), params, retType);
		    retType = clRef.to <ClassRef> ().getRef ().to <semantic::Class> ().getGenerator ().to <Value> ().getType ();
		    params.insert (params.begin (), ParamVar::init (cs.getName (), clRef, true));
		    insertLocal (params [0].getName (), params [0]);
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors = msgs;
		} FINALLY;
	    }
	    
	    TRY (
		// If this is just a construction redirection, there is no need to check 
		if (cs.getContent ().getExplicitSelfCall ().isEof ()) {
		    std::set <std::string> validated;
		    auto & superParams = cs.getContent ().getSuperParams ();
		    if (!clRef.to <ClassRef> ().getAncestor ().isEmpty ()) {
			auto ancestor = clRef.to <ClassRef> ().getAncestor ();
			auto loc = cs.getContent ().getExplicitSuperCall ();
			if (loc.isEof ()) loc = cs.getName ();
			auto superBin = TemplateSyntaxWrapper::init (loc, getClassConstructors (loc, ancestor.to <ClassRef> ().getRef ().to <semantic::Class> ().getGenerator ()));				      
		    
			auto call = syntax::MultOperator::init ({loc, Token::LPAR}, {loc, Token::RPAR}, superBin, superParams);
			auto result = validateValue (call);
			verifyConstructionLoop (loc, result);
		    }

		    for (auto & it : cs.getContent ().getFieldConstruction ()) {
			auto right = this-> validateValue (it.second);
			if (right.to <Value> ().getType ().is <ClassRef> ()) {
			    locs.back () = it.second.getLocation ();
			    verifyConstructionLoop (it.second.getLocation (), right);
			    validated.emplace (it.first.str);
			}
		    }

		    for (auto & it : clRef.to<ClassRef> ().getRef ().to <semantic::Class> ().getFields ()) {
			if (validated.find (it.to<syntax::VarDecl> ().getName ().str) == validated.end ()) {
			    if (!it.to <syntax::VarDecl> ().getValue ().isEmpty ()) {
				auto right = this-> validateValue (it.to <syntax::VarDecl> ().getValue ());
				if (right.to <Value> ().getType ().is <ClassRef> ()) {
				    auto loc = it.to <syntax::VarDecl> ().getValue ().getLocation ();
				    locs.back () = loc;
				    verifyConstructionLoop (loc, right);
				}
			    }
			}
		    }
		}		
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors.insert (errors.end (), msgs.begin (), msgs.end ());
	    } FINALLY;

	    {
		TRY (
		    this-> discardAllLocals ();
		    quitBlock ();
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }

	    exitForeign ();
	    exitClassDef ();	    	    
	    protos.pop_back ();
	    gen_protos.pop_back ();
	    locs.pop_back ();	    
	    popReferent ("verifyConstructionLoop");

	    if (errors.size () != 0) {
		THROW (ErrorCode::EXTERNAL, errors);
	    }
	}	

	generator::Generator Visitor::validateEnum (const semantic::Symbol & en) {
	    if (en.to<semantic::Enum> ().getGenerator ().isEmpty ()) {
		auto sym = en;
		auto gen = generator::Enum::init (sym.getName (), sym);
		sym.to<semantic::Enum> ().setGenerator (gen);

		Generator type (Generator::empty ());
		std::vector <std::string> errors;
		std::map <std::string, generator::Generator> syms;		

		enterForeign ();
		pushReferent (en, "validateEnum");
		TRY (		
		    this-> enterBlock ();

		    if (!sym.to<semantic::Enum>().getType ().isEmpty ())
			type = validateType (sym.to<semantic::Enum> ().getType (), true);
		    
		    std::vector <std::string> fields;
		    if (sym.to<semantic::Enum> ().getFields ().size () == 0) {
			// Error
		    }
		    
		    for (auto & it : sym.to <semantic::Enum> ().getFields ()) {
			match (it) {
			    of (syntax::VarDecl, decl, {
				    if (decl.getValue ().isEmpty ()) {
					Ymir::Error::occur (decl.getName (), ExternalError::get (EN_NO_VALUE), decl.getName ().str);
				    }
				}
			    );
			}
			
			auto val = this-> validateValue (it);
			if (type.isEmpty ()) {
			    type = val.to <generator::VarDecl> ().getVarType ();
			    type.changeLocation (gen.getLocation ());
			} else verifyCompatibleType (val.getLocation (), type, val.to<generator::VarDecl> ().getVarType ());
		    }
		    
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;

		{
		    TRY (
			syms = this-> discardAllLocals ();
			this-> quitBlock ();
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;
		}
		
		popReferent ("validateEnum");
		exitForeign ();

		if (errors.size () != 0) {
		    sym.to <semantic::Enum> ().setGenerator (NoneType::init (sym.getName ()));
		    THROW (ErrorCode::EXTERNAL, errors);
		}
		
		std::vector <Generator> fieldsDecl;
		for (auto & it : sym.to <semantic::Enum> ().getFields ()) {
		    auto gen = syms.find (it.to <syntax::VarDecl> ().getName ().str);		    
		    fieldsDecl.push_back (gen-> second);
		}

		type.to <Type> ().setProxy (EnumRef::init (en.getName (), sym));
		gen.to <generator::Enum> ().setFields (fieldsDecl);
		gen.to <generator::Enum> ().setType (type);
		
		sym.to <semantic::Enum> ().setGenerator (gen);
	       		
		return type;
	    }
	    auto gen = en.to <semantic::Enum> ().getGenerator ();
	    if (gen.is <semantic::generator::Enum> ())
		return gen.to <semantic::generator::Enum> ().getType ();
	    else {
		Ymir::Error::occur (en.getName (), ExternalError::get (INCOMPLETE_TYPE_CLASS), en.getRealName ());
		return Generator::empty ();
	    }
	}

	void Visitor::verifyRecursivity (const lexing::Word & loc, const generator::Generator & gen, const semantic::Symbol & sym) const {
	    match (gen) {
		of (StructRef, str_ref, {
			if (str_ref.isRefOf (sym)) {
			    auto note = Ymir::Error::createNote (sym.getName ());
			    Ymir::Error::occurAndNote (loc, note, ExternalError::get (NO_SIZE_FORWARD_REF));
			} else {
			    auto & str = str_ref.getRef ().to <semantic::Struct> ().getGenerator ();
			    for (auto & it : str.to<generator::Struct> ().getFields ()) {
				verifyRecursivity (loc, it.to <generator::VarDecl> ().getVarType (), sym);
			    }
			}
		    })
		else of (Pointer, p ATTRIBUTE_UNUSED, // No forward problem on pointer types
		)
		else of (Slice, s ATTRIBUTE_UNUSED, ) // No problem for slice, their size can be 0			 
		else of (Type, t, {
			if (t.isComplex ()) {
			    for (auto & it : t.getInners ()) verifyRecursivity (loc, it, sym);
			}
		    });
	    }
	}
	
	Generator Visitor::validateValue (const syntax::Expression & expr, bool canBeType, bool fromCall) {
	    Generator value (Generator::empty ());
	    if (canBeType) 
		value = validateValueNoReachable (expr, true);
	    else
		value = validateValueNoReachable (expr, fromCall);
	    // If it can be a type, that means we are looking for a type, and that implicit call is not an option
	    
	    if (!value.is <Value> () && !canBeType) {
		auto note = Ymir::Error::createNote (expr.getLocation ());
		Ymir::Error::occurAndNote (expr.getLocation (), note, ExternalError::get (USE_AS_VALUE));
	    }
	    
	    if (value.is <Value> () && value.to <Value> ().isBreaker ())
		Ymir::Error::occur (value.getLocation (), ExternalError::get (BREAK_INSIDE_EXPR));

	    
	    return value;
	}

	Generator Visitor::validateCteValue (const syntax::Expression & value) {
	    match (value) {
		of (syntax::If, fi,
		    return validateCteIfExpression (fi);
		) else of (syntax::Assert, as,
		    return validateCteAssert (as);
		) else of (syntax::Block, bl ATTRIBUTE_UNUSED,
		    return validateValue (value);
		) else {
			return retreiveValue (validateValue (value));
		}		    
	    }
	    return Generator::empty ();
	}
	
	Generator Visitor::validateValueNoReachable (const syntax::Expression & value, bool fromCall) {
	    match (value) {
		of (syntax::Block, block,
		    return validateBlock (block);
		);
		
		of (syntax::Fixed, fixed,
		    return validateFixed (fixed);
		);

		of (syntax::Bool, b,
		    return validateBool (b);
		);
		
		of (syntax::Float, f,
		    return validateFloat (f);
		);

		of (syntax::Char, c,
		    return validateChar (c);
		);

		of (syntax::String, s,
		    return validateString (s);
		);
		
		of (syntax::Binary, binary,
		    return validateBinary (binary, fromCall);
		);
		
		of (syntax::Var, var,
		    return validateVar (var);
		);

		of (syntax::VarDecl, var,
		    return validateVarDeclValue (var);
		);

		of (syntax::Set, set,
		    return validateSet (set);
		);
		
		of (syntax::DecoratedExpression, dec_expr,
		    return validateDecoratedExpression (dec_expr);
		);

		of (syntax::If, _if,
		    return validateIfExpression (_if);
		);

		of (syntax::While, _while,
		    return validateWhileExpression (_while);
		);

		of (syntax::For, _for,
		    return validateForExpression (_for);
		);
		
		of (syntax::Break, _break,
		    return validateBreak (_break);
		);

		// of (syntax::Return, _return,
		//     return validateReturn (_return);
		// );
		
		of (syntax::List, list,
		    return validateList (list);
		);

		of (syntax::Intrinsics, intr,
		    return validateIntrinsics (intr);
		);

		of (syntax::Unit, u,
		    return None::init (u.getLocation ());
		);

		of (syntax::MultOperator, mult,
		    return validateMultOperator (mult);
		);

		of (syntax::Unary, un,
		    return validateUnary (un);
		);

		of (syntax::NamedExpression, named,
		    auto inner = validateValue (named.getContent ());
		    return NamedGenerator::init (named.getLocation (), inner);
		);

		of (syntax::TemplateCall, cl, {
			auto ret = validateTemplateCall (cl);
			if (!fromCall && ret.is <FrameProto> ()) {
			    std::vector <std::string> errors;
			    int score;
			    auto visit = CallVisitor::init (*this);			    
			    ret = visit.validateFrameProto (cl.getLocation (), ret.to <FrameProto> (), {}, score, errors);
			    if (errors.size () != 0) {
				THROW (ErrorCode::EXTERNAL, errors);
			    }			    
			} else if (!fromCall && ret.is <generator::Struct> ()) {
			    std::vector <std::string> errors;
			    int score;
			    auto visit = CallVisitor::init (*this);			    
			    auto sec_ret = visit.validateStructCst (cl.getLocation (), ret.to <generator::Struct> (), {}, score, errors);
			    if (errors.size () != 0) {
				THROW (ErrorCode::EXTERNAL, errors);
			    }
			    if (!sec_ret.isEmpty ()) ret = sec_ret;
			}
			return ret;
		    }
		);

		of (syntax::Return, rt,
		    return validateReturn (rt);
		);

		of (TemplateSyntaxList, lst,
		    return validateListTemplate (lst);
		);
		
		of (TemplateSyntaxWrapper, st,
		    return st.getContent ()
		);

		of (syntax::Cast, cst,
		    return validateCast (cst);
		);

		of (syntax::ArrayAlloc, alloc,
		    return validateArrayAlloc (alloc);
		);

		of (syntax::DestructDecl, destr,
		    return validateDestructDecl (destr);
		);

		of (syntax::Lambda, lmbd,
		    return validateLambda (lmbd);
		);

		of (syntax::FuncPtr, ptr,
		    return validateFuncPtr (ptr);
		);

		of (syntax::Null, nl,
		    return validateNullValue (nl);
		);

		of (syntax::TemplateChecker, ch,
		    return validateTemplateChecker (ch);
		);

		of (syntax::Throw, thr,
		    return validateThrow (thr);
		);
		
		of (syntax::Match, match,
		    return validateMatch (match);
		);

		of (syntax::Catch, cat,
		    return validateCatchOutOfScope (cat); // Out of scope is useless
		);

		of (syntax::Scope, scope, 
		    return validateScopeOutOfScope (scope); // Out of scope is useless
		);

		of (syntax::Assert, assert,
		    return validateAssert (assert);
		);
	    }

	    OutBuffer buf;
	    value.treePrint (buf, 0);
	    println (buf.str ());	    
	    
	    Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");
	    return Generator::empty ();
	}

	Generator Visitor::validateBlock (const syntax::Block & block) {
	    std::vector <Generator> values;
	    
	    Generator type (Void::init (block.getLocation ()));
	    bool breaker = false, returner = false;
	    std::vector <std::string> errors;
	    Symbol decl (Symbol::empty ());
	    { // We enter a sub scope to prevent TRY catch buffer shadow at compile time
		TRY (
		    enterBlock ();
		    decl = validateInnerModule (block.getDeclModule ());
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    decl = Symbol::empty ();
		} FINALLY;
	    }

	    if (!decl.isEmpty ()) {
		pushReferent (decl, "validateBlock");
	    }
	    
	    std::vector <Generator> onExit;
	    std::vector <Generator> onSuccess;
	    std::vector <Generator> onFailure;
	    std::vector <Generator> catchVars;
	    std::vector <Generator> catchInfos;
	    std::vector <Generator> catchActions;
	    std::vector <syntax::Expression> toValidate;
	    
	    // i is not really volatile, but the compiler seems to want it to be (due to TRY CATCHS)
	    for (volatile int i = 0 ; i < (int) block.getContent ().size () ; i ++) {
		TRY (
		    if (block.getContent ()[i].is <syntax::Scope> ()) {
			auto scope = block.getContent ()[i].to <syntax::Scope> ();
			if (scope.isExit ()) {
			    onExit.push_back (validateValue (scope.getContent()));
			    if (onExit.back ().to <Value> ().isReturner ()) returner = true;
			    if (onExit.back ().to <Value> ().isBreaker ()) breaker = true;
			} else if (scope.isSuccess ()) {
			    onSuccess.push_back (validateValue (scope.getContent ()));
			    if (onSuccess.back ().to <Value> ().isReturner ()) returner = true;
			    if (onSuccess.back ().to <Value> ().isBreaker ()) breaker = true;
			} else if (scope.isFailure ()) {
			    onFailure.push_back (validateValue (scope.getContent ()));
			} else Ymir::Error::occur (scope.getLocation (), ExternalError::get (UNDEFINED_SCOPE_GUARD), scope.getLocation ().str);
			
			if (i == (int) block.getContent ().size () - 1) { // Add Unit, scope guard does not have values
			    values.push_back (None::init (scope.getLocation ()));
			    type = Void::init (scope.getLocation ());
			}
		    } else if (block.getContent ()[i].is <syntax::Catch> ()) {
			toValidate.push_back (block.getContent ()[i]);
		    } else {
			if ((returner || breaker) && !block.getContent ()[i].is <syntax::Unit> ()) {			
			    Error::occur (block.getContent () [i].getLocation (), ExternalError::get (UNREACHBLE_STATEMENT));
			}
			auto value = validateValueNoReachable (block.getContent () [i]);
		    
			if (value.to <Value> ().isReturner ()) returner = true;
			if (value.to <Value> ().isBreaker ()) breaker = true;
			type = value.to <Value> ().getType ();
			type.to <Type> ().isRef (false);
			type.changeLocation (block.getContent ()[i].getLocation ());
			if (!value.is<Aliaser> () && !value.is<Referencer> ())
			    type.to <Type> ().isMutable (false);
		    
			values.push_back (value);
		    }
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }

	    if (errors.size () == 0) {
		// We do not verify useless expression, it is only warnings, and it must be verified after the all function has been verified
		for (auto & it : toValidate) { // Validation of the catchers
		    TRY (
			validateCatchers (it, catchVars, catchInfos, catchActions, type);
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());		    
		    } FINALLY;
		}
	    }

	    {
		TRY (
		    if (!type.is<Void> ()) {			
			verifyMemoryOwner (block.getEnd (), type, values.back(), false);
		    } else if (type.is<Void> () && values.size () != 0 && !values.back ().is <None> () && isUseless (values.back ()))
			Ymir::Error::occur (block.getContent ().back ().getLocation (), ExternalError::get (USE_UNIT_FOR_VOID));	    
		)  CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }

	    if (!decl.isEmpty ()) {
		popReferent ("validateBlock");
	    }
	    
	    {
		TRY (
		    
		    // If there are some errors, no need to add the warning about the unused vars
		    // Moreover, they may be not pertinent 
		    if (errors.size () != 0)
			discardAllLocals ();
		    
		    quitBlock ();
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }

	    if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);
	    
	    auto ret = Block::init (block.getLocation (), type, values);
	    ret.to <Value> ().isBreaker (breaker);
	    ret.to <Value> ().isReturner (returner);
	    
	    if (onSuccess.size () != 0) ret = SuccessScope::init (block.getLocation (), type, ret, onSuccess);
	    if (onExit.size () != 0 || onFailure.size () != 0 || catchVars.size () != 0) {
		auto jmp_buf_type = validateType (syntax::Var::init ({block.getLocation (), global::CoreNames::get (JMP_BUF_TYPE)}));
		onFailure.insert (onFailure.end (), onExit.begin (), onExit.end ());
		return ExitScope::init (block.getLocation (), type, jmp_buf_type, ret, onExit, onFailure, catchVars, catchInfos, catchActions);
	    }
	    return ret;
	}

	void Visitor::validateCatchers (const syntax::Expression & catcher, std::vector <Generator> & varDecl, std::vector <Generator> & typeInfos, std::vector <Generator> & actions, const generator::Generator& typeBlock) {
	    auto & vars = catcher.to <syntax::Catch> ().getVars ();
	    auto & syntaxActions = catcher.to <syntax::Catch> ().getActions ();
	    std::vector <std::string> errors;
	    for (auto it : Ymir::r (0, vars.size ())) {
		enterBlock ();
		{
		    TRY ( 
			auto var = validateVarDeclValue (vars [it].to <syntax::VarDecl> (), false);
			auto type = var.to <generator::VarDecl> ().getVarType ();
			auto simpleType = type;
			type.to <Type> ().isRef (true);
			type.to <Type> ().isMutable (false);
			
			simpleType.to <Type> ().isRef (false);
			simpleType.to <Type> ().isMutable (false);
			
			var = generator::VarDecl::init (var.getLocation (), var.getName (), type, Generator::empty (), false);
			if (var.getName () != Keys::UNDER)
			    insertLocal (var.getName (), var);
			
			if (!var.to <generator::VarDecl> ().getVarType ().isEmpty ()) {
			    for (auto & j : varDecl) {
				if (type.to <Type> ().isCompatible (j.to <generator::VarDecl> ().getVarType ())) {
				    auto note = Ymir::Error::createNote (j.getLocation ());
				    Ymir::Error::occurAndNote (var.getLocation (), note, ExternalError::get (CATCH_MULTIPLE_TIME), type.prettyString ());
				}		    
			    }			    
			    typeInfos.push_back (validateTypeInfo (var.getLocation (), simpleType));
			} else {
			    for (auto & j : varDecl) {
				if (j.to <generator::VarDecl> ().getName () == Keys::UNDER) {
				    auto note = Ymir::Error::createNote (j.getLocation ());
				    Ymir::Error::occurAndNote (var.getLocation (), note, ExternalError::get (CATCH_MULTIPLE_TIME), "any");
				}
			    }
			    typeInfos.push_back (None::init (catcher.getLocation ()));
			}									 
			varDecl.push_back (var);
			auto value = validateValue (syntaxActions [it]);
			if (!type.is<Void> ()) {
			    verifyMemoryOwner (syntaxActions [it].getLocation (), typeBlock, value, false);
			}
			actions.push_back (value);						
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;
		}
		{
		    TRY (
			quitBlock ();
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;
		}
	    }
	    
	    if (errors.size () != 0) {
		THROW (ErrorCode::EXTERNAL, errors);
	    }
	}

	Symbol Visitor::validateInnerModule (const syntax::Declaration & decl) {
	    if (decl.isEmpty ()) return Symbol::empty ();
	    auto sym = declarator::Visitor::init ().visit (decl);
	    if (!sym.isEmpty ()) {
		std::vector <std::string> errors;
		
		this-> _referent.back ().insert (sym);
		enterForeign ();
		TRY (
		    this-> validate (sym);
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;		
		exitForeign ();
		
		if (errors.size () != 0) {
		    THROW (ErrorCode::EXTERNAL, errors);
		}
		
	    }
	    return sym;
	}
	
	void Visitor::validateTemplateSymbol (const semantic::Symbol & sym, const Generator & gen) {
	    std::vector <std::string> errors;
	    
	    pushReferent (sym.getRef (), "validateTemplateSymbol");
	    
	    enterForeign ();	    
	    TRY (
		if (gen.is <MethodTemplateRef> () && sym.is <TemplateSolution> ())
		    this-> validateTemplateSolutionMethod (sym, gen.to <MethodTemplateRef> ().getSelf ());
		else 
		    this-> validate (sym);		
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors.insert (errors.end (), msgs.begin (), msgs.end ());
	    } FINALLY;	    
	    exitForeign ();
	    
	    popReferent ("validateTemplateSymbol");
	    
	    if (errors.size () != 0) {
		THROW (ErrorCode::EXTERNAL, errors);
	    }
	}

	
	Generator Visitor::validateSet (const syntax::Set & set) {
	    std::vector <Generator> values;
	    Generator type (Void::init (set.getLocation ()));
	    bool breaker = false, returner = false;

	    std::vector <std::string> errors;
	    for (int i = 0 ; i < (int) set.getContent ().size () ; i ++) {
		TRY (
		    if (returner || breaker) {			
			Error::occur (set.getContent () [i].getLocation (), ExternalError::get (UNREACHBLE_STATEMENT));
		    }
		    
		    auto value = validateValue (set.getContent () [i]);
		    
		    if (value.to <Value> ().isReturner ()) returner = true;
		    if (value.to <Value> ().isBreaker ()) breaker = true;
		    type = value.to <Value> ().getType ();
		    
		    values.push_back (value);		    
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());		    
		} FINALLY;
	    }

	    if (errors.size () != 0) {
		THROW (ErrorCode::EXTERNAL, errors);
	    }

	    return Set::init (set.getLocation (), type, values);
	}
	
	Generator Visitor::validateFixed (const syntax::Fixed & fixed, int base) {
	    struct Anonymous {

		static std::string removeUnder (const std::string & value) {
		    auto aux = value;
		    aux.erase (std::remove (aux.begin (), aux.end (), '_'), aux.end ());
		    return aux;
		}
		
		static ulong convertU (const lexing::Word & loc, const Integer & type, int base) { 
		    char * temp = nullptr; errno = 0; // errno !!
		    auto val = removeUnder (loc.str);
		    if (val.length () > 2 && val [0] == '0' && val [1] == Keys::LX[0]) {
			val = val.substr (2, val.length () - 1);
			base = 16;
		    } else if (val.length () > 2 && val [0] == '0' && val [1] == 'o') {
			val = val.substr (2, val.length () - 1);
			base = 8;
		    }
		    
		    ulong value = std::strtoul (val.c_str (), &temp, base);
		    bool overflow = false;
		    if (temp == val.c_str () || *temp != '\0' ||
			((value == 0 || value == ULONG_MAX) && errno == ERANGE)) {
			overflow = true;
		    }
		    
		    if (overflow || (value > getMaxU (type) && getMaxU (type) != 0))
			Error::occur (loc, ExternalError::get (OVERFLOW), type.getTypeName (), val);
		    
		    return value;
		}
		
		static long convertS (const lexing::Word & loc, const Integer & type, int base) {
		    char * temp = nullptr; errno = 0; // errno !!
		    auto val = removeUnder (loc.str);
		    if (val.length () > 2 && val [0] == '0' && val [1] == Keys::LX[0]) {
			val = val.substr (2, val.length () - 1);
			base = 16;
		    } else if (val.length () > 2 && val [0] == '0' && val [1] == 'o') {
			val = val.substr (2, val.length () - 1);
			base = 8;
		    }

		    ulong value = std::strtol (val.c_str (), &temp, base);
		    bool overflow = false;
		    if (temp == val.c_str () || *temp != '\0' ||
			((value == 0 || value == ULONG_MAX) && errno == ERANGE)) {
			overflow = true;
		    }
		    
		    if (overflow || (value > getMaxS (type) && getMaxS (type) != 0))
			Error::occur (loc, ExternalError::get (OVERFLOW), type.getTypeName (), val);
		    
		    return value;
		}

		static ulong getMaxU (const Integer & type) {
		    switch (type.getSize ()) {
		    case 8 : return UCHAR_MAX;
		    case 16 : return USHRT_MAX;
		    case 32 : return UINT_MAX;
		    case 64 : return ULONG_MAX;
		    case 0 : return 0;
		    }
		    Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");
		    return 0;
		}
		
		static ulong getMaxS (const Integer & type) {
		    switch (type.getSize ()) {
		    case 8 : return SCHAR_MAX;
		    case 16 : return SHRT_MAX;
		    case 32 : return INT_MAX;
		    case 64 : return LONG_MAX;
		    case 0 : return 0;
		    }
		    Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");
		    return 0;
		}
		
	    };

	    Generator type (Generator::empty ());
	    if (fixed.getSuffix () == Keys::U8) type = Integer::init (fixed.getLocation (), 8, false);
	    if (fixed.getSuffix () == Keys::U16) type = Integer::init (fixed.getLocation (), 16, false);
	    if (fixed.getSuffix () == Keys::U32) type = Integer::init (fixed.getLocation (), 32, false);
	    if (fixed.getSuffix () == Keys::U64) type = Integer::init (fixed.getLocation (), 64, false);
	    if (fixed.getSuffix () == Keys::USIZE) type = Integer::init (fixed.getLocation (), 0, false);
	    
	    if (fixed.getSuffix () == Keys::I8) type = Integer::init (fixed.getLocation (), 8, true);
	    if (fixed.getSuffix () == Keys::I16) type = Integer::init (fixed.getLocation (), 16, true);
	    if (fixed.getSuffix () == "") type = Integer::init (fixed.getLocation (), 32, true);
	    if (fixed.getSuffix () == Keys::I64) type = Integer::init (fixed.getLocation (), 64, true);
	    if (fixed.getSuffix () == Keys::ISIZE) type = Integer::init (fixed.getLocation (), 0, true);

	    auto integer = type.to<Integer> ();
	    //type.to <Type> ().isMutable (true);
	    Fixed::UI value;
	    
	    if (integer.isSigned ()) value.i = Anonymous::convertS (fixed.getLocation (), integer, base);
	    else value.u = Anonymous::convertU (fixed.getLocation (), integer, base);	    
	    
	    return Fixed::init (fixed.getLocation (), type, value);
	}       
	
	Generator Visitor::validateBool (const syntax::Bool & b) {
	    return BoolValue::init (b.getLocation (), Bool::init (b.getLocation ()), b.getLocation () == Keys::TRUE_);
	}

	Generator Visitor::validateFloat (const syntax::Float & f) {
	    Generator type (Generator::empty ());
	    if (f.getSuffix () == Keys::FLOAT_S) type = Float::init (f.getLocation (), 32);
	    else {
		type = Float::init (f.getLocation (), 64);
	    }
	    
	    return FloatValue::init (f.getLocation (), type, f.getValue ());
	}

	Generator Visitor::validateChar (const syntax::Char & c) {	  	    
	    Generator type (Generator::empty ());	    
	    if (c.getSuffix () == Keys::C8) type = Char::init (c.getLocation (), 8);
	    else type = Char::init (c.getLocation (), 32);

	    auto visitor = UtfVisitor::init (*this);
	    
	    uint value = visitor.convertChar (c.getLocation (), c.getSequence (), type.to<Char> ().getSize ());	   
	    return CharValue::init (c.getLocation (), type, value);
	}
	
	Generator Visitor::validateBinary (const syntax::Binary & bin, bool isFromCall) {
	    if (bin.getLocation () == Token::DCOLON) {
		auto subVisitor = SubVisitor::init (*this);
		return subVisitor.validate (bin);
	    } else if (bin.getLocation () == Token::DOT) {
		auto dotVisitor = DotVisitor::init (*this);
		return dotVisitor.validate (bin, isFromCall);
	    } else {
		auto binVisitor = BinaryVisitor::init (*this);
		return binVisitor.validate (bin);
	    }
	}

	Generator Visitor::validateUnary (const syntax::Unary & un) {
	    auto unVisitor = UnaryVisitor::init (*this);
	    return unVisitor.validate (un);
	}
	
	Generator Visitor::validateVar (const syntax::Var & var) {
	    auto gen = getLocal (var.getName ().str);
	    if (gen.isEmpty ()) {
		auto sym = getGlobal (var.getName ().str);
		if (sym.empty ()) {
		    sym = getGlobalPrivate (var.getName ().str);
		    std::string note;
		    for (auto it : Ymir::r (0, sym.size ())) {
			if (it != 0) note = note + "\n";
			note = note + Ymir::Error::createNoteOneLine (ExternalError::get (PRIVATE_IN_THIS_CONTEXT), sym[it].getName (), sym [it].getRealName ());
		    }
		    Error::occurAndNote (var.getLocation (), note, ExternalError::get (UNDEF_VAR), var.getName ().str);
		}
		
		return validateMultSym (var.getLocation (), sym);
	    }

	    // The gen that we got can be either a param decl or a var decl
	    if (gen.is <ParamVar> ()) {
		return VarRef::init (var.getLocation (), var.getName ().str, gen.to<Value> ().getType (), gen.getUniqId (), gen.to<ParamVar> ().isMutable (), Generator::empty ());
	    } else if (gen.is <generator::VarDecl> ()) {
		Generator value (Generator::empty ());
		if (!gen.to <generator::VarDecl> ().isMutable ())
		    value = gen.to <generator::VarDecl> ().getVarValue ();
		return VarRef::init (var.getLocation (), var.getName ().str, gen.to<generator::VarDecl> ().getVarType (), gen.getUniqId (), gen.to<generator::VarDecl> ().isMutable (), value);		
	    } else if (gen.is <StructAccess> ()) {// Closure
		gen.changeLocation (var.getLocation ());
		return gen;
	    } else if (gen.is <ProtoVar> ()) { // PrototypeForProto validation
		return VarRef::init (var.getLocation (), var.getName ().str, gen.to <Value> ().getType (), gen.getUniqId (), gen.to <ProtoVar> ().isMutable (), Generator::empty ());
	    }

	    Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");
	    return Generator::empty ();
	}
	
	Generator Visitor::validateMultSym (const lexing::Word & loc, const std::vector <Symbol> & multSym) {	    
	    std::vector <Generator> gens;
	    for (auto & sym : multSym) {
		pushReferent (sym, "validateMultSym");
		volatile bool succ = false;
		std::vector <std::string> errors;
		TRY (
		    match (sym) {
			of (semantic::Function, func, {
				if (!func.isMethod ()) {
				    gens.push_back (validateFunctionProto (func));			    
				}
				succ = true;
			    }
			) else of (semantic::Constructor, func ATTRIBUTE_UNUSED, {
				gens.push_back (validateConstructorProto (sym));		    
				succ = true;
			    }
			) else of (semantic::ModRef, r ATTRIBUTE_UNUSED, {
				gens.push_back (ModuleAccess::init (loc, sym));
				succ = true;
			    }		    		    
			) else of (semantic::Module, mod ATTRIBUTE_UNUSED, {
				gens.push_back (ModuleAccess::init (loc, sym));
				succ = true;
			    }
			) else of (semantic::Struct, st ATTRIBUTE_UNUSED, {
				auto str_ref = validateStruct (sym);
				gens.push_back (str_ref.to <StructRef> ().getRef ().to <semantic::Struct> ().getGenerator ());
				succ = true;
			    }

			) else of (semantic::Class, cl ATTRIBUTE_UNUSED, {
				auto cl_ref = validateClass (sym);
				if (cl_ref.is<ClassRef> ())
				    gens.push_back (cl_ref.to <ClassRef> ().getRef ().to <semantic::Class> ().getGenerator ());
				else gens.push_back (cl_ref);
				succ = true;
			    }
			) else of (semantic::Trait, tr ATTRIBUTE_UNUSED, {
				gens.push_back (TraitRef::init ({loc, tr.getName ().str}, sym));
				succ = true;
			    }
			) else of (semantic::Enum, en ATTRIBUTE_UNUSED, {
				auto en_ref = validateEnum (sym);
				gens.push_back (en_ref.to <Type> ().getProxy ().to <EnumRef> ().getRef ().to <semantic::Enum> ().getGenerator ());
				succ = true;
			    }
			) else of (semantic::Template, tmp ATTRIBUTE_UNUSED, {
				gens.push_back (TemplateRef::init (sym.getName (), sym));
				succ = true;
			    }
			) else of (semantic::TemplateSolution, sol, {			    
				auto loc_gens = validateMultSym (loc, sol.getAllLocal ());
				match (loc_gens) {
				    of (MultSym, mlt_sym, {
					    gens.insert (gens.end (), mlt_sym.getGenerators ().begin (), mlt_sym.getGenerators ().end ());
					}) else {
					gens.push_back (loc_gens);
				    }
				}
				succ = true;
			    }
			) else of (semantic::VarDecl, decl, {
				validateVarDecl (sym);
				auto gen = decl.getGenerator ().to <GlobalVar> ();
				Generator value (Generator::empty ());
				if (!gen.isMutable ())
				    value = gen.getValue ();
				gens.push_back (VarRef::init (decl.getName (), decl.getName ().str, gen.getType (), gen.getUniqId (), gen.isMutable (), value));
				succ = true;
			    }
			) else of (semantic::Alias, al ATTRIBUTE_UNUSED, {
				auto al_ref = validateAlias (sym);
				gens.push_back (al_ref);
				succ = true;
			    }
			);
		    }
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors = msgs;
		} FINALLY;		
		
		popReferent ("validateMultSym");
		if (errors.size () != 0)
		    THROW (ErrorCode::EXTERNAL, errors);
		
		if (!succ) {
		    println (sym.formatTree ());
		    Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");
		}
	    }

	    if (gens.size () == 1) return gens [0];
	    else return MultSym::init (loc, gens);
	}

	Generator Visitor::validateMultSymType (const lexing::Word & loc, const std::vector <Symbol> & multSym) {
	    Generator gen (Generator::empty ());
	    std::vector <std::string> errors;

	    for (auto  it : Ymir::r (0, multSym.size ())) {
		pushReferent (multSym [it], "validateMultSymType");
		Generator locGen (Generator::empty ());
		TRY (
		    match (multSym [it]) {		    
			of (semantic::Struct, st ATTRIBUTE_UNUSED, {
				locGen = validateStruct (multSym [it]);
			    }
			) else of (semantic::Enum, en ATTRIBUTE_UNUSED, {
				locGen = validateEnum (multSym [it]);
			    }
			) else of (semantic::Class, cl ATTRIBUTE_UNUSED, {
				locGen = validateClass (multSym [it]);
			    }		
			) else of (semantic::Trait, tr ATTRIBUTE_UNUSED, {
				locGen = TraitRef::init ({loc, tr.getName ().str}, multSym [it]);
			    }			
			) else of (semantic::Template, tmp ATTRIBUTE_UNUSED, {
				Ymir::Error::occur (loc, ExternalError::get (USE_AS_TYPE));
			    }							     
			) else of (semantic::Module, mod ATTRIBUTE_UNUSED, {
				Ymir::Error::occur (loc, ExternalError::get (USE_AS_TYPE));
			    }
			) else of (semantic::ModRef, mod ATTRIBUTE_UNUSED, {
				Ymir::Error::occur (loc, ExternalError::get (USE_AS_TYPE));
			    }
			) else of (semantic::Alias, al ATTRIBUTE_UNUSED, {
				locGen = validateAlias (multSym [0]);
			    }
			) else of (semantic::Function, func, {
				locGen = validateFunctionProto (func);
			    }
			) else {
			        Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");				
			}
		    }
		    		    		    
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors = msgs;
		} FINALLY;

		popReferent ("validateMultSymType");
		
		if (!gen.isEmpty () && !locGen.isEmpty ()) {
		    auto note = Ymir::Error::createNoteOneLine (ExternalError::get (CANDIDATE_ARE), gen.getLocation (), gen.prettyString ()) + "\n";
		    note = note + Ymir::Error::createNoteOneLine (ExternalError::get (CANDIDATE_ARE), locGen.getLocation (), locGen.prettyString ());
			
		    Ymir::Error::occurAndNote (loc,
					       note,
					       ExternalError::get (SPECIALISATION_WORK_TYPE_BOTH));
		}
		
		if (!locGen.isEmpty ())
		    gen = locGen;
	    }
	    
	    if (errors.size () != 0 && gen.isEmpty ())
		THROW (ErrorCode::EXTERNAL, errors);
	    
	    return gen;
	}
	
	Generator Visitor::validateFunctionProto (const semantic::Function & func) {
	    enterForeign ();
	    std::vector <Generator> params;
	    static std::list <lexing::Word> __validating__; 
	    auto & function = func.getContent ();
	    std::vector <std::string> errors;
	    bool no_value = false;
	    Generator retType (Generator::empty ());
	    for (auto func_loc : __validating__) {
		// If there is a foward reference, we can't validate the values
		if (func_loc.isSame (func.getName ())) no_value = true;		    
	    }

	    __validating__.push_back (func.getName ());
	    enterBlock ();
	    
	    TRY (		
		validatePrototypeForProto (func.getName (), function.getPrototype (), no_value, params, retType);
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors = msgs;
	    } FINALLY;

	    {
		TRY (
		    this-> discardAllLocals ();
		    this-> quitBlock ();
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }
	    
	    __validating__.pop_back ();
	    exitForeign ();

	    if (errors.size () != 0) {
		THROW (ErrorCode::EXTERNAL, errors);		
	    }


	    auto frame = FrameProto::init (function.getName (), func.getRealName (), retType, params, func.isVariadic ());
	    auto ln = func.getExternalLanguage ();
	    if (ln == Keys::CLANG) 
		frame.to <FrameProto> ().setManglingStyle (Frame::ManglingStyle::C);
	    else if (ln == Keys::CPPLANG)
		frame.to <FrameProto> ().setManglingStyle (Frame::ManglingStyle::CXX);
	    frame.to <FrameProto> ().setMangledName (func.getMangledName ());
	    return frame;
	}

	Generator Visitor::validateConstructorProto (const semantic::Symbol & sym) {
	    auto & func = sym.to <Constructor> ();
	    pushReferent (sym, "validateConstructorProto");
	    enterForeign ();
	    
	    std::vector <Generator> params;
	    static std::list <lexing::Word> __validating__;
	    auto & function = func.getContent ();
	    std::vector <std::string> errors;
	    bool no_value = false;
	    Generator retType (Generator::empty ());
	    
	    for (auto func_loc : __validating__) {
		if (func_loc.isSame (func.getName ())) no_value = true;
	    }

	    Generator cl (Generator::empty ());
	    TRY (
		cl = validateClass (func.getClass ());
		cl.to <Type> ().isMutable (true);
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors = msgs;
	    } FINALLY;
	    
	    if (!cl.isEmpty ()) {		
		__validating__.push_back (func.getName ());
		enterBlock ();
		this-> insertLocal (Keys::SELF, ProtoVar::init (func.getName (), cl, Generator::empty (), true));
		TRY (		
		    validatePrototypeForProto (func.getName (), function.getPrototype (), no_value, params, retType);
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors = msgs;
		} FINALLY;

		{
		    TRY (
			this-> discardAllLocals ();
			this-> quitBlock ();
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;
		}

		__validating__.pop_back ();
	    }
	    
	    exitForeign ();
	    popReferent ("validateConstructorProto");
	    	    
	    if (errors.size () != 0) {
		THROW (ErrorCode::EXTERNAL, errors);		
	    }
	    	    
	    auto frame = ConstructorProto::init (func.getName (), func.getRealName (), sym, cl, params);
	    frame.to <ConstructorProto> ().setMangledName (func.getMangledName ());	    
	    return frame;
	}

	Generator Visitor::validateMethodProto (const semantic::Function & func, const Generator & classType) {
	    enterForeign ();
	    std::vector <Generator> params;
	    static std::list <lexing::Word> __validating__;
	    auto & function = func.getContent ();
	    std::vector <std::string> errors;
	    bool no_value = false;
	    Generator retType (Generator::empty ());
	    for (auto func_loc : __validating__) {
		if (func_loc.isSame (func.getName ())) no_value = true;
	    }

	    __validating__.push_back (func.getName ());
	    enterBlock ();
	    this-> insertLocal (Keys::SELF, ProtoVar::init (func.getName (), classType, Generator::empty (), true));

	    TRY (
		auto & __params = function.getPrototype ().getParameters ();
		auto fakeParams = std::vector <syntax::Expression> (__params.begin () + 1, __params.end ());
		auto proto = syntax::Function::Prototype::init (fakeParams, function.getPrototype ().getType (), false);
		validatePrototypeForProto (func.getName (), proto, no_value, params, retType);
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors = msgs;
	    } FINALLY;

	    {
		TRY (
		    this-> discardAllLocals ();
		    this-> quitBlock ();
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }
	    
	    __validating__.pop_back ();
	    exitForeign ();

	    if (errors.size () != 0) {
		THROW (ErrorCode::EXTERNAL, errors);		
	    }
	    
	    auto frame = MethodProto::init (function.getName (), func.getRealName (), retType, params, false,
					    classType,
					    function.getPrototype ().getParameters ()[0].to <syntax::VarDecl> ().hasDecorator (syntax::Decorator::MUT), function.getBody ().getBody ().isEmpty (), func.isFinal ());
	    frame.to <MethodProto> ().setMangledName (func.getMangledName ());
	    return frame;
	}

	

	void Visitor::validatePrototypeForFrame (const lexing::Word &, const syntax::Function::Prototype & proto,  std::vector <Generator> & params, generator::Generator & retType) {
	    std::vector <std::string> errors;
	    {
		TRY (
		    for (auto & param : proto.getParameters ()) {
			auto var = param.to <syntax::VarDecl> ();
			Generator type (Generator::empty ());
			Generator value (Generator::empty ());
			if (!var.getType ().isEmpty ()) {
			    type = validateType (var.getType ());
			}
		
			if (!var.getValue ().isEmpty ()) {
			    value = validateValue (var.getValue ());
			    if (!type.isEmpty ())
				verifySameType (type, value.to <Value> ().getType ());
			    else {
				type = value.to <Value> ().getType ();
				type.to <Type> ().isMutable (false);
			    }
			}
		
			bool isMutable = false;
			bool isRef = false;
			applyDecoratorOnVarDeclType (var.getDecorators (), type, isRef, isMutable);
			verifyMutabilityRefParam (var.getLocation (), type, MUTABLE_CONST_PARAM);
			
			if (!value.isEmpty ()) {		    
			    verifyMemoryOwner (value.getLocation (), type, value, true);
			}

			if (type.is <NoneType> () || type.is<Void> ()) {
			    Ymir::Error::occur (var.getLocation (), ExternalError::get (VOID_VAR));
			} else if (type.is <generator::LambdaType> ()) {
			    Ymir::Error::occur (type.getLocation (), ExternalError::get (INCOMPLETE_TYPE), type.prettyString ());
			}
		
			params.push_back (ParamVar::init (var.getName (), type, isMutable));
			if (var.getName () != Keys::UNDER) {
			    verifyShadow (var.getName ());		
			    insertLocal (var.getName ().str, params.back ());
			}
		    }
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }
	    {

		TRY (
		    if (!proto.getType ().isEmpty ())
			retType = validateType (proto.getType (), true);
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }

	    if (errors.size () != 0) {
		THROW (ErrorCode::EXTERNAL, errors);		
	    }
	    
	}	
	
	void Visitor::validatePrototypeForProto (const lexing::Word & loc, const syntax::Function::Prototype & proto, bool no_value, std::vector <Generator> & params, generator::Generator & retType) {
	    std::vector <std::string> errors;
	    for (auto & param : proto.getParameters ()) {
		TRY (
		    auto var = param.to <syntax::VarDecl> ();
		    Generator type (Generator::empty ()); // C++ macros are a mystery to me.
		    // You just can't declare two vars in a row !!

		    Generator value (Generator::empty ());
		    
		    if (!var.getType ().isEmpty ()) {
		    	type = validateType (var.getType ());
		    }
		    		
		    if (!var.getValue ().isEmpty () && !no_value) {
			value = validateValue (var.getValue ());
			if (!type.isEmpty ()) 
			    verifyCompatibleType (param.getLocation (), type, value.to <Value> ().getType ());
		    	else {
		    	    type = value.to <Value> ().getType ();
		    	}
		    }

		    if (var.getType ().isEmpty () && no_value && !var.getValue ().isEmpty ()) {
			Ymir::Error::occur (var.getLocation (), ExternalError::get (FORWARD_REFERENCE_VAR));
		    }
				
		    bool isMutable = false;
		    bool isRef = false;
		    applyDecoratorOnVarDeclType (var.getDecorators (), type, isRef, isMutable);
		    verifyMutabilityRefParam (var.getLocation (), type, MUTABLE_CONST_PARAM);
		
		    if (!value.isEmpty ()) {		    
		    	verifyMemoryOwner (value.getLocation (), type, value, true);
		    }

		    if (type.is <NoneType> () || type.is<Void> ()) {
			Ymir::Error::occur (var.getLocation (), ExternalError::get (VOID_VAR));
		    }
		    
		    if (type.is <generator::LambdaType> ()) {
			Ymir::Error::occur (type.getLocation (), ExternalError::get (INCOMPLETE_TYPE), type.prettyString ());
		    }

		    
		    params.push_back (ProtoVar::init (var.getName (), type, value, isMutable));
		    if (var.getName () != Keys::UNDER) {
			verifyShadow (var.getName ());		
			insertLocal (var.getName ().str, params.back ());
		    }
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    errors.push_back (Ymir::Error::createNote (param.getLocation ()));
		} FINALLY;
	    }
	    
	    {
		TRY (
		    if (!proto.getType ().isEmpty ())
			retType = validateType (proto.getType (), true);
		    else retType = Void::init (loc);
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }

	    
	    if (errors.size () != 0) {
		THROW (ErrorCode::EXTERNAL, errors);		
	    }

	}
	
	Generator Visitor::validateVarDeclValue (const syntax::VarDecl & var, bool needInitValue) {
	    if (var.getName () != Keys::UNDER)
		verifyShadow (var.getName ());

	    Generator value (Generator::empty ());
	    if (!var.getValue ().isEmpty ()) {
		value = validateValue (var.getValue ());
	    }

	    if (var.getValue ().isEmpty () && var.getType ().isEmpty ()) {
		Error::occur (var.getLocation (), ExternalError::get (VAR_DECL_WITH_NOTHING));
	    }

	    Generator type (Generator::empty ());
	    if (!var.getType ().isEmpty ()) {
		type = validateType (var.getType ());
	    } else {
		type = value.to <Value> ().getType ();
		type.to<Type> ().isRef (false);
		type.to <Type> ().isMutable (false);
	    }

	    
	    if (var.getValue ().isEmpty () && needInitValue) {
		Error::occur (var.getLocation (), ExternalError::get (VAR_DECL_WITHOUT_VALUE));
	    } 
		    
	    bool isMutable = false, isRef = false;
	    applyDecoratorOnVarDeclType (var.getDecorators (), type, isRef, isMutable);
	    
	    type.to <Type> ().isLocal (true);
	    if (!value.isEmpty ()) {
		// We do not check the lambdatype complete type if the var is not mutable
		if (isMutable || !type.is <LambdaType> ())
		    verifyMemoryOwner (var.getLocation (), type, value, true);
	    }

	    if (type.is<NoneType> () || type.is<Void> ()) {
		Ymir::Error::occur (var.getLocation (), ExternalError::get (VOID_VAR));
	    }
	    
	    
	    auto ret = generator::VarDecl::init (var.getLocation (), var.getName ().str, type, value, isMutable);
	    if (var.getName () != Keys::UNDER)
		insertLocal (var.getName ().str, ret);
	    return ret;
	}
	
	Generator Visitor::validateDecoratedExpression (const syntax::DecoratedExpression & dec_expr) {
	    Generator inner = Generator::empty ();
	    if (dec_expr.hasDecorator (syntax::Decorator::CTE)) {
		inner = validateCteValue (dec_expr.getContent ());
	    } else {
		inner = validateValue (dec_expr.getContent ());
	    }
	    
	    if (dec_expr.hasDecorator (syntax::Decorator::MUT)) {
		if (!inner.to<Value> ().getType ().to<Type> ().isMutable ()) 
		    Ymir::Error::occur (dec_expr.getDecorator (syntax::Decorator::MUT).getLocation (),
					ExternalError::get (DISCARD_CONST)
		    );
		else
		    Ymir::Error::warn (dec_expr.getDecorator (syntax::Decorator::MUT).getLocation (),
				       ExternalError::get (USELESS_DECORATOR)
		    );
	    } 

	    if (dec_expr.hasDecorator (syntax::Decorator::DMUT)) {
		if (!inner.to<Value> ().getType ().to<Type> ().isMutable ()) 
		    Ymir::Error::occur (dec_expr.getDecorator (syntax::Decorator::DMUT).getLocation (),
					ExternalError::get (DISCARD_CONST)
		    );
		else
		    Ymir::Error::warn (dec_expr.getDecorator (syntax::Decorator::DMUT).getLocation (),
				       ExternalError::get (USELESS_DECORATOR)
		    );
	    } 
	    
	    if (dec_expr.hasDecorator (syntax::Decorator::REF)) {
		if (inner.to <Value> ().isLvalue ()) {
		    // if (!inner.to <Value> ().getType ().to <Type> ().isMutable ()) {
		    // 	Ymir::Error::occur (inner.getLocation (), ExternalError::get (IMMUTABLE_LVALUE));
		    // } // We allow this, since we want to pass element by const reference to function, or variable
		    // The mutability will verify if we are allowed to do a reference of the element

		    if (!inner.is<Referencer> ()) {
			auto type = inner.to <Value> ().getType ();
			type.to <Type> ().isRef (true);
			inner = Referencer::init (dec_expr.getLocation (), type, inner);
		    } 
		} else {
		    Ymir::Error::occur (inner.getLocation (),
					ExternalError::get (NOT_A_LVALUE)
		    );
		}
	    }

	    if (dec_expr.hasDecorator (syntax::Decorator::CONST)) {
		auto type = inner.to<Value> ().getType ();
		type.to <Type> ().isMutable (false);
		inner.to<Value> ().setType (type);
	    }

	    return inner;
	}

	Generator Visitor::validateIfExpression (const syntax::If & _if) {
	    Generator test (Generator::empty ());
	    if (!_if.getTest ().isEmpty ()) { // Has a test if it is not an else
		test = validateValue (_if.getTest ());
		
		if (!test.to<Value> ().getType ().is <Bool> ()) {
		    Ymir::Error::occur (test.getLocation (), ExternalError::get (INCOMPATIBLE_TYPES),
					test.to <Value> ().getType ().to <Type> ().getTypeName (),
					Bool::NAME
		    );
		}
	    }

	    auto content = validateValueNoReachable (_if.getContent ());
	    auto type = content.to <Value> ().getType ();

	    if (!_if.getElsePart ().isEmpty ()) {
		auto _else = validateValueNoReachable (_if.getElsePart ());
		verifyCompatibleType (_if.getLocation (), _else.to<Value> ().getType (), type);
		return Conditional::init (_if.getLocation (), type, test, content, _else);	    
	    } else {
		verifyCompatibleType (_if.getLocation (), Void::init (_if.getLocation ()), type);
		return Conditional::init (_if.getLocation (), type, test, content, Generator::empty ());
	    }
	}

	
	Generator Visitor::validateCteIfExpression (const syntax::If & _if) {
	    Generator test (Generator::empty ());
	    if (!_if.getTest ().isEmpty ()) {
		auto test = validateValue (_if.getTest ());
		auto value = retreiveValue (test);
		if (!value.is<BoolValue> ()) {
		    Ymir::Error::occur (test.getLocation (), ExternalError::get (INCOMPATIBLE_TYPES),
					test.to <Value> ().getType ().to <Type> ().getTypeName (),
					Bool::NAME
		    );
		    return Generator::empty ();
		}
		if (value.to <BoolValue> ().getValue ()) {
		    return validateValueNoReachable (_if.getContent ());
		} else if (!_if.getElsePart ().isEmpty ()) {
		    return validateValue (_if.getElsePart ());
		} else return Block::init (_if.getLocation (), Void::init (_if.getLocation ()), {});
	    } else return validateValueNoReachable (_if.getContent ());
	}	
	
	Generator Visitor::validateWhileExpression (const syntax::While & _wh) {
	    Generator test (Generator::empty ());
	    if (!_wh.getTest ().isEmpty ()) {
		test = validateValue (_wh.getTest ());
		if (!test.to <Value> ().getType ().is <Bool> ()) {
		    Ymir::Error::occur (test.getLocation (), ExternalError::get (INCOMPATIBLE_TYPES),
					test.to <Value> ().getType ().to <Type> ().getTypeName (),
					Bool::NAME
		    );
		}
	    }

	    std::vector <std::string> errors;
	    enterLoop ();
	    Generator content (Generator::empty ());
	    TRY (
		content = validateValueNoReachable (_wh.getContent ());
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors.insert (errors.end (), msgs.begin (), msgs.end ());
	    } FINALLY;	    
	    
	    auto breakType = quitLoop ();
	    if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);
	    
	    Generator type (Generator::empty ());
	    if (!test.isEmpty ()) {
		type = content.to <Value> ().getType ();

		if (!breakType.isEmpty () && !content.to <Value> ().isBreaker () && !type.equals (breakType)) {
		    auto note = Ymir::Error::createNote (breakType.getLocation ());
		    Ymir::Error::occurAndNote (content.getLocation (), note, ExternalError::get (INCOMPATIBLE_TYPES),
					       type.to <Type> ().getTypeName (),
					       breakType.to <Type> ().getTypeName ()
		    );				    
		} else if (content.to <Value> ().isBreaker ()) {
		    type = breakType;
		}
	    } else {
		if (breakType.isEmpty ()) type = Void::init (_wh.getLocation ());
		else type = breakType;
	    }

	    return Loop::init (_wh.getLocation (), type, test, content, _wh.isDo ());	    
	}	

	Generator Visitor::validateForExpression (const syntax::For & _for) {
	    auto forVisitor = ForVisitor::init (*this);
	    Generator content (Generator::empty ());
	    enterLoop ();
	    std::vector <std::string> errors;
	    TRY (
		content = forVisitor.validate (_for);
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors.insert (errors.end (), msgs.begin (), msgs.end ());
	    } FINALLY;	    
	    
	    auto breakType = quitLoop ();
	    if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);
	    
	    auto type = content.to <Value> ().getType ();
	    if (!breakType.isEmpty () && !content.to <Value> ().isBreaker () && !type.equals (breakType)) {
		auto note = Ymir::Error::createNote (breakType.getLocation ());
		Ymir::Error::occurAndNote (content.getLocation (), note, ExternalError::get (INCOMPATIBLE_TYPES),
					   type.to <Type> ().getTypeName (),
					   breakType.to <Type> ().getTypeName ()
		);				    
	    }
	    
	    return content;
	}
	
	Generator Visitor::validateBreak (const syntax::Break & _break) {
	    if (!this-> isInLoop ())
		Ymir::Error::occur (_break.getLocation (), ExternalError::get (BREAK_NO_LOOP));
				    
	    Generator value = Generator::empty ();
	    Generator type = Generator::empty ();
	    if (!_break.getValue ().isEmpty ()) {
		value = validateValue (_break.getValue ());
		type = value.to <Value> ().getType ();
	    } else type = Void::init (_break.getLocation ());

	    auto loop_type = getCurrentLoopType ();
	    if (loop_type.isEmpty ()) {
		setCurrentLoopType (type);
		loop_type = type;
	    } else if (!loop_type.equals (type)) {		
		auto note = Ymir::Error::createNote (loop_type.getLocation ());
		Ymir::Error::occurAndNote (value.getLocation (), note, ExternalError::get (INCOMPATIBLE_TYPES),
					   type.to <Type> ().getTypeName (),
					   loop_type.to <Type> ().getTypeName ()
		);				    
	    }

	    if (!loop_type.is<Void> ()) verifyMemoryOwner (_break.getLocation (), loop_type, value, false);
	    
	    return Break::init (_break.getLocation (), Void::init (_break.getLocation ()), value);
	}

	Generator Visitor::validateReturn (const syntax::Return & rt) {
	    Generator value = Generator::empty ();
	    Generator type = Generator::empty ();
	    if (!rt.getValue ().isEmpty ()) {
	    	value = validateValue (rt.getValue ());
	    	type = value.to <Value> ().getType ();
	    } else type = Void::init (rt.getLocation ());

	    auto fn_type = getCurrentFuncType ();

	    /** In case of lambda proto, we are able to validate it with the function return type */
	    if (type.is <LambdaType> () && (fn_type.is <FuncPtr> () || fn_type.is <Delegate> ())) {
		std::vector <Generator> paramTypes;
		if (fn_type.is <FuncPtr> ()) paramTypes = fn_type.to <FuncPtr> ().getParamTypes ();
		else paramTypes = fn_type.to <Delegate> ().getInners ()[0].to <FuncPtr> ().getParamTypes ();

		if (value.is <VarRef> ()) {
		    value = validateLambdaProto (value.to <VarRef> ().getValue ().to <LambdaProto> (), paramTypes);
		    type = value.to <Value> ().getType ();
		} else if (value.is <LambdaProto> ()) {
		    value = validateLambdaProto (value.to <LambdaProto> (), paramTypes);
		    type = value.to <Value> ().getType ();
		} // This is working exactly like findParameter for function, maybe we can merge those two blocks
	    }
	    
	    if (fn_type.isEmpty ()) {
		this-> setCurrentFuncType (type);
		fn_type = type;
	    } else
		verifyCompatibleType (type.getLocation (), fn_type, type);
	    
	    // 	if (!fn_type.equals (type)) {
	    // 	auto note = Ymir::Error::createNote (fn_type.getLocation ());
	    // 	Ymir::Error::occurAndNote (value.getLocation (), note, ExternalError::get (INCOMPATIBLE_TYPES),
	    // 				   type.to <Type> ().getTypeName (),
	    // 				   fn_type.to <Type> ().getTypeName ()
	    // 	);				    
	    // }

	    if (!fn_type.is<Void> ()) {
		verifyMemoryOwner (rt.getLocation (), fn_type, value, true);
	    }
	    
	    return Return::init (rt.getLocation (), Void::init (rt.getLocation ()), fn_type, value);
	}
	    
	Generator Visitor::validateList (const syntax::List & list) {
	    if (list.isArray ()) return validateArray (list);
	    if (list.isTuple ()) return validateTuple (list);
	    
	    Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");
	    return Generator::empty ();
	}

	Generator Visitor::validateListTemplate (const TemplateSyntaxList & lst) {
	    std::vector <syntax::Expression> exprs;
	    for (auto & it : lst.getContents ())
		exprs.push_back (TemplateSyntaxWrapper::init (it.getLocation (), it));
	    return validateTuple (syntax::List::init (lst.getLocation (), lst.getLocation (), exprs).to <syntax::List> ());
	}
	
	Generator Visitor::validateString (const syntax::String & str) {
	    Generator inner (Generator::empty ());
	    if (str.getSuffix () == Keys::S8) inner = Char::init (str.getLocation (), 8);
	    else inner = Char::init (str.getLocation (), 32);
	    inner.to <Type> ().isMutable (false);
	    
	    auto visitor = UtfVisitor::init (*this);
	    int len = 0;
	    auto value = visitor.convertString (str.getLocation (), str.getSequence (), inner.to <Char> ().getSize (), len);

	    auto type = Array::init (str.getLocation (), inner, len);
	    type.to <Type> ().isMutable (true);
	    type.to <Type> ().isLocal (false);

	    auto sliceType = Slice::init (str.getLocation (), inner);
	    sliceType.to <Type> ().isMutable (true);
	    sliceType.to <Type> ().isLocal (false);
	    
	    return Aliaser::init (
		str.getLocation (),
		sliceType, 
		StringValue::init (str.getLocation (), type, value, len)
	    );
	}
	
	Generator Visitor::validateArray (const syntax::List & list) {
	    std::vector <Generator> params;
	    for (auto it : list.getParameters ()) {
		auto val = validateValue (it);
		if (val.is<List> ()) {
		    for (auto & g_it : val.to <List> ().getParameters ()) {
			params.push_back (g_it);
			verifyMemoryOwner (params.back ().getLocation (), params [0].to <Value> ().getType (), params.back (), false);
		    }
		} else {
		    params.push_back (val);
		    verifyMemoryOwner (params.back ().getLocation (), params [0].to <Value> ().getType (), params.back (), false);
		}
	    }

	    Generator innerType (Void::init (list.getLocation ()));
	    if (params.size () != 0)
		innerType = params [0].to <Value> ().getType ();	    
	    innerType.to <Type> ().isRef (false);
	    
	    // An array literal is always static
	    auto type = Array::init (list.getLocation (), innerType, params.size ());
	    type.to <Type> ().isMutable (true); // Array constant are mutable by default (not lvalue), to ease simple affectation
	    type.to <Type> ().isLocal (true); // Array constant are declared in the stack, so they are local

	    innerType.to <Type> ().isMutable (true);
	    auto slc = Slice::init (list.getLocation (), innerType);
	    slc.to <Type> ().isMutable (true);

	    if (!innerType.is <Void> ()) {
		return Copier::init (list.getLocation (),
				     slc,
				     Aliaser::init (list.getLocation (), slc,
						    ArrayValue::init (list.getLocation (), type.to <Type> ().toDeeplyMutable (), params)
				     )
		);
	    } else
		return
		    Aliaser::init (list.getLocation (), slc,
				   ArrayValue::init (list.getLocation (), type.to <Type> ().toDeeplyMutable (), params)
		    );
	}	
	
	Generator Visitor::validateTuple (const syntax::List & list) {
	    std::vector <Generator> params;
	    std::vector <Generator> types;
	    for (auto it : list.getParameters ()) {
		auto val = validateValue (it);
		if (val.is <List> ()) {
		    for (auto & g_it : val.to<List> ().getParameters ()) {
			params.push_back (g_it);
			auto type = params.back ().to <Value> ().getType ();
			type.to <Type> ().isRef (false);
			types.push_back (type);
			verifyMemoryOwner (params.back ().getLocation (), type, params.back (), false);
		    }
		} else {
		    params.push_back (val);
		    auto type = params.back ().to <Value> ().getType ();
		    type.to <Type> ().isRef (false);
		    types.push_back (type);
		    verifyMemoryOwner (params.back ().getLocation (), type, params.back (), false);
		}
	    }
	    
	    auto type = Tuple::init (list.getLocation (), types);
	    type.to <Type> ().isMutable (true); // Tuple are mutable by default (not lvalue)
	    type.to <Type> ().isLocal (true); //
	    return TupleValue::init (list.getLocation (), type, params);	    
	}

	Generator Visitor::validateTemplateChecker (const syntax::TemplateChecker & check) {
	    std::vector<Generator> params;
	    std::vector <std::string> errors;
	    for (auto & it : check.getCalls ()) {
		bool succeed = true;
		TRY (
		    auto val = validateType (it, true);
		    params.push_back (val);
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    succeed = false;
		} FINALLY;
		
		if (!succeed) {
		    auto val = validateValue (it);
		    auto rvalue = retreiveValue (val);
		    params.push_back (rvalue);
		}
	    }
	    

	    volatile bool succeed = false; // volatile again, due to longjmp
	    
	    TRY (
		auto visitor = TemplateVisitor::init (*this);
		auto mapper = visitor.validateFromExplicit (check.getParameters (), params);
		succeed = mapper.succeed;
	    ) CATCH (ErrorCode::EXTERNAL) {
		GET_ERRORS_AND_CLEAR (msgs);
		errors.insert (errors.end (), msgs.begin (), msgs.end ());
	    } FINALLY;

	    return BoolValue::init (check.getLocation (), Bool::init (check.getLocation ()), succeed);
	}

	Generator Visitor::validateThrow (const syntax::Throw & thr) {
	    auto inner = this-> validateValue (thr.getValue ());
	    auto type = inner.to <Value> ().getType ();
	    auto simpleType = inner.to<Value> ().getType ();
	    simpleType.to <Type> ().isRef (false);
	    simpleType.to <Type> ().isMutable (false);
	    
	    type.to <Type> ().isRef (true);
	    auto value = Copier::init (thr.getLocation (), type, inner, true);
	    Generator info (Generator::empty ());
	    if (type.is <ClassRef> () && !type.to <ClassRef> ().getRef ().to <semantic::Class> ().getTypeInfo ().isEmpty ()) {
		auto loc = thr.getLocation ();
		auto bin = syntax::Binary::init ({loc, Token::DCOLON},
						 TemplateSyntaxWrapper::init (loc, inner),
						 syntax::Var::init ({loc, SubVisitor::__TYPEINFO__}),
						 syntax::Expression::empty ()
		);
		info = validateValue (bin);
	    } else 
		info = validateTypeInfo (thr.getLocation (), simpleType);
	    return Throw::init (thr.getLocation (), info, value);
	}

	Generator Visitor::validateMatch (const syntax::Match & matcher) {
	    auto visitor = MatchVisitor::init (*this);
	    return visitor.validate (matcher);
	}

	Generator Visitor::validateCatchOutOfScope (const syntax::Catch & cat) {
	    Ymir::Error::occur (cat.getLocation (), ExternalError::get (CATCH_OUT_OF_SCOPE));
	    return Generator::empty ();
	}

	Generator Visitor::validateScopeOutOfScope (const syntax::Scope & scope) {
	    Ymir::Error::occur (scope.getLocation (), ExternalError::get (SCOPE_OUT_OF_SCOPE));
	    return Generator::empty ();
	}

	Generator Visitor::validateAssert (const syntax::Assert & assert) {
	    auto test = validateValue (assert.getTest ());
	    if (!test.to <Value> ().getType ().is <Bool> ()) {
		Ymir::Error::occur (test.getLocation (), ExternalError::get (INCOMPATIBLE_TYPES),
				    test.to <Value> ().getType ().to <Type> ().getTypeName (),
				    Bool::NAME
		);
	    }

	    std::vector <syntax::Expression> params;
	    params.push_back (TemplateSyntaxWrapper::init (test.getLocation (), test));
	    if (!assert.getMsg ().isEmpty ()) {
		params.push_back (assert.getMsg ());
	    }

	    auto loc = assert.getLocation ();
	    auto func = createVarFromPath (assert.getLocation (), {CoreNames::get (CORE_MODULE), CoreNames::get (EXCEPTION_MODULE), CoreNames::get (ASSERT_FUNC)});
	    auto call = syntax::MultOperator::init (
		{loc, Token::LPAR}, {loc, Token::RPAR},
		func,
		params
	    );
	    
	    auto ret = validateValue (call);
	    TRY (
	    	auto val = retreiveValue (test);
	    	ret.to <Value> ().isReturner (!val.to <BoolValue> ().getValue ());
	    ) CATCH (ErrorCode::EXTERNAL) {
	    	GET_ERRORS_AND_CLEAR (msgs);
	    } FINALLY;
	    
	    return ret;
	}

	Generator Visitor::validateCteAssert (const syntax::Assert & assert) {
	    auto test = validateValue (assert.getTest ());
	    if (!test.to <Value> ().getType ().is <Bool> ()) {
		Ymir::Error::occur (test.getLocation (), ExternalError::get (INCOMPATIBLE_TYPES),
				    test.to <Value> ().getType ().to <Type> ().getTypeName (),
				    Bool::NAME
		);
	    }

	    auto val = retreiveValue (test);
	    if (!val.to <BoolValue> ().getValue ()) {
		std::string msg;
		if (!assert.getMsg ().isEmpty ())
		    msg = validateValue (assert.getMsg ()).prettyString ();
		Ymir::Error::occur (assert.getLocation (), ExternalError::get (ASSERT_FAILED), msg);
	    }
	    
	    return None::init (assert.getLocation ());
	}

	
	Generator Visitor::validateTypeInfo (const lexing::Word & loc, const Generator & type_) {
	    auto type = type_;
	    type.to <Type> ().isRef (false);
	    type.to <Type> ().isMutable (false);
	    
	    auto typeInfo = createVarFromPath (loc, {CoreNames::get (CORE_MODULE), CoreNames::get (TYPE_INFO_MODULE), CoreNames::get (TYPE_INFO)});
		
	    auto str = validateType (typeInfo);

	    auto typeIDs = createVarFromPath (loc, {CoreNames::get (CORE_MODULE), CoreNames::get (TYPE_INFO_MODULE), CoreNames::get (TYPE_IDS)});
	    auto en_m = validateValue (typeIDs);
	       
	    std::vector <Generator> types = {
		Integer::init (loc, 32, false),
		Integer::init (loc, 0, false),
		Slice::init (loc, str),
		Slice::init (loc, Char::init (loc, 32))
	    };

	    std::vector <Generator> innerTypes;
	    if (!type.is <ClassRef> ()) {
		if (type.to <Type> ().isComplex ()) {
		    for (auto & it : type.to <Type> ().getInners ())
			innerTypes.push_back (validateTypeInfo (loc, it));
		}
	    } else {		
		if (!type.to <ClassRef> ().getAncestor ().isEmpty ()) {
		    innerTypes.push_back (validateTypeInfo (loc, type.to <ClassRef> ().getAncestor ()));
		}
	    }
	    
	    auto arrayType = Array::init (loc, str, innerTypes.size ());
	    auto stringLit = syntax::String::init (loc, loc, {loc, type.prettyString ()}, lexing::Word::eof ());
	    auto name = validateValue (stringLit);
	    auto constName = Mangler::init ().mangle (type) + "_" + "name";
	    auto constNameInner = Mangler::init ().mangle (type) + "_" + "nameInner";
	    auto sliceType = Slice::init (loc, str);
	    auto inner = name.to <Aliaser> ().getWho ();

	    std::vector <Generator> values = {
		en_m.to <generator::Enum> ().getFieldValue (typeInfoName (type)),
		SizeOf::init (loc, Integer::init (loc, 0, false), type),
		Copier::init (loc, sliceType, Aliaser::init (loc, sliceType, ArrayValue::init (loc, arrayType, innerTypes))),	       
		GlobalConstant::init (loc, constName, name.to <Value> ().getType (),
				      Aliaser::init (loc, name.to <Value> ().getType (),
						     GlobalConstant::init (
							 loc, constNameInner, inner.to<Value> ().getType (), inner
						     )
				      )
		)
	    };
	    
	    return StructCst::init (
		loc,
		str,
		str.to <StructRef> ().getRef ().to <semantic::Struct> ().getGenerator ().clone (),
		types,
		values
	    );
	}

	std::string Visitor::typeInfoName (const Generator & type) {
	    // Maybe that's enhanceable
	    match (type) {
		of (Void, bo ATTRIBUTE_UNUSED, return "VOID";);
		of (Array, ar ATTRIBUTE_UNUSED, return "ARRAY";);
		of (Bool,  bo ATTRIBUTE_UNUSED, return "BOOL";);
		of (Char,  ca ATTRIBUTE_UNUSED, return "CHAR";);
		of (Closure,  cl ATTRIBUTE_UNUSED, return "CLOSURE";);
		of (Float,  fl ATTRIBUTE_UNUSED, return "FLOAT";);
		of (FuncPtr,  ptr ATTRIBUTE_UNUSED, return "FUNC_PTR";);
		of (Integer,  i, if (i.isSigned ()) return "SIGNED_INT"; return "UNSIGNED_INT";);
		of (Pointer,  ptr ATTRIBUTE_UNUSED, return "POINTER";);
		of (Slice,  sl ATTRIBUTE_UNUSED, return "SLICE";);
		of (StructRef,  sl ATTRIBUTE_UNUSED, return "STRUCT";);
		of (Tuple,  tu ATTRIBUTE_UNUSED, return "TUPLE";);
		of (ClassRef, cl ATTRIBUTE_UNUSED, return "OBJECT";);
	    }
	    
	    Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");
	    return "";
	}
	
	Generator Visitor::validateTemplateCall (const syntax::TemplateCall & tcl) {
	    auto value = this-> validateValue (tcl.getContent (), false, true);
	    std::vector <std::string> errors;
	    std::vector <Generator> params;
	    for (auto & it : tcl.getParameters ()) {
		bool succeed = true;
		std::vector <std::string> locErrors;
		TRY (
		    params.push_back (validateType (it, true));
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    locErrors.insert (locErrors.begin (), msgs.begin (), msgs.end ());
		    succeed = false;
		} FINALLY;

		if (!succeed) {
		    succeed = true;
		    TRY (
			auto val = validateValue (it);
			auto rvalue = retreiveValue (val);
			params.push_back (rvalue);
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			succeed = false;
		    } FINALLY;		    
		}
		
		if (!succeed)
		    errors.insert (errors.end (), locErrors.begin (), locErrors.end ());
	    }
	    
	    if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);
	    
	    if (value.is <TemplateRef> ()) {
		Generator ret (Generator::empty ());
		TRY (
		    int score = -1;
		    auto templateVisitor = TemplateVisitor::init (*this);
		    auto sym = templateVisitor.validateFromExplicit (value.to <TemplateRef> (), params, score);
		    if (!sym.isEmpty ()) {
			this-> validateTemplateSymbol (sym, value);
			ret = this-> validateMultSym (value.getLocation (), {sym});
		    } 		    
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;

		
		if (errors.size () != 0) {
		    errors.insert (errors.begin (), Ymir::Error::createNoteOneLine (ExternalError::get (CANDIDATE_ARE), value.getLocation (), value.prettyString ()));
		} else return ret;
		
	    } else if (value.is<MultSym> ()) {
		volatile int all_score = -1; // Not volatile, but due to longjmp the compiler prefers it to be
		Symbol final_sym (Symbol::empty ());
		std::map <int, std::vector <Symbol>> loc_scores;
		std::map <int, std::vector <Generator>> loc_elem;
		for (auto & elem : value.to <MultSym> ().getGenerators ()) {
		    if (elem.is<TemplateRef> ()) {
			int local_score = 0;
			Symbol local_sym (Symbol::empty ());
			bool succeed = true;
			TRY (
			    auto templateVisitor = TemplateVisitor::init (*this);
			    local_sym = templateVisitor.validateFromExplicit (elem.to <TemplateRef> (), params, local_score);
			    if (!local_sym.isEmpty ()) {
				loc_scores [local_score].push_back (local_sym);
				loc_elem [local_score].push_back (elem);
			    }
			) CATCH (ErrorCode::EXTERNAL) {
			    GET_ERRORS_AND_CLEAR (msgs);
			    errors.push_back (Ymir::Error::createNoteOneLine (ExternalError::get (CANDIDATE_ARE), elem.getLocation (), elem.prettyString ()));
			    errors.insert (errors.end (), msgs.begin (), msgs.end ());
			    succeed = false;
			} FINALLY;

			if (succeed) {
			    if (local_score > all_score) {
				all_score = local_score;
				final_sym = local_sym;
			    } else if (local_sym.isEmpty ()) {
				errors.push_back (Ymir::Error::createNoteOneLine (ExternalError::get (CANDIDATE_ARE), elem.getLocation (), elem.prettyString ()));
			    }
			}
		    }
		}
		
		if (loc_scores.size () != 0) {
		    Generator ret (Generator::empty ());
		    TRY (
			auto element_on_scores = loc_scores.find ((int) all_score);
			std::vector <Symbol> syms;
			std::vector <Generator> aux;
			for (auto it : Ymir::r (0, element_on_scores-> second.size ())) {
			    this-> validateTemplateSymbol (element_on_scores-> second [it], loc_elem.find ((int) all_score)-> second [it]);
			    if (loc_elem.find ((int) all_score)-> second [it].is <MethodTemplateRef> ()) {
				if (element_on_scores-> second [it].is <TemplateSolution> ()) {
				    auto self = loc_elem.find ((int) all_score)-> second [it].to <MethodTemplateRef> ().getSelf ();
				    auto proto = this-> validateTemplateSolutionMethod (element_on_scores-> second [it], self);
				    std::vector <Generator> types;
				    auto delType = Delegate::init (proto.getLocation (), proto);
				    aux.push_back (				    
					DelegateValue::init (proto.getLocation (),
							     delType,
							     proto.to<MethodProto> ().getClassType (),
							     self,
							     proto
					)
				    );
				} else { // Not finalized template call
				    auto & tmpRef = loc_elem.find ((int) all_score)-> second [it];
				    aux.push_back (
					MethodTemplateRef::init (tmpRef.getLocation (), element_on_scores-> second [it], tmpRef.to <MethodTemplateRef> ().getSelf ())
				    );
				}
			    } else {
				syms.push_back (element_on_scores-> second [it]);
			    }
			}
			
			if (syms.size () != 0) {
			    ret = this-> validateMultSym (value.getLocation (), syms);
			    if (ret.is <MultSym> ()) 
				aux.insert (aux.end (), ret.to <MultSym> ().getGenerators ().begin (), ret.to <MultSym> ().getGenerators ().end ());
			    else aux.push_back (ret);
			}
			
			if (aux.size () == 1)
			    ret = aux [0];
			else ret = MultSym::init (value.getLocation (), aux);
		    ) CATCH (ErrorCode::EXTERNAL) {
			GET_ERRORS_AND_CLEAR (msgs);
			errors.insert (errors.end (), msgs.begin (), msgs.end ());
		    } FINALLY;
		    
		    if (!ret.isEmpty ())
			return ret;
		}
	    }

	    std::vector<std::string> names;
	    for (auto & it : params)
		names.push_back (it.prettyString ());

	    std::string leftName = value.getLocation ().str ;
	    OutBuffer buf;
	    for (auto & it : errors)
		buf.write (it, "\n");

	    
	    Ymir::Error::occurAndNote (
		tcl.getLocation (),
		buf.str (),
		ExternalError::get (UNDEFINED_TEMPLATE_OP),
		leftName,
		names
	    );

	    return Generator::empty ();	    
	}

	Generator Visitor::validateCast (const syntax::Cast & cast) {
	    auto visitor = CastVisitor::init (*this);
	    return visitor.validate (cast);
	}

	Generator Visitor::validateArrayAlloc (const syntax::ArrayAlloc & alloc) {
	    if (alloc.isDynamic ()) {
		auto value = validateValue (alloc.getLeft ());
		verifyMemoryOwner (alloc.getLocation (), value.to <Value> ().getType (), value, false);
		auto size = SizeOf::init (
		    alloc.getLocation (),
		    Integer::init (alloc.getLocation (), 0, false),
		    value.to <Value> ().getType ()
		);
		
		auto len = validateValue (alloc.getSize ());
		auto type = Slice::init (alloc.getLocation (), value.to<Value> ().getType ());
		type.to <Type> ().isMutable (true);
		type.to <Type> ().isLocal (true);
		
		return ArrayAlloc::init (alloc.getLocation (), type.to<Type> ().toDeeplyMutable (), value, size, len);
	    } else {
		auto value = validateValue (alloc.getLeft ());
		verifyMemoryOwner (alloc.getLocation (), value.to <Value> ().getType (), value, false);
		
		auto size = SizeOf::init (
		    alloc.getLocation (),
		    Integer::init (alloc.getLocation (), 0, false),
		    value.to <Value> ().getType ()
		);
		
		auto len = retreiveValue (validateValue (alloc.getSize ()));
		if (!len.is <Fixed> () || (len.to<Fixed> ().getType ().to <Integer> ().isSigned () && len.to <Fixed> ().getUI ().i < 0)) {
		    Ymir::Error::occur (alloc.getSize ().getLocation (), ExternalError::get (INCOMPATIBLE_TYPES),
					value.to <Value> ().getType ().to <Type> ().getTypeName (),
					(Integer::init (lexing::Word::eof (), 64, false)).to<Type> ().getTypeName ()
		    );
		}
		
		auto type = Array::init (alloc.getLocation (), value.to<Value> ().getType (), len.to <Fixed> ().getUI ().u);
		type.to <Type> ().isMutable (true);
		type.to <Type> ().isLocal (true);
		return ArrayAlloc::init (alloc.getLocation (), type.to <Type> ().toDeeplyMutable (), value, size, len.to <Fixed> ().getUI ().u);
	    }
	}

	Generator Visitor::validateDestructDecl (const syntax::DestructDecl & decl) {
	    auto value = validateValue (syntax::Intrinsics::init ({decl.getLocation (), Keys::EXPAND}, decl.getValue ()));
	    match (value) {
		of (List, lst, {
			if ((decl.isVariadic () && lst.getParameters ().size () < decl.getParameters ().size ())
			    || (!decl.isVariadic () && lst.getParameters ().size () != decl.getParameters ().size ())) {
			    Ymir::Error::occur (decl.getLocation (),
						ExternalError::get (MISMATCH_ARITY),
						decl.getParameters ().size (),
						lst.getParameters ().size ());			    
			}

			std::vector <Generator> values;
			Generator type (Void::init (decl.getLocation ()));
			for (int i = 0 ; i < (int) decl.getParameters ().size () ; i++) {
			    if (i != (int) decl.getParameters ().size () - 1 || lst.getParameters ().size () == decl.getParameters ().size ()) {
				auto varDecl = decl.getParameters ()[i];
				varDecl.to <syntax::VarDecl> ().setValue (TemplateSyntaxWrapper::init (lst.getLocation (), lst.getParameters ()[i]));
				values.push_back (validateValue (varDecl));
			    } else {
				std::vector <Generator> rest (lst.getParameters ().begin () + i, lst.getParameters ().end ());
				std::vector <Generator> types;
				for (auto & it : rest) {
				    types.push_back (it.to<Value> ().getType ());
				}
				auto tupleType = Tuple::init (lst.getLocation (), types);
				tupleType.to <Type> ().isMutable (true);
				tupleType.to <Type> ().isLocal (true);
				
				auto varDecl = decl.getParameters ()[i];
				varDecl.to<syntax::VarDecl> ().setValue (
				    TemplateSyntaxWrapper::init (
					lst.getLocation (),
					TupleValue::init (
					    lst.getLocation (),
					    tupleType,
					    rest
					)));
				
				values.push_back (validateValue (varDecl));				
			    }			    
			}
			return Set::init (decl.getLocation (), type, values);
		    })
		else {
		    if (decl.getParameters ().size () != 1) {
			Ymir::Error::occur (decl.getLocation (),
					    ExternalError::get (OVERFLOW_ARITY),
					    decl.getParameters ().size (),
					    1);
		    }
		    auto varDecl = decl.getParameters () [0];
		    varDecl.to<syntax::VarDecl> ().setValue (TemplateSyntaxWrapper::init (value.getLocation (), value));
		    return validateValue (varDecl);
		}
	    }
	    
	    return Generator::empty ();
	}

	Generator Visitor::validateLambda (const syntax::Lambda & function) {
	    auto name = format ("_%", function.getUniqId ());
	    auto frameName = this-> _referent.back ().getRealName () + "::" + name;
	    auto lambdaStored = this-> _lambdas.find (frameName);
	    if (lambdaStored != this-> _lambdas.end ()) { // We want to avoid multiple time validation
		return lambdaStored-> second;
	    }
	    
	    std::vector <Generator> params;
	    std::vector <Generator> paramsProto;
	    std::vector <Generator> paramTypes;
	    std::vector <std::string> errors;
	    Generator body (Generator::empty ());
	    Generator retType (Generator::empty ());

	    volatile bool uncomplete = false; // idem
	    auto syms = this-> _symbols.size () - 1; // index of the last symbol is the current enclosure of the frame
	    
	    enterForeign ();
	    enterBlock ();
	    {
		TRY (
		    for (auto & param : function.getPrototype ().getParameters ()) {
			auto var = param.to <syntax::VarDecl> ();
			Generator type (Generator::empty ());
			if (!var.getType ().isEmpty ()) {
			    type = validateType (var.getType ());
			} else {
			    uncomplete = true;
			}

			bool isMutable = false;
			bool isRef = false;
			if (!type.isEmpty ()) {
			    applyDecoratorOnVarDeclType (var.getDecorators (), type, isRef, isMutable);
			    verifyMutabilityRefParam (var.getLocation (), type, MUTABLE_CONST_PARAM);
			    if (type.is <NoneType> () || type.is<Void> ()) {
				Ymir::Error::occur (var.getLocation (), ExternalError::get (VOID_VAR));
			    }
			}
		
			params.push_back (ParamVar::init (var.getName (), type, isMutable));
			paramsProto.push_back (ProtoVar::init (var.getName (), type, Generator::empty (), isMutable));
			paramTypes.push_back (type);
			if (var.getName () != Keys::UNDER) {
			    verifyShadow (var.getName ());		
			    insertLocal (var.getName ().str, params.back ());
			}		
		    }
	   
		    if (!function.getPrototype ().getType ().isEmpty ()) {
			retType = validateType (function.getPrototype ().getType (), true);		
		    }

		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }

	    this-> discardAllLocals ();

	    {
		TRY (
		    quitBlock ();
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }
	    
	    exitForeign ();
		
	    if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);

	    auto proto = LambdaProto::init (function.getLocation (), frameName, retType, paramsProto, function.getContent (), function.isRefClosure (), function.isMoveClosure (), syms);
	    proto.to<LambdaProto>().setMangledName (format ("%%%", this-> _referent.back ().getMangledName (), name.length (), name));	    

	    if (!uncomplete) {
		return validateLambdaProto (proto.to <LambdaProto> (), paramTypes);
	    } else {
		return proto;
	    }
	}

	Generator Visitor::validateLambdaProto (const LambdaProto & proto, const std::vector <Generator> & types) {
	    std::vector <Generator> params;
	    std::vector <Generator> paramsProto;
	    std::vector <std::string> errors;
	    Generator body (Generator::empty ());
	    Generator retType (proto.getReturnType ());

	    volatile bool needFinalReturn = false;// mmmh, not understanding why, but gcc doesn't like it otherwise
	    enterForeign ();
	    enterBlock ();
	    {
		TRY (
		    for (auto it : Ymir::r (0, proto.getParameters ().size ())) {
			auto var = proto.getParameters ()[it].to <ProtoVar> ();
			bool isMutable = types [it].to <Type> ().isMutable ();
			params.push_back (ParamVar::init (var.getLocation (), types [it], isMutable));
			paramsProto.push_back (ProtoVar::init (var.getLocation (), types [it], Generator::empty (), isMutable));
			if (var.getName () != Keys::UNDER) {
			    insertLocal (var.getName (), params.back ());
			}		
		    }
		    
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;		    
	    }
	    
	    volatile uint refId = 0;
	    {
		TRY (		    
		    this-> setCurrentFuncType (retType);
		    refId = Generator::getLastId ();
		    if (proto.isRefClosure () || proto.isMoveClosure ())
			this-> enterClosure (proto.isRefClosure (), refId, proto.getClosureIndex ());
			    
		    body = validateValue (proto.getContent ());		    
		    retType = this-> getCurrentFuncType ();
	    
		    if (!body.to<Value> ().isReturner ()) {
			if (!retType.isEmpty ()) {
			    verifyMemoryOwner (body.getLocation (), retType, body, true);		    
			    needFinalReturn = !retType.is<Void> ();
			} else {
			    needFinalReturn = !body.to <Value> ().getType ().is<Void> ();
			    retType = body.to <Value> ().getType ();
			}
		    }
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;		    
	    }

	    Generator closure (Generator::empty ());
	    if (proto.isRefClosure () || proto.isMoveClosure ()) {
		closure = this-> exitClosure ();
		if (closure.to <Closure> ().getNames ().size () != 0) {
		    params.insert (params.begin (), ParamVar::init ({lexing::Word::eof (), "#_closure"}, closure, false));
		    params [0].setUniqId (refId);
		} else closure = Generator::empty ();
	    }
	    
	    {
		TRY ( // We want to guarantee that we exit the foreign at the end of this function 
		    quitBlock ();
		) CATCH (ErrorCode::EXTERNAL) {
		    GET_ERRORS_AND_CLEAR (msgs);
		    errors.insert (errors.end (), msgs.begin (), msgs.end ());
		} FINALLY;
	    }
	    
	    exitForeign ();
	    if (errors.size () != 0)
		THROW (ErrorCode::EXTERNAL, errors);

	    auto frame = Frame::init (proto.getLocation (), proto.getName (), params, retType, body, needFinalReturn);
	    frame.to <Frame> ().isWeak (true);
	    frame.to <Frame> ().setMangledName (proto.getMangledName ());

	    insertNewGenerator (frame);
		
	    auto frameProto = FrameProto::init (proto.getLocation (), proto.getName (), retType, paramsProto, false);
	    frameProto.to<FrameProto>().setMangledName (proto.getMangledName ());
		
	    auto funcType = FuncPtr::init (proto.getLocation (), frameProto.to <FrameProto> ().getReturnType (), types);
	    funcType.to <Type> ().isMutable (true);
	    auto addr = Addresser::init (proto.getLocation (), funcType, frameProto);
	    insert_or_assign (this-> _lambdas, proto.getName (), addr);
	    
	    if (!closure.isEmpty ()) {
		auto closureValue = validateClosureValue (closure, proto.isRefClosure (), proto.getClosureIndex ());
		auto tupleType = Delegate::init (proto.getLocation (), funcType);		
		auto tuple = DelegateValue::init (proto.getLocation (), tupleType, closureValue.to <Value> ().getType (), closureValue, addr);
						       
		return tuple;
	    } else {		
		return addr;
	    }
	}

	Generator Visitor::validateMultSymProto (const Generator & sym, const std::vector <Generator> &types) {
	    std::vector <Generator> valueParams;
	    for (auto it : Ymir::r (0, types.size ())) {
		valueParams.push_back (FakeValue::init (types [it].getLocation (), types [it]));
	    }
	    
	    std::vector <std::string> errors;
	    int score;
	    auto call = CallVisitor::init (*this);	    
	    auto ret = call.validate (sym.getLocation (), sym, valueParams, score, errors);
	    if (ret.isEmpty ()) 
		call.error ({sym.getLocation (), ""}, {sym.getLocation (), ""}, sym, valueParams, errors);
	    
	    return ret.to <Call> ().getFrame ();
	}

	Generator Visitor::validateClosureValue (const Generator & closureType, bool isRefClosure, uint closureIndex) {
	    std::vector <Generator> innerTypes;
	    std::vector <Generator> innerValues;
	    auto loc = closureType.getLocation ();
	    for (auto & name : closureType.to <Closure> ().getNames ()) {
		auto & syms = this-> _symbols [closureIndex];
		for (auto _it : Ymir::r (0, syms.size ())) {
		    auto ptr = syms [_it].find (name);
		    if (ptr != syms [_it].end ()) {
			if (ptr-> second.is <ParamVar> ()) {
			    auto type = ptr-> second.to <Value> ().getType ();
			    auto varRef = VarRef::init (loc, name, type, ptr-> second.getUniqId (), false, Generator::empty ());
			    if (isRefClosure) {
				type.to <Type> ().isRef (true);
				innerValues.push_back (Referencer::init (loc, type, varRef));
			    } else
				innerValues.push_back (varRef);
			    
			    innerTypes.push_back (type);
			} else if (ptr-> second.is <generator::VarDecl> ()) {
			    auto type = ptr-> second.to <generator::VarDecl> ().getVarType ();
			    auto varRef = VarRef::init (loc, name, type, ptr-> second.getUniqId (), false, Generator::empty ());
			    if (isRefClosure) {
				type.to <Type> ().isRef (true);
				innerValues.push_back (Referencer::init (loc, type, varRef));
			    } else {
				Generator value (Generator::empty ());
				if (!ptr-> second.to <generator::VarDecl> ().isMutable ())
				    value = ptr-> second.to <generator::VarDecl> ().getVarValue ();
				varRef = VarRef::init (loc, name, type, ptr-> second.getUniqId (), false, value);
				innerValues.push_back (varRef);
			    }
			    innerTypes.push_back (type);
			} else if (ptr-> second.is <StructAccess> ()) {
			    innerValues.push_back (ptr-> second);
			    innerTypes.push_back (ptr-> second.to <Value> ().getType ());
			}
			break; // We found it, go to the the next enclosure
		    }
		}
	    }
	    
	    auto tupleType = TupleClosure::init (loc, innerTypes);
	    auto tupleValue = TupleValue::init (loc, tupleType, innerValues);
	    return Copier::init (loc, Pointer::init (loc, Void::init (loc)), tupleValue);
	}	
	
	Generator Visitor::validateFuncPtr (const syntax::FuncPtr & ptr) {
	    std::vector <Generator> params;
	    if (ptr.getLocation () == Keys::FUNCTION) {
		for (auto & it : ptr.getParameters ()) {
		    params.push_back (validateType (it, true));
		}
		
		return FuncPtr::init (ptr.getLocation (), validateType (ptr.getRetType (), true), params);
	    } else if (ptr.getLocation () == Keys::DELEGATE) {
		for (auto & it : ptr.getParameters ()) {
		    params.push_back (validateType (it, true));
		}
		
		return Delegate::init (ptr.getLocation (), FuncPtr::init (ptr.getLocation (), validateType (ptr.getRetType (), true), params));
	    } else {
		Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");
		return Generator::empty ();
	    }
	}

	Generator Visitor::validateNullValue (const syntax::Null & nl) {
	    auto type = Pointer::init (nl.getLocation (), Void::init (nl.getLocation ()));
	    return NullValue::init (nl.getLocation (), type);
	}
	
	Generator Visitor::validateIntrinsics (const syntax::Intrinsics & intr) {
	    if (intr.isCopy ()) return validateCopy (intr);
	    if (intr.isAlias ()) return validateAlias (intr);
	    if (intr.isExpand ()) return validateExpand (intr);
	    if (intr.isDeepCopy ()) return validateDeepCopy (intr);
	    if (intr.isTypeof ()) {
		auto elem = validateValue (intr.getContent ());
		return elem.to <Value> ().getType ();
	    }
	    if (intr.isSizeof ()) {
		auto elem = validateType (intr.getContent (), true);
		return SizeOf::init (intr.getLocation (), Integer::init (intr.getLocation (), 0, false), elem);
	    }
	    
	    if (intr.isMove ()) {
		Ymir::Error::occur (intr.getLocation (),
				    ExternalError::get (MOVE_ONLY_CLOSURE));				    
	    }
	    
	    Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");
	    return Generator::empty ();
	}
       	
	Generator Visitor::validateCopy (const syntax::Intrinsics & intr) {
	    auto content = validateValue (intr.getContent ());

	    if (content.to <Value> ().getType ().is <Array> () || content.to <Value> ().getType ().is <Slice> ()) {
		auto type = content.to<Value> ().getType ();
		type.to <Type> ().isMutable (false);
		type = type.to <Type> ().toMutable ();
		type.to<Type> ().isLocal (false);
		
		if (type.is <Array> ()) {
		    if (type.is <Array> ()) {
			type = Slice::init (intr.getLocation (), type.to<Array> ().getInners () [0]);
			type.to<Type> ().isMutable (true);
		    }
		    
		    content = Aliaser::init (intr.getLocation (), type, content);
		}
		
		// The copy is done on the first level, so we don't have the right to change the mutability of inner data
		return Copier::init (intr.getLocation (), type, content);
	    } else {
		Ymir::Error::occur (
		    intr.getLocation (),
		    ExternalError::get (NO_COPY_EXIST),
		    content.to<Value> ().getType ().to <Type> ().getTypeName ()
		);
		
		return Generator::empty ();		
	    }	    
	}

	Generator Visitor::validateAlias (const syntax::Intrinsics & intr) {
	    auto content = validateValue (intr.getContent ());

	    if (content.to <Value> ().getType ().is <Array> () || content.to <Value> ().getType ().is <Slice> ()) {
		auto type = content.to <Value> ().getType ();
		if (type.is <Array> ()) {
		    type = Slice::init (intr.getLocation (), type.to<Array> ().getInners () [0]);
		    type.to<Type> ().isMutable (content.to <Value> ().getType ().to <Type> ().isMutable ());
		}
		return Aliaser::init (intr.getLocation (), type, content);
	    }
	    
	    auto type = content.to <Value> ().getType ();
	    if (!type.to <Type> ().isMutable ())
		Ymir::Error::occur (content.getLocation (),
				    ExternalError::get (NOT_A_LVALUE)
		);
	    
	    return Aliaser::init (intr.getLocation (), type, content);	    	    
	}

	Generator Visitor::validateDeepCopy (const syntax::Intrinsics & intr) {
	    auto inner = validateValue (intr.getContent ());
	    syntax::Expression call (syntax::Expression::empty ());
	    auto loc = intr.getLocation ();
	    if (inner.to <Value> ().getType ().is <ClassRef> ()) {
		auto trait = createVarFromPath (loc, {CoreNames::get (CORE_MODULE), CoreNames::get (DUPLICATION_MODULE), CoreNames::get (DCOPY_TRAITS)});		
		auto impl = validateType (trait);
		
		verifyClassImpl (intr.getLocation (), inner.to <Value> ().getType (), impl);		
		call = syntax::MultOperator::init (
		    {loc, Token::LPAR}, {loc, Token::RPAR},
		    syntax::Binary::init ({loc, Token::DOT},
					  TemplateSyntaxWrapper::init (inner.getLocation (), inner), 	       
					  syntax::Var::init ({loc, global::CoreNames::get (DCOPY_OP_OVERRIDE)}),
					  syntax::Expression::empty ()
		    ),
		    {}, false
		);
	    } else {
		auto func = createVarFromPath (loc, {CoreNames::get (CORE_MODULE), CoreNames::get (DUPLICATION_MODULE), CoreNames::get (DCOPY_OP_OVERRIDE)});
		
		call = syntax::MultOperator::init (
		    {loc, Token::LPAR}, {loc, Token::RPAR},
		    func,
		    {TemplateSyntaxWrapper::init (inner.getLocation (), inner)}	       
		);
	    }

	    auto val = validateValue (call);
	    return Aliaser::init (intr.getLocation (), val.to <Value> ().getType (), val);
	}
	
	Generator Visitor::validateExpand (const syntax::Intrinsics & intr) {
	    std::vector <Generator> values;
	    auto content = validateValue (intr.getContent ());
	    auto type = content.to <Value> ().getType ();
	    auto rref = UniqValue::init (intr.getLocation (), type, content);
	    
	    if (content.to<Value> ().getType ().is<Tuple> ()) {
		auto type = Void::init (intr.getLocation ());
		std::vector <Generator> expanded;
		auto & tu_inners = content.to <Value> ().getType ().to<Tuple> ().getInners ();
		for (auto it : Ymir::r (0, tu_inners.size ())) {
		    auto type = tu_inners [it];
		    if (content.to<Value> ().isLvalue () &&
			content.to <Value> ().getType ().to <Type> ().isMutable () &&
			type.to <Type> ().isMutable ())
			type.to <Type> ().isMutable (true);
		    else type.to<Type> ().isMutable (false);
		    
		    expanded.push_back (TupleAccess::init (intr.getLocation (), type, rref, it));
		}
		return List::init (intr.getLocation (), type, expanded);
	    } else {
		return content;
	    }
	}

	Generator Visitor::validateMultOperator (const syntax::MultOperator & op) {
	    if (op.getEnd () == Token::RCRO) {
		auto bracketVisitor = BracketVisitor::init (*this);
		return bracketVisitor.validate (op);
	    } else if (op.getEnd () == Token::RPAR) {
		auto callVisitor = CallVisitor::init (*this);
		return callVisitor.validate (op);
	    }

	    Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");
	    return Generator::empty ();
	}

	Generator Visitor::validateType (const syntax::Expression & expr, bool lock) {
	    auto type = validateType (expr);
	    if (lock && !type.to<Type> ().isMutable ())
		type.to <Type> ().isMutable (false);
	    return type;		
	}
	
	Generator Visitor::validateType (const syntax::Expression & type) {
	    Generator val (Generator::empty ());
	    match (type) {
		of (syntax::ArrayAlloc, array,
		    val = validateTypeArrayAlloc (array);
		);

		of (syntax::Var, var,
		    val = validateTypeVar (var);
		);

		of (syntax::DecoratedExpression, dec_expr,
		    val = validateTypeDecorated (dec_expr);
		);

		of (syntax::Unary, un,
		    val = validateTypeUnary (un);
		);

		of (syntax::List, list,
		    if (list.isArray ())
			val = validateTypeSlice (list);
		    if (list.isTuple ())
			val = validateTypeTuple (list);
		);

		of (TemplateSyntaxList, tmplSynt,
		    val = validateTypeTupleTemplate (tmplSynt);
		);
		
		of (TemplateSyntaxWrapper, tmplSynt,
		    val =  tmplSynt.getContent ();
		);

		of (syntax::TemplateCall, tmpCall,
		    val = validateTypeTemplateCall (tmpCall);		   
		);
		
	    }

	    if (val.isEmpty ()) {		
		val = validateValue (type, true, true); // Can't make a implicit call validation if we are looking for a type
	    }

	    if (val.is<Type> ()) return val;
	    if (val.is<generator::Struct> ())
		return StructRef::init (type.getLocation (), val.to <generator::Struct> ().getRef ());
	    if (val.is <generator::Class> ()) {		
		return val.to <generator::Class> ().getClassRef ();		
	    }
	    
	    if (val.is <StructCst> ()) return val.to <StructCst> ().getStr ();

	    Ymir::Error::occur (type.getLocation (), ExternalError::get (USE_AS_TYPE));
	    return Generator::empty ();	    	   
	}

	Generator Visitor::validateTypeVar (const syntax::Var & var) {
	    if (std::find (Integer::NAMES.begin (), Integer::NAMES.end (), var.getName ().str) != Integer::NAMES.end ()) {
		auto size = var.getName ().str.substr (1);
		
		// According to c++ documentation atoi return 0, if the conversion failed
		return Integer::init (var.getName (), std::atoi (size.c_str ()), var.getName ().str[0] == 'i');
	    } else if (var.getName ().str == Void::NAME) {
		return Void::init (var.getName ());
	    } else if (var.getName ().str == Bool::NAME) {
		return Bool::init (var.getName ());
	    } else if (std::find (Float::NAMES.begin (), Float::NAMES.end (), var.getName ().str) != Float::NAMES.end ()) {
		auto size = var.getName ().str.substr (1);
		return Float::init (var.getName (), std::atoi (size.c_str ())); 
	    } else if (std::find (Char::NAMES.begin (), Char::NAMES.end (), var.getName ().str) != Char::NAMES.end ()) {
		return Char::init (var.getName (), std::atoi (var.getName ().str.substr (1).c_str ()));
	    } else {		
		auto syms = getGlobal (var.getName ().str);
		// println( var.getName ().str, " ", this-> _referent.back ().formatTree ());
		// for (auto & it : syms)
		//     println (it.getRealName ());
		if (!syms.empty ()) {		    
		    auto ret = validateMultSymType (var.getLocation (), syms);		    
		    if (!ret.isEmpty ()) return ret;		    
		} else {
		    syms = getGlobalPrivate (var.getName ().str);
		    std::string note;
		    for (auto it : Ymir::r (0, syms.size ())) {
			if (it != 0) note = note + ("\n");
			note = note + Ymir::Error::createNoteOneLine (ExternalError::get (PRIVATE_IN_THIS_CONTEXT), syms [it].getName (), syms [it].getRealName ());
		    }
		    Error::occurAndNote (var.getName (), note, ExternalError::get (UNDEF_TYPE), var.getName ().str);
		}
	    }
	    
	    Error::occur (var.getName (), ExternalError::get (UNDEF_TYPE), var.getName ().str);
	    return Generator::empty ();
	}

	Generator Visitor::validateTypeUnary (const syntax::Unary & un) {
	    auto op = un.getOperator ();
	    if (op == Token::AND) { // Pointer
		auto inner = validateType (un.getContent (), true);
		if (!inner.isEmpty ()) return Pointer::init (un.getLocation (), inner);
	    }

	    return Generator::empty ();
	}

	Generator Visitor::validateTypeDecorated (const syntax::DecoratedExpression & expr) {
	    auto type = validateType (expr.getContent ());
	    
	    lexing::Word gotConstOrMut (lexing::Word::eof ());
	    for (auto & deco : expr.getDecorators ()) {
		switch (deco.getValue ()) {
		case syntax::Decorator::REF : type.to<Type> ().isRef (true); break;
		case syntax::Decorator::CONST : {
		    if (!gotConstOrMut.isEof ()) {
			auto note = Ymir::Error::createNote (gotConstOrMut);
			Ymir::Error::occurAndNote (expr.getDecorator (syntax::Decorator::CONST).getLocation (), note, ExternalError::get (CONFLICT_DECORATOR));
		    }
		    gotConstOrMut = expr.getDecorator (syntax::Decorator::CONST).getLocation ();
		    type.to<Type> ().isMutable (false); break;
		}
		case syntax::Decorator::MUT : {
		    if (!gotConstOrMut.isEof ()) {
			auto note = Ymir::Error::createNote (gotConstOrMut);
			Ymir::Error::occur (expr.getDecorator (syntax::Decorator::MUT).getLocation (), ExternalError::get (CONFLICT_DECORATOR));
		    }
		    gotConstOrMut = expr.getDecorator (syntax::Decorator::MUT).getLocation ();
		    type.to<Type> ().isMutable (true); break;
		}
		case syntax::Decorator::DMUT : {
		    if (!gotConstOrMut.isEof ()) {
			auto note = Ymir::Error::createNote (gotConstOrMut);
			Ymir::Error::occur (expr.getDecorator (syntax::Decorator::DMUT).getLocation (), ExternalError::get (CONFLICT_DECORATOR));		    
		    }
		    gotConstOrMut = expr.getDecorator (syntax::Decorator::DMUT).getLocation ();
		    type = type.to<Type> ().toDeeplyMutable (); break;
		}
		default :
		    Ymir::Error::occur (deco.getLocation (),
					ExternalError::get (DECO_OUT_OF_CONTEXT),
					deco.getLocation ().str
		    );		
		}
	    }
	    	   	    
	    return type;
	}

	Generator Visitor::validateTypeArrayAlloc (const syntax::ArrayAlloc & alloc) {
	    if (alloc.isDynamic ())
		Ymir::Error::occur (alloc.getLocation (), ExternalError::get (USE_AS_TYPE));

	    auto type = validateType (alloc.getLeft (), true);
	    auto size = validateValue (alloc.getSize ());

	    Generator value = retreiveValue (size);
	    if (!value.is <Fixed> () || (value.to<Fixed> ().getType ().to <Integer> ().isSigned () && value.to <Fixed> ().getUI ().i < 0)) {
		Ymir::Error::occur (alloc.getSize ().getLocation (), ExternalError::get (INCOMPATIBLE_TYPES),
				    value.to <Value> ().getType ().to <Type> ().getTypeName (),
				    (Integer::init (lexing::Word::eof (), 64, false)).to<Type> ().getTypeName ()
		);
	    }
	    
	    return Array::init (alloc.getLocation (), type, value.to <Fixed> ().getUI ().u);	    
	}

	Generator Visitor::validateTypeSlice (const syntax::List & list) {
	    if (list.getParameters ().size () != 1)
		Ymir::Error::occur (list.getLocation (), ExternalError::get (USE_AS_TYPE));

	    auto type = validateType (list.getParameters () [0], true);
	    return Slice::init (list.getLocation (), type);	    
	}
	
	Generator Visitor::validateTypeTuple (const syntax::List & list) {
	    std::vector <Generator> params;
	    for (auto & it : list.getParameters ()) {
		params.push_back (validateType (it, true));
	    }
	    
	    return Tuple::init (list.getLocation (), params);
	}	

	Generator Visitor::validateTypeTupleTemplate (const TemplateSyntaxList & lst) {
	    std::vector<Generator> params;
	    for (auto & it : lst.getContents ()) {
		params.push_back (validateType (TemplateSyntaxWrapper::init (it.getLocation (), it), true));
	    }

	    return Tuple::init (lst.getLocation (), params);
	}
	
	Generator Visitor::validateTypeTemplateCall (const syntax::TemplateCall & call) {	    
	    auto left = call.getContent ();
	    match (left) {
		of (syntax::Var, var, {
			Generator innerType (Generator::empty ());
			if (call.getParameters ().size () == 1) {
			    innerType = validateType (call.getParameters ()[0], true);
			}
			
			if (var.getName ().str == Range::NAME) {
			    return Range::init (var.getName (), innerType);
			}		       
		    }
		);
	    }
	    
	    return Generator::empty ();
	}
	
	const std::vector <generator::Generator> & Visitor::getGenerators () const {
	    return this-> _list;
	}
	
	void Visitor::insertNewGenerator (const Generator & generator) {
	    this-> _list.push_back (generator);
	}
	
	void Visitor::enterBlock () {
	    this-> _usedSyms.back ().push_back ({});
	    this-> _symbols.back ().push_back ({});
	}
	
	void Visitor::quitBlock () {	    
	    if (this-> _symbols.back ().empty ())
		Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");

	    std::vector <std::string> errors;
	    for (auto & sym : this-> _symbols.back ().back ()) {
		if (sym.first != Keys::SELF) { // SELF is like "_", we don't need it to be used
		    if (this-> _usedSyms.back ().back ().find (sym.first) == this-> _usedSyms.back ().back ().end ()) {
			errors.push_back (Error::makeWarn (sym.second.getLocation (), ExternalError::get (NEVER_USED), sym.second.getName ()));
		    }
		}
	    }

	    this-> _usedSyms.back ().pop_back ();
	    this-> _symbols.back ().pop_back ();

	    if (errors.size () != 0) {
		THROW (ErrorCode::EXTERNAL, errors);
	    }
	}

	std::map <std::string, generator::Generator> Visitor::discardAllLocals () {
	    auto ret = this-> _symbols.back ().back ();
	    this-> _symbols.back ().back () = {};
	    return ret;
	}
	
	void Visitor::enterForeign () {
	    this-> _usedSyms.push_back ({});
	    this-> _symbols.push_back ({});
	    this-> _loopSaved.push_back (this-> _loopBreakTypes);
	    this-> _loopBreakTypes = {};
	}
	
	void Visitor::exitForeign () {
	    this-> _usedSyms.pop_back ();
	    this-> _symbols.pop_back ();
	    this-> _loopBreakTypes = this-> _loopSaved.back ();
	    this-> _loopSaved.pop_back ();
	}

	void Visitor::enterClosure (bool isRef, uint refId, uint index) {
	    insert_or_assign (this-> _symbols.back ()[0], "#{CLOSURE}", BoolValue::init (lexing::Word::eof (), Bool::init (lexing::Word::eof ()), isRef));
	    this-> _usedSyms.back ()[0].insert ("#{CLOSURE}");

	    insert_or_assign (this-> _symbols.back ()[0], "#{CLOSURE-TYPE}", Closure::init (lexing::Word::eof (), {}, {}, index));
	    this-> _usedSyms.back ()[0].insert ("#{CLOSURE-TYPE}");

	    insert_or_assign (this-> _symbols.back ()[0], "#{CLOSURE-VARREF}", VarRef::init (lexing::Word::eof (), "#{CLOSURE-VARREF}", Void::init (lexing::Word::eof ()), refId, false, Generator::empty ()));
	    this-> _usedSyms.back () [0].insert ("#{CLOSURE-VARREF}");
	    this-> _enclosed.push_back ({});	    
	}

	Generator Visitor::getInClosure (const std::string & name) {
	    if (!isInClosure ()) return Generator::__empty__;
	    auto closureType = this-> getLocal ("#{CLOSURE-TYPE}", false);
	    bool isRefClosure = this-> getLocal ("#{CLOSURE}").to <BoolValue> ().getValue ();
	    auto field = closureType.to <Closure> ().getField (name);
	    if (field.isEmpty ()) { // need to get it from upper closure
		auto & syms = this-> _symbols [closureType.to <Closure> ().getIndex ()];
		auto & used = this-> _usedSyms [closureType.to <Closure> ().getIndex ()]; 
		for (auto _it : Ymir::r (0, syms.size ())) {
		    auto ptr = syms [_it].find (name);
		    if (ptr != syms [_it].end ()) {
			used [_it].insert (name);
			Generator type (Generator::empty ());
			if (ptr-> second.is <generator::VarDecl> ()) {
			    type = ptr-> second.to <generator::VarDecl> ().getVarType ();
			} else type = ptr-> second.to <Value> ().getType ();
			
			auto types = closureType.to <Type> ().getInners ();
			type.to <Type> ().isMutable (false);
			type.to <Type> ().isRef (isRefClosure);
			
			auto names = closureType.to <Closure> ().getNames ();
			types.push_back (type);
			names.push_back (name);

			closureType = Closure::init (lexing::Word::eof (), types, names, closureType.to <Closure> ().getIndex ());
			closureType.to <Type> ().isRef (true);
			
			insert_or_assign (this-> _symbols.back ()[0], "#{CLOSURE-TYPE}", closureType);
			auto closureRef = this-> getLocal ("#{CLOSURE-VARREF}", false);
			insert_or_assign (this-> _symbols.back ()[0], "#{CLOSURE-VARREF}", VarRef::init (lexing::Word::eof (), "#{CLOSURE-VARREF}", closureType, closureRef.to <VarRef> ().getRefId (), false, Generator::empty ()));			
			return StructAccess::init (lexing::Word::eof (), type, this-> getLocal ("#{CLOSURE-VARREF}", false), name);
		    }
		}
	    } else {
		auto closureRef = this-> getLocal ("#{CLOSURE-VARREF}", false);
		return StructAccess::init (lexing::Word::eof (), field, closureRef, name);
	    }

	    return Generator::__empty__;
	}	
	
	bool Visitor::isInClosure () {
	    auto ret = this-> getLocal ("#{CLOSURE}", false);
	    return !ret.isEmpty ();
	}
	
	bool Visitor::isInRefClosure () {
	    auto ret = this-> getLocal ("#{CLOSURE}", false);
	    return !ret.isEmpty () && ret.to<BoolValue> ().getValue ();
	}
	
	Generator Visitor::exitClosure () {
	    return this-> getLocal ("#{CLOSURE-TYPE}", false);
	}
	
	void Visitor::enterContext (const std::vector <lexing::Word> & Cas) {
	    this-> _contextCas.push_back (Cas);
	}

	void Visitor::exitContext () {
	    if (this-> _contextCas.empty ())
		Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");

	    this-> _contextCas.pop_back ();
	}

	void Visitor::enterClassDef (const semantic::Symbol & sym) {
	    this-> _classContext.push_back (sym);
	}

	void Visitor::exitClassDef () {
	    this-> _classContext.pop_back ();
	}
              
	void Visitor::getClassContext (const semantic::Symbol & cl, bool & isPrivate, bool & isProtected) {
	    isPrivate = false;
	    isProtected = false;
	    if (this-> _classContext.size () != 0) {
		isPrivate = this-> _classContext.back ().equals (cl);
		if (isPrivate) isProtected = true;
		else {
		    Symbol clSym (Symbol::empty ());
		    auto ancestor = this-> _classContext.back ().to <semantic::Class> ().getGenerator ().to <generator::Class> ().getClassRef ().to <ClassRef> ().getAncestor ();
		    if (!ancestor.isEmpty ())
			clSym = ancestor.to <ClassRef> ().getRef ();
		    
		    while (!clSym.isEmpty ()) {
			if (clSym.equals (cl)) {
			    isProtected = true;
			    break;
			} else {
			    auto ancestor = clSym.to <semantic::Class> ().getGenerator ().to <generator::Class> ().getClassRef ().to <ClassRef> ().getAncestor ();
			    if (!ancestor.isEmpty ())
				clSym = ancestor.to <ClassRef> ().getRef ();
			    else break;
			}
		    }
		}
	    }
	}

	bool Visitor::getModuleContext (const semantic::Symbol & cl) {
	    auto module = cl;
	    if (module.is <ModRef> ()) module = module.to <ModRef> ().getModule ();
	    
	    if (this-> _referent.size () != 0) {
		auto curr = this-> _referent.back ();
		while (!curr.isEmpty () && !module.isEmpty ()) {
		    if (curr.equals (module)) return true; 
		    curr = curr.getReferent ();
		}
	    }
	    return false;
	}
	
	bool Visitor::isInContext (const std::string & context) {
	    if (!this-> _contextCas.empty ()) {
		for (auto & it : this-> _contextCas.back ())
		    if (it.str == context) return true;
	    }
	    return false;
	}
	
	void Visitor::enterLoop () {
	    this-> _loopBreakTypes.push_back (Generator::empty ());	    
	}

	Generator Visitor::quitLoop () {
	    auto last = this-> _loopBreakTypes.back ();
	    this-> _loopBreakTypes.pop_back ();
	    return last;
	}

	const Generator & Visitor::getCurrentLoopType () const {
	    return this-> _loopBreakTypes.back ();
	}

	void Visitor::setCurrentLoopType (const Generator & type) {
	    this-> _loopBreakTypes.back () = type;
	}

	Generator Visitor::getCurrentFuncType () {
	    return getLocal ("#{RET}", false);
	}

	void Visitor::setCurrentFuncType (const Generator & type) {
	    insert_or_assign (this-> _symbols.back () [0], "#{RET}", type);
	    this-> _usedSyms.back () [0].insert ("#{RET}");
	}
	
	bool Visitor::isInLoop () const {
	    return !this-> _loopBreakTypes.empty ();
	}
	
	void Visitor::insertLocal (const std::string & name, const Generator & gen) {
	    if (this-> _symbols.back ().empty ())
		Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");
	    if (this-> _symbols.back ().back ().find (name) == this-> _symbols.back ().back ().end ())
		insert_or_assign (this-> _symbols.back ().back (), name, gen);
	    else
		insert_or_assign (this-> _symbols.back ().back (), name, gen);
	}       

	Generator Visitor::getLocal (const std::string & name, bool canBeInClosure) {
	    for (auto _it : Ymir::r (0, this-> _symbols.back ().size ())) {
		auto it = (this-> _symbols.back ().size () - _it) - 1;
		auto ptr = this-> _symbols.back () [it].find (name); 		    
		if (ptr != this-> _symbols.back () [it].end ()) {
		    this-> _usedSyms.back () [it].insert (name);
		    return ptr-> second;
		}		
	    }

	    if (canBeInClosure)
		return this-> getInClosure (name);
	    
	    return Generator::__empty__;
	}

	void Visitor::printLocal () const {
	    for (auto _it : Ymir::r (0, this-> _symbols.back ().size ())) {
		println (Ymir::format ("%* {", (int) _it, '\t'));
		for (auto it : this-> _symbols.back () [_it]) {		    
		    println (Ymir::format ("%* %-> %", (int) (_it + 1), '\t', it.first, it.second.prettyString ()));
		}
		println (Ymir::format ("%* }", (int)_it, '\t'));
	    }
	}	

	void Visitor::verifyMemoryOwner (const lexing::Word & loc, const Generator & type, const Generator & gen, bool construct, bool checkTypes, bool inMatch) {
	    if (checkTypes) 
		verifyCompatibleTypeWithValue (loc, type, gen);

	    verifyCompleteType (loc, type);
	    verifyImplicitAlias (loc, type, gen);
	    
	    // Verify Implicit referencing
	    if ((!construct || !type.to <Type> ().isRef ()) && gen.is<Referencer> ()) {
		if (!inMatch)
		    Ymir::Error::warn (gen.getLocation (), ExternalError::get (REF_NO_EFFECT));
	    } else {
		if (type.to <Type> ().isRef () && construct) {
		    verifySameType (type, gen.to <Value> ().getType ());
		    
		    if (!gen.is<Referencer> ()) {
			if (gen.to<Value> ().isLvalue ()) {
			    Ymir::Error::occur (gen.getLocation (), ExternalError::get (IMPLICIT_REFERENCE),
						gen.to<Value> ().getType ().to <Type> ().getTypeName ()
			    );
			} else {
			    auto note = Ymir::Error::createNote (loc);
			    Ymir::Error::occurAndNote (gen.getLocation (), note, ExternalError::get (NOT_A_LVALUE));
			}
		    }
		}
	    }

	    // Tuple copy is by default, as we cannot alias a tuple
	    // And for arrays (but left op)
	    if (gen.to <Value> ().getType ().is <Tuple> () || gen.to <Value> ().getType ().is <Range> () || type.is<Array> ()) {
		auto tu = gen.to<Value> ().getType ();
		auto llevel = type.to <Type> ().mutabilityLevel ();
		auto rlevel = tu.to <Type> ().mutabilityLevel ();
		if (llevel > std::max (1, rlevel)) { // left operand can be mutable, but it can't modify inner right operand values
		    auto note = Ymir::Error::createNote (gen.getLocation ());
		    Ymir::Error::occurAndNote (loc, note, ExternalError::get (DISCARD_CONST_LEVEL),
					       llevel, std::max (1, rlevel)
		    );
		}		
	    } else if (type.is <StructRef> () && !type.to <StructRef> ().getRef ().to <semantic::Struct> ().getGenerator ().to <generator::Struct> ().hasComplexField ()) {
		auto tu = gen.to<Value> ().getType ();
		auto llevel = type.to <Type> ().mutabilityLevel ();
		auto rlevel = tu.to <Type> ().mutabilityLevel ();
		if (llevel > std::max (1, rlevel)) { // left operand can be mutable, but it can't modify inner right operand values
		    auto note = Ymir::Error::createNote (gen.getLocation ());
		    Ymir::Error::occurAndNote (loc, note, ExternalError::get (DISCARD_CONST_LEVEL),
					       llevel, std::max (1, rlevel)
		    );
		}		
	    } else if (type.is<Pointer> ()) {
		auto llevel = type.to <Type> ().mutabilityLevel ();
		auto rlevel = gen.to<Value> ().getType ().to <Type> ().mutabilityLevel ();
		if (llevel > std::max (1, rlevel)) {
		    auto note = Ymir::Error::createNote (gen.getLocation ());
		    Ymir::Error::occurAndNote (loc, note, ExternalError::get (DISCARD_CONST_LEVEL),
					       llevel, std::max (1, rlevel)
		    );
		}		
	    } else if (type.is<FuncPtr> ()) { // Yes, i know that's ugly, but easier to understand actually	
	    } else if (type.is<LambdaType> ()) {
		if (!construct || !gen.is<LambdaProto> ()) {
		    auto note = Ymir::Error::createNote (loc);
		    Ymir::Error::occurAndNote (gen.getLocation (), note, ExternalError::get (USE_AS_VALUE));
		} else if (type.to <Type> ().isMutable ())
		    Ymir::Error::occur (gen.getLocation (), ExternalError::get (DISCARD_CONST_LEVEL),
					1, 0
		    );	 
	    } else {
		// Verify mutability
		if (type.to<Type> ().isComplex () || type.to <Type> ().isRef ()) {
		    auto llevel = type.to <Type> ().mutabilityLevel ();
		    auto rlevel = gen.to <Value> ().getType ().to <Type> ().mutabilityLevel ();
		    if ((type.to <Type> ().isRef () && construct && llevel > std::max (0, rlevel))) { // If it is the construction of a ref
			auto note = Ymir::Error::createNote (gen.getLocation ());
			Ymir::Error::occurAndNote (loc, note, ExternalError::get (DISCARD_CONST_LEVEL),
						   llevel, std::max (0, rlevel)
			);		
		    } else if (llevel > std::max (1, rlevel)) {
			auto note = Ymir::Error::createNote (gen.getLocation ());
			Ymir::Error::occurAndNote (loc, note, ExternalError::get (DISCARD_CONST_LEVEL),
						   llevel, std::max (1, rlevel)
			);
		    }
		}
	    }

	    // TODO Verify locality
	    // if (!type.to<Type> ().isLocal () && gen.to <Value> ().getType ().to <Type> ().isLocal ()) {
	    // 	Ymir::Error::occur (loc, ExternalError::get (DISCARD_LOCALITY));				    
	    // }
	}

	void Visitor::verifyImplicitAlias (const lexing::Word & loc, const Generator & type, const Generator & gen) {
	    if (!type.to <Type> ().needExplicitAlias ()) return; // No need to explicitly alias 
	    if (gen.is<Copier> () || gen.is <Aliaser> () || gen.is <Referencer> ()) return; // It is aliased or copied, that's ok
	    match (gen) {
		of (ArrayValue, arr ATTRIBUTE_UNUSED, return);
		of (StructCst, arr ATTRIBUTE_UNUSED, return);
		of (ClassCst, arr ATTRIBUTE_UNUSED, return);
		of (Block, arr, { if (!arr.isLvalue ()) return; });
		of (Conditional, arr ATTRIBUTE_UNUSED, return);
		of (ExitScope, arr ATTRIBUTE_UNUSED, return);
		of (SuccessScope, arr ATTRIBUTE_UNUSED, return);
	    }
	    auto llevel = type.to <Type> ().mutabilityLevel ();

	    auto max_level = type.is <ClassRef> () ? 0 : 1;
	    // If the type is totally immutable, it's it not necessary to make an explicit alias 
	    if (llevel > max_level) {
		auto note = Ymir::Error::createNote (gen.getLocation (), ExternalError::get (IMPLICIT_ALIAS),
						     gen.to <Value> ().getType ().to <Type> ().getTypeName ());

		if (type.is <StructRef> ()) {
		    auto & varDecl = type.to <StructRef> ().getExplicitAliasTypeLoc ();
		    note = note + Ymir::Error::createNote (varDecl.getLocation (), ExternalError::get (IMPLICIT_ALIAS),
							   varDecl.to <generator::VarDecl> ().getVarType ().prettyString ());
		}
		
		Ymir::Error::occurAndNote (loc, note, ExternalError::get (DISCARD_CONST_LEVEL),
					   llevel, max_level
		);
	    }
	}

	void Visitor::verifyCompleteType (const lexing::Word & loc, const Generator &type) {
	    if (type.is <LambdaType> ()) {
		if (!loc.isSame (type.getLocation ())) {
		    auto note = Ymir::Error::createNote (loc);
		    Ymir::Error::occurAndNote (type.getLocation (), note, ExternalError::get (INCOMPLETE_TYPE), type.prettyString ());
		} else
		    Ymir::Error::occur (type.getLocation (), ExternalError::get (INCOMPLETE_TYPE), type.prettyString ());
	    }
	}
	
	void Visitor::applyDecoratorOnVarDeclType (const std::vector <syntax::DecoratorWord> & decos, Generator & type, bool & isRef, bool & isMutable) {
	    isMutable = false;
	    isRef = false;
	    for (auto & deco : decos) {
		switch (deco.getValue ()) {
		case syntax::Decorator::REF : {
		    if (!type.isEmpty ())
			type.to <Type> ().isRef (true);
		    isRef = true;
		} break;
		case syntax::Decorator::MUT : {
		    if (!type.isEmpty ())
			type.to <Type> ().isMutable (true);
		    isMutable = true;
		} break;
		case syntax::Decorator::DMUT : {
		    if (!type.isEmpty ())
			type = type.to <Type> ().toDeeplyMutable ();
		    isMutable = true;
		} break;
		default :
		    Ymir::Error::occur (deco.getLocation (),
					ExternalError::get (DECO_OUT_OF_CONTEXT),
					deco.getLocation ().str
		    );
		}
	    }
	    
	    if (!isMutable) type.to <Type>().isMutable (false);
	    if (!isRef) type.to <Type>().isRef (false);
	}

	void Visitor::verifyMutabilityRefParam (const lexing::Word & loc, const Generator & type, Ymir::ExternalErrorValue error) {
	    // Exception slice can be mutable even if it is not a reference, that is the only exception
	    if (type.to<Type> ().isMutable () && !type.to<Type> ().isRef () && !type.to <Type> ().needExplicitAlias ()) {
		Ymir::Error::occur (loc, ExternalError::get (error));
	    }	    
	}

	void Visitor::verifySameType (const Generator & left, const Generator & right) {
	    if (!left.equals (right)) {
		if (left.getLocation ().line == right.getLocation ().line && left.getLocation ().column == right.getLocation ().column) 
		    Ymir::Error::occur (left.getLocation (), ExternalError::get (INCOMPATIBLE_TYPES),
					left.to<Type> ().getTypeName (),
					right.to <Type> ().getTypeName ()
		    );
		else {
		    auto note = Ymir::Error::createNote (right.getLocation ());
		    Ymir::Error::occurAndNote (left.getLocation (), note, ExternalError::get (INCOMPATIBLE_TYPES),
					left.to<Type> ().getTypeName (),
					right.to <Type> ().getTypeName ()
		    );
		}
	    }
	}

	void Visitor::verifyClassImpl (const lexing::Word & loc, const Generator & cl, const Generator & trait) {
	    if (!trait.is <TraitRef> ()) {
		Ymir::Error::occur (trait.getLocation (), ExternalError::get (IMPL_NO_TRAIT), trait.prettyString ());
	    }

	    auto sym = cl.to <ClassRef> ().getRef ();
	    while (!sym.isEmpty ()) {
		for (auto & it : sym.to <semantic::Class> ().getAllInner ()) {
		    match (it) {
			of (semantic::Impl, im, {
			    auto sec_trait = this-> validateType (im.getTrait ());
			    if (trait.equals (sec_trait)) return;
			    }
			);
		    }		
		}
		auto ancestor = sym.to <semantic::Class> ().getGenerator ().to <generator::Class> ().getClassRef ().to <ClassRef> ().getAncestor ();
		if (!ancestor.isEmpty ())
		    sym = ancestor.to <ClassRef> ().getRef ();
		else break;
	    } 

	    auto note = Ymir::Error::createNote (loc);
	    Ymir::Error::occurAndNote (cl.getLocation (), note, ExternalError::get (NOT_IMPL_TRAIT), cl.prettyString (), trait.prettyString ());
	}

	std::vector <Generator> Visitor::getAllImplClass (const Generator &cl) {
	    auto sym = cl.to <ClassRef> ().getRef ();
	    std::vector <Generator> traits;
	    for (auto & it : sym.to <semantic::Class> ().getAllInner ()) {
		match (it) {
		    of (semantic::Impl, im, {
			    auto sec_trait = this-> validateType (im.getTrait ());
			    traits.push_back (sec_trait);
			}
		    );
		}		
	    }
	    return traits;
	}
	
	void Visitor::verifyCompatibleTypeWithValue (const lexing::Word & loc, const Generator & type, const Generator & gen) {
	    if (gen.is <NullValue> () && type.is <Pointer> ()) return;
	    else if (gen.to <Value> ().getType ().is <Slice> () && gen.to <Value> ().getType ().to <Type> ().getInners () [0].is<Void> () && type.is <Slice> ()) return;
	
	    verifyCompatibleType (loc, type, gen.to <Value> ().getType ());
	}	

	
	void Visitor::verifyCompatibleType (const lexing::Word & loc, const Generator & left, const Generator & right) {
	    bool error = false;
	    std::string leftName;
	    if (!left.to<Type> ().isCompatible (right)) {
		// It can be compatible with an ancestor of right
		if (right.is <ClassRef> () && !right.to <ClassRef> ().getRef ().to <semantic::Class> ().getAncestor ().isEmpty ()) {
		    auto ancestor = right.to <ClassRef> ().getAncestor ();
		    while (!ancestor.isEmpty ()) {
			if (left.to <Type> ().isCompatible (ancestor)) return;
			else {
			    if (!ancestor.to <ClassRef> ().getRef ().to <semantic::Class> ().getAncestor ().isEmpty ())
				ancestor = ancestor.to <ClassRef> ().getAncestor ();
			    else ancestor = Generator::empty ();
			}
		    }
		}
		error = true;
		leftName = left.to<Type> ().getTypeName ();
	    }
	    
	    if (!left.to <Type> ().getProxy ().isEmpty () && !left.to <Type> ().getProxy ().to <Type> ().isCompatible (right.to <Type> ().getProxy ())) {
		error = true;
		leftName = left.to<Type> ().getProxy ().to <Type> ().getTypeName ();
	    }

	    if (error) {
		if (loc.line == right.getLocation ().line) 
		    Ymir::Error::occur (loc, ExternalError::get (INCOMPATIBLE_TYPES),
					leftName, 
					right.to <Type> ().getTypeName ()
		    );
		else {
		    auto note = Ymir::Error::createNote (right.getLocation ());
		    Ymir::Error::occurAndNote (loc, note, ExternalError::get (INCOMPATIBLE_TYPES),
					       leftName,
					       right.to <Type> ().getTypeName ()
		    );
		}
	    }

	}	

	void Visitor::verifyShadow (const lexing::Word & name) {
	    verifyNotIsType (name);
	    
	    auto gen = getLocal (name.str);	    
	    if (!gen.isEmpty ()) {		
		auto note = Ymir::Error::createNote (gen.getLocation ());		
		Error::occurAndNote (name, note, ExternalError::get (SHADOWING_DECL), name.str);
	    }	    
	}

	void Visitor::verifyNotIsType (const lexing::Word & name) {
	    if (std::find (Integer::NAMES.begin (), Integer::NAMES.end (), name.str) != Integer::NAMES.end ()) {
		Error::occur  (name, ExternalError::get (IS_TYPE), name.str);
	    } else if (name.str == Void::NAME) {
		Error::occur  (name, ExternalError::get (IS_TYPE), name.str);
	    } else if (name.str == Bool::NAME) {
		Error::occur  (name, ExternalError::get (IS_TYPE), name.str);
	    } else if (std::find (Float::NAMES.begin (), Float::NAMES.end (), name.str) != Float::NAMES.end ()) {
		Error::occur  (name, ExternalError::get (IS_TYPE), name.str);
	    } else if (std::find (Char::NAMES.begin (), Char::NAMES.end (), name.str) != Char::NAMES.end ()) {
		Error::occur  (name, ExternalError::get (IS_TYPE), name.str);
	    }
	}
	
	void Visitor::verifySafety (const lexing::Word & location) const {
	    if (this-> _contextCas.empty ())
		Ymir::Error::halt ("%(r) - reaching impossible point", "Critical");
	    
	    for (auto & it : this-> _contextCas.back ()) {
		if (it == Keys::SAFE) {
		    auto note = Ymir::Error::createNote (it);
		    Ymir::Error::occur (location,
					ExternalError::get (SAFE_CONTEXT)
		    );
		}
	    }
	}
	
	Generator Visitor::retreiveValue (const Generator & gen) {
	    auto compile_time = CompileTime::init (*this);
	    return compile_time.execute (gen);
	}

	const Generator & Visitor::retreiveFrameFromProto (const FrameProto & proto) {
	    auto name = Mangler::init ().mangleFrameProto (proto);
	    for (auto & it : this-> _list) {
		if (it.is<Frame> ()) {
		    auto sec_name = Mangler::init ().mangleFrame (it.to <Frame> ());
		    if (sec_name == name) return it;
		}
	    }
	    return Generator::__empty__;
	}

	bool Visitor::insertTemplateSolution (const Symbol & sol) {
	    for (auto & it : this-> _templateSolutions) {
		if (it.equals (sol)) return false;		
	    }
	    
	    this-> _templateSolutions.push_back (sol);
	    return true;
	}
	
	std::vector <Symbol> Visitor::getGlobal (const std::string & name) {					       
	    return this-> _referent.back ().get (name);
	}

	std::vector <Symbol> Visitor::getGlobalPrivate (const std::string & name) {					       
	    return this-> _referent.back ().getPrivate (name);
	}	
	
	void Visitor::pushReferent (const semantic::Symbol & sym, const std::string &) {
	    // print (this-> _referent.size (), ' ');
	    // println ("IN : ", msg, " => ", sym.getRealName ());
	    this-> _referent.push_back (sym);
	}

	void Visitor::popReferent (const std::string &) {
	    //print (this-> _referent.size ());
	    //println ("Out : ", msg, " => ", this-> _referent.back ().getRealName ());
	    this-> _referent.pop_back ();
	}

	syntax::Expression Visitor::createVarFromPath (const lexing::Word & loc, const std::vector <std::string> & names_) {
	    auto names = names_;
	    syntax::Expression last = syntax::Var::init ({loc, names [0]});
	    names = std::vector <std::string> (names.begin () + 1, names.end ());
	    while (names.size () > 0) {
		last = syntax::Binary::init (
		    {loc, Token::DCOLON},
		    last,
		    syntax::Var::init ({loc, names [0]}),
		    syntax::Expression::empty ()
		);
		names = std::vector <std::string> (names.begin () + 1, names.end ());
	    }	    
	    return last;
	}
	
	bool Visitor::isUseless (const Generator & value) {
	    match (value) {
		of (Affect, af ATTRIBUTE_UNUSED,  return false;);
		of (Block,  bl ATTRIBUTE_UNUSED,  return false;);
		of (Break,  br ATTRIBUTE_UNUSED,  return false;);
		of (Call,   cl ATTRIBUTE_UNUSED,  return false;);
		of (Conditional, c ATTRIBUTE_UNUSED, return false;);
		of (Loop,   lp ATTRIBUTE_UNUSED,  return false;);
		of (generator::VarDecl, vd ATTRIBUTE_UNUSED, return false;);
		of (Return, rt ATTRIBUTE_UNUSED, return false;);
		of (Set, set ATTRIBUTE_UNUSED, return false;);
		of (Throw, th ATTRIBUTE_UNUSED, return false;);
		of (ExitScope, ex ATTRIBUTE_UNUSED, return false;);
	    }
	    return true;
	}	
	
    }
    
}
