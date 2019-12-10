#include <ymir/errors/ListError.hh>

namespace Ymir {    
    DECLARE_ENUM_WITH_TYPE (ExternalError, std::string,			         
			    AFFECT_COMPILE_TIME		    = "affectation at compile time is prohibited",
			    BORROWED_HERE		    = "borrow occured here",
			    BREAK_INSIDE_EXPR		    = "break here will result in an undefined behavior",
			    BREAK_NO_LOOP		    = "break statement must be placed inside a loop",
			    CALL_RECURSION                  = "limit of compile time call recursion reached %(y)", 
			    CANDIDATE_ARE                   = "candidate % : %",
			    CANNOT_BE_CTE                   = "cannot be compile time executed",
			    CATCH_MULTIPLE_TIME             = "type %(y) is catch multiple time",
			    COMPILATION_END		    = "",
			    COMPILE_TIME_UNKNOWN	    = "the value of this expression is required, but could not be known at compile time",
			    CONFLICT_DECORATOR		    = "conflicting decorator",
			    DECL_VARIADIC_FUNC              = "cannot declare a variadic function",
			    DECO_OUT_OF_CONTEXT		    = "decorator %(y) is unusable in this context",
			    DIRECT_COPY_STRING_LIT          = "cannot make a direct copy of a string literal, you must make an alias or a copy of it", 
			    DISCARD_CONST		    = "discard the constant qualifier is prohibited",
			    DISCARD_CONST_LEVEL		    = "discard the constant qualifier is prohibited, left operand mutability level is %(y) but must be at most %(y)",
			    DISCARD_LOCALITY		    = "discard the locality is prohibited",
			    DYNAMIC_CAST_FAILED		    = "dynamic cast failed : %",
			    EN_NO_VALUE                     = "%(y) declared in enum, but has no value",
			    FORWARD_REFERENCE_VAR           = "the type cannot be infered, as it depends on a forward reference",
			    IMMUTABLE_LVALUE		    = "left operand of type %(y) is immutable",
			    IMPLICIT_ALIAS                  = "implicit alias of type %(y) is not allowed, it will implicitly discard const qualifier", 
			    IMPLICIT_COPY		    = "implicit memory copy of type %(y) is not allowed",
			    IMPLICIT_REFERENCE		    = "implicit referencing of type %(y) is not allowed",
			    IMPOSSIBLE_EXTERN               = "impossible extern declaration",
			    INCOMPATIBLE_TYPES		    = "incompatible types %(y) and %(y)",
			    INCOMPATIBLE_VALUES             = "incompatible values",
			    INSERT_NO_TABLE		    = "insertion of a symbol in an entity that cannot be a referent",
			    IN_COMPILE_TIME_EXEC            = "in compile time execution",
			    IN_IMPORT                       = "in importation",
			    IN_TEMPLATE_DEF                 = "in template specialization",
			    IS_TYPE                         = "cannot declare var named %(y), it is a type",
			    MAIN_FUNCTION_ONE_ARG           = "main function take at most one argument",
			    MALFORMED_CHAR		    = "malformed literal, number of %(y) is %(y)",
			    MISMATCH_ARITY                  = "mismatch arity in tuple destructor (%(y)) and (%(y))",
			    MOVE_ONLY_CLOSURE               = "move is only usable for lambda closure",
			    MUTABLE_CONST_ITER		    = "an iterator cannot be mutable, if it is not a reference",
			    MUTABLE_CONST_PARAM		    = "a parameter cannot be mutable, if it is not a reference",
			    MUTABLE_CONST_RETURN	    = "a return value cannot be mutable, if it is not a reference",
			    NEVER_USED			    = "symbol %(y) declared but never used",
			    NOT_A_LVALUE		    = "not a lvalue",
			    NOT_ITERABLE                    = "%(y) type is not iterable",
			    NOT_ITERABLE_WITH               = "%(y) type is not iterable with %(y) vars",
			    NO_ALIAS_EXIST		    = "cannot alias type %(y)",
			    NO_COPY_EXIST		    = "no copy exists for type %(y)",
			    NO_SIZE_FORWARD_REF             = "field has no size because of forward reference",
			    NO_SUCH_FILE		    = "%(y) no such file, or permission denied",
			    NULL_PTR			    = "try to unref a null pointer",
			    OF                              = "of : %",
			    OTHER_CALL                      = "there are further calls, use option -v to show them",
			    OTHER_CANDIDATES                = "there are other candidates, use option -v to show them",
			    OVERFLOW			    = "overflow capacity for type %(y) = %(y)",
			    OVERFLOW_ARITY                  = "tuple access out of bound (%(y)), tuple arity is %(y)",
			    PACKED_AND_UNION		    = "structure could not be packed and union at the same time",
			    PARAMETER_NAME                  = "parameter % : %",
			    REF_NOT_TYPE		    = "use a reference as a value",
			    REF_NO_EFFECT		    = "the creation of ref has no effect on left operand",
			    REF_NO_VALUE		    = "%(y) declared as a reference but not initialized",
			    RETURN_INSIDE_EXPR		    = "return here will result in an undefined behavior", 
			    RETURN_LOCAL_REFERENCE	    = "expression does not live long enough",
			    RETURN_NO_FRAME                 = "return outside of function has no meaning",
			    SAFE_CONTEXT                    = "unsafe operation in safe context",
			    SHADOWING_DECL		    = "declaration of %(y) shadow another declaration",
			    SPACE_EXTERN_C                  = "extern C cannot be namespaced",
			    SPECIALISATION_WOTK_WITH_BOTH   = "%(y) called with {%(y)} work with both",
			    STATIC_ARRAY_REF		    = "left operand is a static array, it cannot store a reference to a dynamic array",
			    SYNTAX_ERROR_AT		    = "[%] expected, when %(y) found",	// syntax error a specific location
			    SYNTAX_ERROR_AT_SIMPLE	    = "%(y) unexpected",	// syntax error a specific location
			    SYNTAX_ERROR_IF_ON_NON_TEMPLATE = "test on non template declaration",
			    SYNTAX_ERROR_MISSING_TEMPL_PAR  = "multiple ! arguments are not allowed",
			    TEMPLATE_RECURSION              = "limit of template recursion reached %(y)",
			    TEMPLATE_TEST_FAILED            = "the test of the template failed with {%(y)} specialization",
			    UNDEFINED_BIN_OP		    = "undefined operator %(y) for types %(y) and %(y)",
			    UNDEFINED_BRACKETS_OP	    = "index operator is not defined for type %(y) and {%(y)}",
			    UNDEFINED_CA		    = "custom attribute %(y), does not exist in this context",
			    UNDEFINED_CALL_OP		    = "call operator is not defined for type %(y) and {%(y)}",
			    UNDEFINED_CAST_OP               = "undefined cast operator for type %(y) to type %(y)",
			    UNDEFINED_ESCAPE		    = "undefined escape sequence",
			    UNDEFINED_FIELD_FOR             = "undefined field %(y) for element %(y)",
			    UNDEFINED_SUB_PART_FOR          = "undefined sub symbol %(y) for element %(y)",
			    UNDEFINED_TEMPLATE_OP           = "undefined template operator for %(y) and {%(y)}",
			    UNDEFINED_SCOPE_GUARD           = "undefined scope guard %(y)",
			    UNDEFINED_UN_OP		    = "undefined operator %(y) for type %(y)",
			    UNDEF_TYPE			    = "undefined type %(y)",
			    UNDEF_VAR			    = "undefined symbol %(y)",
			    UNREACHBLE_STATEMENT	    = "unreachable statement",
			    UNRESOLVED_TEMPLATE             = "unresolved template",
			    UNTERMINATED_SEQUENCE	    = "unterminated escape sequence",
			    USELESS_DECORATOR		    = "useless decorator",
			    USELESS_EXPR                    = "expression has no effect",
			    USE_AS_TYPE			    = "expression used as type",
			    USE_AS_VALUE                    = "type expression used as value",
			    USE_UNIT_FOR_VOID               = "The block value cannot be void without a unit expression, maybe you forgot the ';'",
			    VAR_DECL_WITHOUT_VALUE	    = "var declaration must have an initial a value",
			    VAR_DECL_WITH_NOTHING           = "var declaration must at least have a type or a value",
			    VOID_VAR			    = "cannot declare var of type void",
			    WRONG_MODULE_NAME		    = "module named %(y) must be placed in file named %(y)",			    
    );    
}
