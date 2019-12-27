#pragma once

#include <ymir/syntax/Declaration.hh>
#include <ymir/syntax/expression/_.hh>

#include <ymir/semantic/Symbol.hh>
#include <ymir/semantic/symbol/_.hh>
#include <ymir/semantic/Generator.hh>
#include <ymir/semantic/generator/_.hh>
#include <ymir/utils/Match.hh>
#include <ymir/syntax/expression/VarDecl.hh>

#include <list>
#include <ymir/lexing/Word.hh>
#include <ymir/errors/Error.hh>
#include <ymir/errors/ListError.hh>


namespace semantic {

    namespace validator {

	/**
	 * \struct Visitor
	 * This class is the final semantic validation before code production
	 * It will traverse all the declared symbol of a given module and validated each of them
	 */
	class Visitor {

	    enum VisitConstante {
		LIMIT_TEMPLATE_RECUR = 300
	    };

	    enum ClosureState {
		NONE = 0,
		REF = 1,
		MOVE = 2
	    };

	    static std::string TYPE_INFO;
	    static std::string TYPE_IDS;
	    static std::string DCOPY_OP_OVERRIDE;
	    
	    /** List of referent for symbol access and insertion */
	    std::list <Symbol> _referent;	

	    /** The list of generator produced by the search */
	    std::vector <generator::Generator> _list;
	    
	    /** The list of loop breaks types (when inside a loop) cf enterLoop and quitLoop */
	    std::list <generator::Generator> _loopBreakTypes;

	    /** The backup of the loop break when entering a foreign */
	    std::list <std::list <generator::Generator> > _loopSaved;

	    std::vector <std::vector <std::set <std::string> > > _usedSyms;
	    
	    std::vector <std::vector <std::map <std::string, generator::Generator> > > _symbols;

	    /** The list of declared and usable structures */
	    std::vector <semantic::Symbol>     _structSyms;
	    
	    std::vector <generator::Generator> _structs;

	    /** All the solution generated by template specialization */
	    std::vector <semantic::Symbol> _templateSolutions;

	    /** The current Cas context */
	    std::vector <std::vector <lexing::Word> > _contextCas;

	    /** The already validated lambdas */
	    std::map <std::string, generator::Generator> _lambdas;

	    /** The current enclosed vars */
	    std::vector <std::vector <generator::Generator> > _enclosed;
	    
	private :

	    /** 
	     * Does nothing special
	     * It is private for homogeneity reason
	     * We wan't all class to be initialized the same way
	     */
	    Visitor ();

	public :

	    /**
	     * \brief Create an empty visitor
	     */
	    static Visitor init ();

	    /**
	     * \brief Validate a symbol 
	     * \brief Traverse all inner declaration and validate them
	     * \brief If no errors occurs the returned list of generators are ready for code generation
	     */	    
	    void validate (const semantic::Symbol & sym);


	    /**
	     * \brief Validate a module
	     */
	    void validateModule (const semantic::Module & mod);

	    /**
	     * \brief Validate a module
	     */
	    void validateTemplateSolution (const semantic::TemplateSolution & sol);
	    
	    /**
	     * \brief Validate a function
	     */
	    void validateFunction (const semantic::Function & func, bool isWeak = false);

	    /**
	     * \brief Validate a global var declaration 
	     */
	    void validateVarDecl (const semantic::Symbol & sym);


	    /**
	     * \brief Validate an alias
	     */
	    generator::Generator validateAlias   (const semantic::Symbol & sym);
	    
	    /**
	     * \brief Validate an enum declaration
	     * \brief Unlike, function or var decl, this will not create any generator, but just check the integrity of the enum
	     * \return An EnumRef
	     */
	    generator::Generator validateEnum (const semantic::Symbol & en); 

	    /**
	     * \brief Validate a struct declaration
	     * \brief unlike, function or var decl, this will not create any generator, but just check the integrity of the structure
	     * \return A StructRef
	     */
	    generator::Generator validateStruct (const semantic::Symbol & str);


