#include <ymir/errors/ListError.hh>

namespace Ymir {    
    DECLARE_ENUM_WITH_TYPE (ExternalError, std::string,
			    ADDR_MIGHT_THROW                = "a function pointer cannot be created from the %(y) function that could throw an exception", 
			    AFFECT_COMPILE_TIME		    = "assignment at compile time is prohibited",
			    ALIAS_NO_EFFECT                 = "the creation of an alias has no effect on the left operand",
			    ALLOC_ABSTRACT_CLASS            = "the class %(y) is declared abstract",
			    ASSERT_FAILED                   = "assertion failed : %(y)",
			    BREAK_INSIDE_EXPR		    = "the break statement placed here will result in undefined behavior",
			    BREAK_NO_LOOP		    = "the break statement must be placed inside a loop",
			    CALL_RECURSION                  = "the limit of the number of call recursions at compilation time has been reached %(y)", 
			    CANDIDATE_ARE                   = "candidate % : %",
			    CANNOT_BE_CTE                   = "cannot be compile time executed",
			    CANNOT_OVERRIDE_AS_PRIVATE      = "the private method %(y) cannot override the method of the ancestor",
			    CANNOT_OVERRIDE_FINAL           = "cannot override final method %(y)",
			    CATCH_MULTIPLE_TIME             = "type %(y) is caught multiple time",
			    CATCH_OUT_OF_SCOPE              = "catch used as an expression",
			    COMPILATION_END		    = "",
			    COMPILE_TIME_UNKNOWN	    = "the value of this expression is required, but could not be known at the time of compilation",
			    CONFLICTING_DECLARATIONS        = "those two declarations named %(y) shadow each other",
			    CONFLICT_DECORATOR		    = "conflicting decorator",
			    DECLARED_PROTECTION             = "declared %(y) here",
			    DECL_VARIADIC_FUNC              = "cannot declare a variadic function",
			    DECO_OUT_OF_CONTEXT		    = "the decorator %(y) is unusable in this context",
			    DISCARD_CONST		    = "discard the constant qualifier is prohibited",
			    DISCARD_CONST_LEVEL		    = "discard the constant qualifier is prohibited, left operand mutability level is %(y) but must be at most %(y)",
			    DISCARD_CONST_LEVEL_TEMPLATE    = "discard the constant qualifier is prohibited, left operand mutability level is %(y) but must be exactly %(y)",
			    DOLLAR_OUSIDE_CONTEXT           = "dollar operator without context",
			    DYNAMIC_CAST_FAILED		    = "dynamic cast failed : %",
			    EN_NO_VALUE                     = "%(y) was declared in an enumeration, but has no value",
			    FAILURE_NO_THROW                = "failure scope guard will never be triggered",
			    FORGET_TOKEN                    = "did you forget '%(y)' ?",
			    FORWARD_REFERENCE_VAR           = "the type cannot be infered, as it depends on a forward reference",
			    IMMUTABLE_LVALUE		    = "left operand of type %(y) is immutable",
			    IMPLICIT_ALIAS                  = "implicit alias of type %(y) is not allowed, it will implicitly discard constant qualifier", 
			    IMPLICIT_OVERRIDE               = "implicit override of method %(y) is not allowed",
			    IMPLICIT_REFERENCE		    = "implicit referencing of type %(y) is not allowed",
			    IMPL_NO_TRAIT                   = "impl statement must be followed by a trait, not %(y)",
			    IMPOSSIBLE_EXTERN               = "impossible extern declaration",
			    INCOMPATIBLE_TOKENS             = "incompatible tokens : [%(y)] and [%(y)]",
			    INCOMPATIBLE_TYPES		    = "incompatible types %(y) and %(y)",
			    INCOMPATIBLE_VALUES             = "incompatible values",
			    INCOMPLETE_TYPE                 = "the type %(y) is not complete",
			    INCOMPLETE_TYPE_CLASS           = "the type %(y) is not complete due to previous errors",
			    INFINITE_CONSTRUCTION_LOOP      = "infinite construction loop",
			    INHERIT_FINAL_CLASS             = "the base class %(y) is marked as final",
			    INHERIT_NO_CLASS                = "the base of a class must be a class, not a %(y)",
			    INSERT_NO_TABLE		    = "insertion of a symbol in an entity that cannot be a referent",
			    INVALID_MACRO_EVAL              = "invalid macro evaluation % in %",
			    INVALID_MACRO_RULE              = "%(y) is not a macro rule",
			    IN_COMPILE_TIME_EXEC            = "in compilation time execution",
			    IN_IMPORT                       = "in importation",
			    IN_MACRO_EXPANSION              = "in macro expansion",
			    IN_MATCH_DEF                    = "in pattern",
			    IN_TEMPLATE_DEF                 = "in template specialization",
			    IN_TRAIT_VALIDATION             = "in trait validation", 
			    IS_TYPE                         = "cannot declare a variable named %(y), identifier is already used for a type",
			    MACRO_MULT_NO_VAR               = "macro multiple expression must be identified inside a var",
			    MACRO_REST                      = "macro validation incomplete, there remains : [%(y)]",
			    MAIN_FUNCTION_ONE_ARG           = "main function takes at most one argument",
			    MALFORMED_CHAR		    = "malformed literal, number of %(y) is %(y)",
			    MATCH_CALL                      = "match call only works on classes or structures, not %(y)",
			    MATCH_FINAL_NO_DEFAULT          = "final match has no default match case",
			    MATCH_NO_DEFAULT                = "match of type %(y) has no default match case",
			    MATCH_PATTERN_CLASS             = "the internal patterns of a class pattern matching must be named expressions",
			    MISMATCH_ARITY                  = "mismatch arity in tuple destructor (%(y)) against (%(y))",
			    MOVE_ONLY_CLOSURE               = "the move qualifier is only usable for lambda closure",
			    MULTIPLE_CATCH                  = "%(y) is caught multiple times, this catcher will never be activated",
			    MULTIPLE_FIELD_INIT             = "the field %(y) is initialized multiple times",
			    MUST_ESCAPE_CHAR                = "an escape char must be used for '\n' '\r' or '\t'",
			    MUTABLE_CONST_ITER		    = "an iterator cannot be mutable, if it is not a reference",
			    MUTABLE_CONST_PARAM		    = "a parameter cannot be mutable, if it is not a reference",
			    NEVER_USED			    = "the symbol %(y) declared but never used",
			    NOTHING_TO_CATCH                = "nothing to catch",
			    NOT_ABSTRACT_NO_OVER            = "the class %(y) is not abstract, but does not override the empty parent method %(y)",
			    NOT_AN_ALIAS                    = "the type %(y) is not an aliasable type",
			    NOT_A_CLASS                     = "the type %(y) is not a class type",
			    NOT_A_LVALUE		    = "not a lvalue",
			    NOT_A_STRUCT                    = "type %(y) is not a struct type",
			    NOT_CATCH                       = "the exception %(y) might be thrown but is not caught",
			    NOT_IMPL_TRAIT                  = "the class %(y) does not implement trait %(y)",
			    NOT_ITERABLE                    = "the type %(y) is not iterable",
			    NOT_ITERABLE_WITH               = "the type %(y) is not iterable with %(y) vars",
			    NOT_OVERRIDE                    = "the method %(y) marked as override does not override anything",
			    NO_ALIAS_EXIST		    = "cannot alias type %(y)",
			    NO_BODY_METHOD                  = "the method %(y) has no body",
			    NO_COPY_EXIST		    = "no copy exists for type %(y)",
			    NO_SIZE_FORWARD_REF             = "the field has no size because of forward reference",
			    NO_SUCH_FILE		    = "%(y) no such file, or permission denied",
			    NO_SUPER_FOR_CLASS              = "cannot construct super for class %(y), it has no ancestor", 
			    NULL_PTR			    = "try to unref a null pointer",
			    OF                              = "of : %",
			    OTHER_CALL                      = "there are further calls, use option -v to show them",
			    OTHER_CANDIDATES                = "there are other candidates, use option -v to show them",
			    OVERFLOW			    = "overflow capacity for type %(y) = %(y)",
			    OVERFLOW_ARITY                  = "tuple access out of bound (%(y)), tuple arity is %(y)",
			    OVERFLOW_ARRAY                  = "array access out of bound (%(y)), array size is %(y)",
			    OVERRIDE_MISMATCH_PROTECTION    = "the protection %(y) of the overriden method %(y) does not match with the definition of the ancestor class",
			    OVERRIDE_PRIVATE                = "cannot override private method %(y)",
			    PACKED_AND_UNION		    = "structures could not be packed and union at the same time",
			    PARAMETER_NAME                  = "parameter % : %",
			    PRIVATE_IN_THIS_CONTEXT         = "%(B) : % is private within this context",
			    REF_NO_EFFECT		    = "the creation of ref has no effect on the left operand",
			    REF_NO_VALUE		    = "%(y) declared as a reference but not initialized",
			    REF_RETURN_TYPE                 = "cannot return a reference type",
			    REF_SELF                        = "cannot reference self",
			    RESERVED_RULE_NAME              = "%(y) rule name is reserved",
			    RETHROW_NOT_MATCHING_PARENT     = "rethrowing an exception %(y) that is not rethrowed by parent method", 
			    RETURN_INSIDE_EXPR		    = "return here will result in an undefined behavior", 
			    RETURN_NO_FRAME                 = "it doesn't make sense to return outside of a function",
			    SCOPE_OUT_OF_SCOPE              = "scope guard used as expression", 
			    SHADOWING_DECL		    = "declaration of %(y) shadows another declaration",
			    SPACE_EXTERN_C                  = "extern C cannot be namespaced",
			    SPECIALISATION_WORK_TYPE_BOTH   = "type match both",
			    SPECIALISATION_WORK_WITH_BOTH   = "%(y) called with {%(y)} work with both",
			    SYNTAX_ERROR_AT		    = "[%] expected, when [%(y)] found",	
			    SYNTAX_ERROR_AT_SIMPLE	    = "[%(y)] unexpected",	
			    SYNTAX_ERROR_IF_ON_NON_TEMPLATE = "test on a non-template declaration",
			    SYNTAX_ERROR_MISSING_TEMPL_PAR  = "multiple ! arguments are not allowed",
			    TEMPLATE_IN_TRAIT               = "cannot declare template inside a trait",
			    TEMPLATE_RECURSION              = "limit of template recursion reached %(y)",
			    TEMPLATE_REST                   = "template validation incomplete, rest : [%(y)]",
			    TEMPLATE_TEST_FAILED            = "the test of the template failed with {%(y)} specialization",
			    THROWS                          = "throws %(y)",
			    THROWS_IN_LAMBDA                = "a lambda function must be safe, but there are exceptions that are not caught",
			    THROWS_NOT_DECLARED             = "the function %(y) might throw an exception of type %(y), but that is not declared in its prototype",
			    THROWS_NOT_USED                 = "the function %(y) prototype informs about a possible throw of an exception of type %(y), but this is not true",
			    TRAIT_NO_METHOD                 = "the trait %(y) does not have any method %(y)",
			    UNDEFINED_BIN_OP		    = "undefined operator %(y) for types %(y) and %(y)",
			    UNDEFINED_BRACKETS_OP	    = "index operator is not defined for type %(y) and {%(y)}",
			    UNDEFINED_CA		    = "the custom attribute %(y) does not exist in this context",
			    UNDEFINED_CALL_OP		    = "the call operator is not defined for type %(y) and {%(y)}",
			    UNDEFINED_CAST_OP               = "undefined cast operator for type %(y) to type %(y)",
			    UNDEFINED_ESCAPE		    = "undefined escape sequence",
			    UNDEFINED_FIELD_FOR             = "undefined field %(y) for element %(y)",
			    UNDEFINED_MACRO_OP              = "undefined macro operator for element %(y)", 
			    UNDEFINED_SCOPE_GUARD           = "undefined scope guard %(y)",
			    UNDEFINED_SUB_PART_FOR          = "undefined sub symbol %(y) for element %(y)",
			    UNDEFINED_TEMPLATE_OP           = "undefined template operator for %(y) and {%(y)}",
			    UNDEFINED_UN_OP		    = "undefined operator %(y) for type %(y)",
			    UNDEF_MACRO_EVAL                = "undefined macro var %(y)",
			    UNDEF_MACRO_EVAL_IN             = "undefined macro var %(y) in {%}",
			    UNDEF_TYPE			    = "undefined type %(y)",
			    UNDEF_VAR			    = "undefined symbol %(y)",
			    UNINIT_FIELD                    = "the field %(y) has no initial value",
			    UNION_CST_MULT                  = "the construction of an union requires only one parameter",
			    UNION_INIT_FIELD                = "default field value of union has no sense",
			    UNKNOWN_PRAGMA                  = "unknown __pragma %(y)",
			    UNREACHBLE_STATEMENT	    = "unreachable statement",
			    UNRESOLVED_TEMPLATE             = "unresolved template",
			    UNTERMINATED_SEQUENCE	    = "unterminated escape sequence",
			    UNUSED_MATCH_CALL_OP            = "unused patterns {%(y)} in struct destructuring",
			    USELESS_CATCH                   = "useless catch of %(y), that cannot be thrown",  
			    USELESS_DECORATOR		    = "useless decorator",
			    USELESS_EXPR                    = "expression has no effect",
			    USE_AS_TYPE			    = "expression used as a type",
			    USE_AS_VALUE                    = "type expression used as a value",
			    USE_UNIT_FOR_VOID               = "The block value cannot be void without a unit expression, maybe you forgot the ';'",
			    VAR_DECL_WITHOUT_VALUE	    = "var declaration must have an initial value",
			    VAR_DECL_WITH_NOTHING           = "var declaration must at least have a type or a value",
			    VOID_VALUE                      = "value cannot be void",
			    VOID_VAR			    = "cannot declare a var of type void",
			    WRONG_MODULE_NAME		    = "the module named %(y) must be placed in a file named %(y)",
    );    
}