	    /**
	     * \brief Validate a class declaration
	     * \brief unlike function or vardel, this will not create any generator, but just check the integrity of the the class, and generate all the inner function of the class
	     * \return A ClassRef
	     */
	    generator::Generator validateClass (const semantic::Symbol & cls);
	    
	    /**
	     * \brief validate an expression, that produce a type
	     * \return a generator containing the resulting type
	     * \param lock if the return type is not mutable do we lock the mutability ?
	     * \      if true, even if we set it to mutable later all the inner types will be immutable
	     */
	    generator::Generator validateType (const syntax::Expression & type, bool lock);

	    /**
	     * \brief validate an expression, that produce a type
	     * \return a generator containing the resulting type
	     */
	    generator::Generator validateType (const syntax::Expression & type);

	    /**
	     * \brief Validate a decorated type
	     */
	    generator::Generator validateTypeDecorated (const syntax::DecoratedExpression & type);
	    
	    /**
	     * \brief Validate the var using it as a type
	     */
	    generator::Generator validateTypeVar (const syntax::Var & var);

	    /**
	     * \brief Validate the unary using it as a type (for &() for example)
	     */
	    generator::Generator validateTypeUnary (const syntax::Unary & var);

	    /**
	     * \brief Validate an array allocation as a type
	     */
	    generator::Generator validateTypeArrayAlloc (const syntax::ArrayAlloc & alloc);

	    /**
	     * \brief Validate a slice type from an array literal
	     */
	    generator::Generator validateTypeSlice (const syntax::List & array);
	    
	    /**
	     * \brief Validate a tuple type from a tuple literal
	     */
	    generator::Generator validateTypeTuple (const syntax::List & tuple);

	    /**
	     * \brief Validate a tuple type from a template syntax list
	     */
	    generator::Generator validateTypeTupleTemplate (const generator::TemplateSyntaxList & tuple);
	    
	    /**
	     * \brief Validate a template call type from a template call literal
	     */
	    generator::Generator validateTypeTemplateCall (const syntax::TemplateCall & tmpCall);

	    /**
	     * \brief Validate a template checker 
	     */
	    generator::Generator validateTemplateChecker (const syntax::TemplateChecker & tmpChecker);

	    /**
	     * \brief Validate a throw expression
	     */
	    generator::Generator validateThrow (const syntax::Throw & thr);

	    /**
	     * \brief Validate a match expression
	     */
	    generator::Generator validateMatch (const syntax::Match & matcher);

	    /**
	     * \brief Validate a catch (this function must be called only if this catch is out from any scope)
	     * \param cat the catcher to validate
	     */
	    generator::Generator validateCatchOutOfScope (const syntax::Catch & cat);


	    /**
	     * \brief Validate a scope guard (this function must be called only if this scope is out from any scope)
	     * \param sc the scope guard to validate
	     */
	    generator::Generator validateScopeOutOfScope (const syntax::Scope & sc);
	    
	    /**
	     * \return the typeinfo of the type type
	     */
	    generator::Generator validateTypeInfo (const lexing::Word & loc, const generator::Generator & type);

	    /**
	     * \return the name of the field in TypeIDs for the type type
	     */
	    static std::string typeInfoName (const generator::Generator & type);
	    
	    /**
	     * \brief Validate a template test, a template test must be executed from the context of the template
	     * \param context the context of the template definition
	     * \param expr the test of the template
	     * \return a generator (not cte, just validated)
	     */
	    generator::Generator validateTemplateTest (const Symbol & context, const syntax::Expression & expr);
	    
	    /**
	     * \brief validate an expression, that produce a value
	     * \brief If the value is a breaker or a returner throw an error
	     * \return a tree containing the result of the value
	     */
	    generator::Generator validateValue (const syntax::Expression & value, bool canBeType = false, bool fromCall = false);

	    /**
	     * \brief Validate an compile time expression , that produce a value
	     * \brief If the value is a breaker or a returner throw an error
	     * \return a tree containing the result of the value
	     */
	    generator::Generator validateCteValue (const syntax::Expression & value);
	    
	    /**
	     * \brief validate an expression, that produce a value
	     * \return a tree containing the result of the value
	     */
	    generator::Generator validateValueNoReachable (const syntax::Expression & value, bool fromCall = false);

	    /**
	     * \brief Validate a block of expression
	     * \return a tree containing the result of the block
	     */
	    generator::Generator validateBlock (const syntax::Block & block);

	    /**
	     * \brief Validate a catch block
	     * \brief return by ref the vardecls and actions to perform in case of catch
	     * \brief All the actions must return the type type (if type !is void)
	     */
	    void validateCatchers (const syntax::Expression & catcher, std::vector <generator::Generator> & varDecl, std::vector <generator::Generator> & typeInfos, std::vector <generator::Generator> & actions, const generator::Generator& type);
	    
	    
	    /**
	     * \brief Validate an inner declaration of a module
	     * \param decl the module to declare, and validate
	     */
	    semantic::Symbol validateInnerModule (const syntax::Declaration & decl);

	    /**
	     * \brief Validate a symbol created by templateSpecialization
	     */
	    void validateTemplateSymbol (const semantic::Symbol & sym);
	    
	    /**
	     * \brief Validate a set of expression
	     * \return a tree containing the result of the block
	     */
	    generator::Generator validateSet (const syntax::Set & set);
	    
	    /**
	     * \brief Validate an fixed const integer value
	     */
	    generator::Generator validateFixed (const syntax::Fixed & fixed, int base = 0);	    

	    
	    /**
	     * \brief Validate a const bool value
	     */
	    generator::Generator validateBool (const syntax::Bool & b);

	    /**
	     * \brief Validate a const floating value
	     */
	    generator::Generator validateFloat (const syntax::Float & f);

	    /**
	     * \brief Validate a literal char
	     */
	    generator::Generator validateChar (const syntax::Char & c);

	    /**
	     * \brief Validate a string literal
	     */
	    generator::Generator validateString (const syntax::String & s);
	    
	    /**
	     * \brief Validate a binary expression 
	     * \brief This generation is a bit complex as it depends on the type of the operands
	     * \brief All binary operations are handled by BinaryVisitor class
	     */
	    generator::Generator validateBinary (const syntax::Binary & bin, bool isFromCall = false);

	    /**
	     * \brief Validate a unary expression
	     * \brief All unary operations are handled by UnaryVisitor class
	     */
	    generator::Generator validateUnary (const syntax::Unary & un);
	    
	    /**
	     * \brief Validate a var 
	     * \brief It will check all the local reference
	     * \brief If no local reference are found, it will find the symbol inside the table of the current frame
	     * \brief And then produce the generator for this symbol
	     */
	    generator::Generator validateVar (const syntax::Var & var);

	    /**
	     * \brief Transform global extern symbol into valid generators
	     * \param loc the location of the reference to those symbols
	     * \param multSym the list of symbols
	     * \param fromTemplate allow from template frame prototype
	     */
	    generator::Generator validateMultSym (const lexing::Word & loc, const std::vector <Symbol> & multSym);

	    /**
	     * \brief Transform global extern symbol into valid generators
	     * \param loc the location of the reference to those symbols
	     * \param multSym the list of symbols
	     */
	    generator::Generator validateMultSymType (const lexing::Word & loc, const std::vector <Symbol> & multSym);  
	    
	    /**
	     * \brief Validate the prototype of a function in order to refer to it
	     * \param func the function prototype to validate
	     */
	    generator::Generator validateFunctionProto (const semantic::Function & func);
	    
	    /**
	     * \brief Validate a var declaration inside a block (or a frame)
	     * \param needValue Is the value mandatory in this var declaration?
	     */
	    generator::Generator validateVarDeclValue (const syntax::VarDecl & decl, bool needValue = true);
	    
	    /**
	     * \brief Validate a decorated expression
	     */
	    generator::Generator validateDecoratedExpression (const syntax::DecoratedExpression & dec_expr);

	    /**
	     * \brief Validate an if expression 
	     * \return as always a generator 
	     * \warning the generator is not always a if expression, for optimization purpose (such as the test is always false, ...)
	     */
	    generator::Generator validateIfExpression (const syntax::If & _if);


	    /**
	     * \brief Validate an if expression and evaluate it at compile time
	     * \return as always a generator 
	     */
	    generator::Generator validateCteIfExpression (const syntax::If & _if);
	    
	    /**
	     * \brief Validate a while expression 
	     * \return as always a generator 
	     */
	    generator::Generator validateWhileExpression (const syntax::While & _wh);

	    /**
	     * \brief Validate a for expression
	     * \return as always a generator
	     */
	    generator::Generator validateForExpression (const syntax::For & _for);
	    
	    /**
	     * \brief Validate a break expression
	     */
	    generator::Generator validateBreak (const syntax::Break & br);

	    /**
	     * \brief Validate a return expression
	     */
	    generator::Generator validateReturn (const syntax::Return & rt);
	    
	    /**
	     * \brief Validate a list, it could be either : 
	     * \brief - an array
	     * \brief - a tuple
	     * \brief - an array type
	     * \brief - a tuple type
	     */
	    generator::Generator validateList (const syntax::List & list);

	    /**
	     * \brief Validate a template generated list of generator (it will create a tuple)
	     */
	    generator::Generator validateListTemplate (const generator::TemplateSyntaxList & list);

	    /**
	     * \brief Validate a cast expression
	     */
	    generator::Generator validateCast (const syntax::Cast & cast);

	    /**
	     * \brief Validate an array alloc
	     */
	    generator::Generator validateArrayAlloc (const syntax::ArrayAlloc & alloc);

	    /**
	     * \brief Validate a destruct declaration
	     */
	    generator::Generator validateDestructDecl (const syntax::DestructDecl & decl);

	    /**
	     * \brief Validate a lambda function	     
	     */
	    generator::Generator validateLambda (const syntax::Lambda & lmbd);

	    /**
	     * \brief Validate a lambda proto that is validated by calling it
	     */
	    generator::Generator validateLambdaProto (const generator::LambdaProto & lmbd, const std::vector <generator::Generator> & types);

	    /**
	     * \brief Validate a mult sym proto, 
	     */
	    generator::Generator validateMultSymProto (const generator::Generator & sym, const std::vector <generator::Generator> & types);	    
	    
	    /**
	     * \brief Create the closure value, that will be passed in the delegate value 
	     * \brief A closure value is a copy or a ref to all the enclosed variable in a tuple value that is then copied to heap	     
	     */
	    generator::Generator validateClosureValue (const generator::Generator & closureType, bool isrefClosure, uint closureIndex);
	    
	    /**
	     * \brief Validate a func ptr type (fn or dg) into generate type
	     */
	    generator::Generator validateFuncPtr (const syntax::FuncPtr & ptr);

	    /**
	     * \brief Validate a null value
	     */
	    generator::Generator validateNullValue (const syntax::Null & nl);
	    
	    /**
	     * \brief Validate intricisics, it could be either :  
	     * \brief - a copy
	     * \brief - an expand
	     * \brief - type informations ...
	     */
	    generator::Generator validateIntrinsics (const syntax::Intrinsics & intr);
	    
	    /**
	     * \brief Validate the copy intrinsics
	     */
	    generator::Generator validateCopy (const syntax::Intrinsics & intr);

	    /**
	     * \brief Validate the deep copy intrinsics
	     */
	    generator::Generator validateDeepCopy (const syntax::Intrinsics & intr);

	    /**
	     * \brief Validate the alias intrinsics
	     */
	    generator::Generator validateAlias (const syntax::Intrinsics & intr);

	    /**
	     * \brief Validate the expand intrinsics
	     */
	    generator::Generator validateExpand (const syntax::Intrinsics & intr);
	    
	    /**
	     * \brief Validate a mult operator
	     * \brief A mult operator is an operator with one left operand and multiple right operand
	     * \brief It can be either :
	     * \brief - Brackets
	     * \brief - Parentheses
	     */
	    generator::Generator validateMultOperator (const syntax::MultOperator & mult);

	    
	    /**
	     * \brief validate an array literal
	     */
	    generator::Generator validateArray (const syntax::List & list);

	    /**
	     * \brief validate an tuple literal
	     */
	    generator::Generator validateTuple (const syntax::List & list);

	    /**
	     * \brief Validate a template call expression
	     */
	    generator::Generator validateTemplateCall (const syntax::TemplateCall & cl);
	    
	    
	    /**
	     * \return the list of generator produced by semantic validation
	     */
	    const std::vector <generator::Generator> & getGenerators () const;

	    /**
	     * \brief this function is called each time a copy is performed
	     * \param loc the location of the affectation
	     * \param type the type result of the copy 
	     * \param gen the generator that will produce the affectation 
	     * \param construct is this a construction ? (ref are not affected yet)
	     * \brief This function verify that the mutability of gen is preserved
	     * \brief And that no implicit operation are performed
	     */
	    void verifyMemoryOwner (const lexing::Word & loc, const generator::Generator & type, const generator::Generator & gen, bool construct);


	    /**
	     * \brief Verify that if the type must be copied or aliased, the generator gives an explicitly alias construction
	     */
	    void verifyImplicitAlias (const lexing::Word & loc, const generator::Generator & type, const generator::Generator & gen);

	    /**
	     * \brief Applicable to the type of a vardecl, 
	     * \param decos the decorators of the variable (and not of the type)
	     * \param type the type of the var declaration
	     * \return isRef ref decorator found
	     * \return isMutable mut decorator found
	     */
	    void applyDecoratorOnVarDeclType (const std::vector <syntax::DecoratorWord> & decos, generator::Generator & type, bool & isRef, bool & isMutable);

	    /**
	     * \brief Verify that a parameter (of a function, or a for loop or ...) is mutable iif it is a reference
	     * \brief There is some exception, you must call this function to verify the correct behavior
	     * \param loc the location of the variable declaration
	     * \param type the type of the var
	     * \param error the error to throw, (in case of error) 
	     */
	    void verifyMutabilityRefParam (const lexing::Word & loc, const generator::Generator & type, Ymir::ExternalErrorValue error);

	    /**
	     * \brief Throw an exception if left.equals (right) is false
	     * \param left a type
	     * \param rigth a type
	     */
	    void verifySameType (const generator::Generator & left, const generator::Generator & right);

	    /**
	     * \brief Throw an exception if left.isCompatible (right) is false
	     * \param left a type
	     * \param right a type
	     */
	    void verifyCompatibleType (const generator::Generator & left, const generator::Generator & right);

	    
	    /**
	     * \brief Throw an exception if left.isCompatible (right.getType ()) is false
	     * \param left a type
	     * \param right a value
	     */
	    void verifyCompatibleTypeWithValue (const generator::Generator & left, const generator::Generator & right);

	    /**
	     * \brief Throw an exception if there is already some var named name
	     * \param name the forbidden name
	     */
	    void verifyShadow (const lexing::Word & name);

	    /**
	     * \brief Throw an exception if name refer to a primitive type
	     * \param the possibly forbidden name
	     */
	    void verifyNotIsType (const lexing::Word & name);

	    /**
	     * \brief Throw an exception if we are currently in a safe context
	     * \param location the location of the unsafe operation
	     */
	    void verifySafety (const lexing::Word & location) const;
	    
	public :

	    /** 
	     * \brief Enter a foreign definition 
	     * \brief all the symbol currently declared in local block won't be accessible until we exit the foreign
	     */
	    void enterForeign ();

	    /**
	     * \brief exit the foreign (Cf. enterForeign)
	     */
	    void exitForeign ();
	    
	    /**
	     * \brief Enter a new closure
	     */
	    void enterClosure (bool isRefClosure, uint refId, uint index);

	    /**
	     * \brief Quit a closure
	     * \returns all the enclosed vars
	     */
	    generator::Generator exitClosure ();
	    
	    /**
	     * \brief enter a new context, 
	     * \brief A context is a list of Cas (only used in a function)
	     * \brief This list define the behavior that are not allowed
	     */
	    void enterContext (const std::vector <lexing::Word> & Cas);

	    /**
	     * \brief Exit the context (Cf. enterContext)
	     */
	    void exitContext ();
	    
	    /**
	     * \brief Insert a new Generator that has passed the semantic validation
	     * \brief All the symbol passed here, will be transformed at generation time
	     * \param generator the valid generator
	     */
	    void insertNewGenerator (const generator::Generator & generator);

	    /**
	     * \brief Enter a new scope
	     */
	    void enterBlock ();

	    /**
	     * \brief insert a new symbol in the frame local scope
	     * \param name the name of the symbol
	     * \param local the symbol 
	     */
	    void insertLocal (const std::string & name, const generator::Generator & local);

	    /**
	     * \brief Get a localy declared symbol
	     * \param name the name of the symbol to get
	     */
	    generator::Generator getLocal (const std::string & name, bool canBeInClosure = true) ;

	    /**
	     * \brief Exit a scope
	     */
	    void quitBlock ();

	    /**
	     * \brief Ignore all the local var declared in the current block
	     * \return all the local var discarded
	     */
	    std::map <std::string, generator::Generator> discardAllLocals ();
	    
	    /** 
	     * Enter a new breakable loop 
	     */
	    void enterLoop ();
	    
	    /** 
	     * quit a breakable loop
	     * \return the type of the inner breakers
	     */
	    generator::Generator quitLoop ();
	    
	    /**
	     * \return the loop type
	     */
	    const generator::Generator & getCurrentLoopType () const;

	    /**
	     * \brief Change the type of the current loop
	     */
	    void setCurrentLoopType (const generator::Generator & type);

	    /**
	     * \brief Return the type of the current fn
	     */
	    generator::Generator getCurrentFuncType () ;

	    /**
	     * \brief Set the type of the current fn
	     */
	    void setCurrentFuncType (const generator::Generator & type);
	    
	    /**
	     * \return !this-> _loopBreakTypes.empty ()
	     */
	    bool isInLoop () const;

	    /**
	     * \return true if we are validating a closure
	     */
	    bool isInClosure ();

	    /**
	     * \return true if we are validating a closure and it is a ref one
	     */
	    bool isInRefClosure ();

	    /**
	     * \brief Get a variable in the encosed element
	     */
	    generator::Generator getInClosure (const std::string & name);
	    
	    /**
	     * \brief execute the content of the generator in order to retreive the compile time value 
	     */
	    generator::Generator retreiveValue (const generator::Generator & gen);

	    /**
	     * \brief Retreive a globally declared symbol (outside of the frame, or inner declared but not local just private)
	     * \param name the name of the symbol to retreive
	     */
	    std::vector <Symbol> getGlobal (const std::string & name);


	    /**
	     * \brief Try to retreive the frame from the prototype
	     * \brief Basically a by name search
	     * \warning can return an empty generator, if the frame was not found
	     */
	    const generator::Generator & retreiveFrameFromProto (const generator::FrameProto & proto);

	    /**
	     * \brief Tell if an expression use alone has any effect, or if it is just a lost of computation power
	     * \param gen the expression to test
	     */
	    bool isUseless (const generator::Generator & gen);

	    /**
	     * \brief insert a new prototype
	     * \return false if the proto already exists, true otherwise
	     */
	    bool insertTemplateSolution (const semantic::Symbol & sol);

	    /**
	     * \brief this-> _referent.push_back (sym)
	     */
	    void pushReferent (const semantic::Symbol & sym);

	    /**
	     * \brief this-> _referent.pop_back ()
	     */
	    void popReferent ();
	    
	private :

	    void verifyRecursivity (const lexing::Word & loc, const generator::Generator & gen, const Symbol & sym) const;

	    void printLocal () const;
	    
	};
	       
    }
    
}
