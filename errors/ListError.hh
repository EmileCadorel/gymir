#pragma once

#include <ymir/utils/StringEnum.hh>

namespace Ymir {

    /**
     * \enum ErrorCode
     * This enum is used for exception handling
     */
    enum class ErrorCode : int {
	INTERNAL = 0, // internal error, due to compiler problem
	EXTERNAL = 1, // external error, due to source code error
	FATAL = 2, // fatal external error
	FAIL = 3 // fail error, too many errors for example, the compilation must stop
    };    

    /**
       \enum ErrorType
       This enumaration lists all the error types 
       We cannot use C++ enum, as it does not allows to have string values
    */
    DECLARE_ENUM_HEADER (ExternalError, std::string,
			 ADDR_MIGHT_THROW,
			 AFFECT_COMPILE_TIME,
			 ALIAS_NO_EFFECT,
			 ALLOC_ABSTRACT_CLASS,
			 ASSERT_FAILED,
			 BRANCHING_VALUE,
			 BREAK_INSIDE_EXPR,
			 BREAK_NO_LOOP,
			 CALL_RECURSION,
			 CANDIDATE_ARE,
			 CANNOT_BE_CTE,
			 CANNOT_OVERRIDE_AS_PRIVATE,
			 CANNOT_OVERRIDE_FINAL,
			 CANNOT_OVERRIDE_NON_TRAIT_IN_IMPL,
			 CANNOT_OVERRIDE_TRAIT_OUTSIDE_IMPL, 
			 CATCH_MULTIPLE_TIME,
			 CATCH_OUT_OF_SCOPE,
			 COMPILATION_END,
			 COMPILE_TIME_UNKNOWN,
			 CONDITIONAL_NON_TEMPLATE_CLASS,
			 CONDITIONAL_NON_TEMPLATE_TRAIT,
			 CONFLICTING_DECLARATIONS,
			 CONFLICT_DECORATOR,
			 DECLARED_PROTECTION,
			 DECL_VARIADIC_FUNC,
			 DECO_OUT_OF_CONTEXT,
			 DISCARD_CONST,
			 DISCARD_CONST_LEVEL,
			 DISCARD_CONST_LEVEL_TEMPLATE,
			 DOC_FILE_ERROR,
			 DOLLAR_OUSIDE_CONTEXT,
			 DYNAMIC_CAST_FAILED,
			 ENUM_EMPTY,
			 EN_NO_VALUE,
			 EXTERNAL_VAR_DECL_WITHOUT_TYPE,
			 EXTERNAL_VAR_WITH_VALUE,
			 FAILURE_NO_THROW,
			 FIELD_NO_DEFAULT,
			 FORGET_TOKEN,
			 FORWARD_REFERENCE_VAR,
			 GLOBAL_VAR_DECL_WITHOUT_VALUE,
			 GLOBAL_VAR_DEPENDENCY, 
			 IMMUTABLE_LVALUE,
			 IMPLICIT_ALIAS,
			 IMPLICIT_OVERRIDE,
			 IMPLICIT_OVERRIDE_BY_TRAIT,
			 IMPLICIT_REFERENCE,
			 IMPL_NO_TRAIT,
			 IMPOSSIBLE_EXTERN,
			 INCOMPATIBLE_TOKENS,
			 INCOMPATIBLE_TYPES,			 
			 INCOMPATIBLE_VALUES,
			 INCOMPLETE_TEMPLATE,
			 INCOMPLETE_TYPE,
			 INCOMPLETE_TYPE_CLASS,
			 INFINITE_CONSTRUCTION_LOOP,
			 INHERIT_FINAL_CLASS,
			 INHERIT_NO_CLASS,
			 INSERT_NO_TABLE,
			 INVALID_MACRO_EVAL,
			 INVALID_MACRO_RULE,
			 IN_BLOCK_OPEN,
			 IN_COMPILE_TIME_EXEC,
			 IN_IMPORT,
			 IN_MACRO_EXPANSION,
			 IN_MATCH_DEF,
			 IN_TEMPLATE_DEF,
			 IN_TRAIT_VALIDATION,
			 IS_TYPE,
			 LOCAL_FIELD_OFFSET_OUT_CLASS,
			 LOCAL_TUPLEOF_OUT_CLASS,
			 LOCKED_CONTEXT,
			 MACRO_MULT_NO_VAR,
			 MACRO_REST,
			 MAIN_FUNCTION_ONE_ARG,
			 MALFORMED_CHAR,
			 MALFORMED_PRAGMA, 
			 MATCH_CALL,			 
			 MATCH_FINAL_NO_DEFAULT,
			 MATCH_NO_DEFAULT,
			 MATCH_PATTERN_CLASS,
			 MISMATCH_ARITY,
			 MONITOR_NON_CLASS,
			 MOVE_ONLY_CLOSURE,
			 MULTIPLE_CATCH,
			 MULTIPLE_DESTRUCTOR,
			 MULTIPLE_FIELD_INIT,
			 MUST_ESCAPE_CHAR, 
			 MUTABLE_CONST_ITER,
			 MUTABLE_CONST_PARAM,
			 NEVER_USED,
			 NOTHING_TO_CATCH,
			 NOT_ABSTRACT_NO_OVER,
			 NOT_AN_ALIAS,
			 NOT_A_CLASS,
			 NOT_A_LVALUE,
			 NOT_A_STRUCT,
			 NOT_CATCH,
			 NOT_CTE_ITERABLE,
			 NOT_IMPL_TRAIT,
			 NOT_ITERABLE,
			 NOT_ITERABLE_WITH,
			 NOT_OVERRIDE,
			 NO_ALIAS_EXIST,
			 NO_BODY_METHOD,
			 NO_COPY_EXIST,
			 NO_SIZE_FORWARD_REF,
			 NO_SUCH_FILE,
			 NO_SUPER_FOR_CLASS,
			 NULL_PTR,
			 OF, 
			 OTHER_ERRORS,
			 OVERFLOW,
			 OVERFLOW_ARITY,
			 OVERFLOW_ARRAY,
			 OVERRIDE_MISMATCH_PROTECTION,
			 OVERRIDE_PRIVATE,
			 PACKED_AND_UNION,
			 PARAMETER_NAME,
			 PRIVATE_IN_THIS_CONTEXT,
			 PROTECTION_NO_IMPACT,
			 REF_NO_EFFECT,
			 REF_NO_VALUE,
			 REF_RETURN_TYPE,
			 REF_SELF,
			 RESERVED_RULE_NAME,
			 RETHROW_NOT_MATCHING_PARENT,
			 RETURN_INSIDE_EXPR,
			 RETURN_NO_FRAME,
			 SCOPE_OUT_OF_SCOPE,
			 SHADOWING_DECL,
			 SPACE_EXTERN_C,
			 SPECIALISATION_WORK_TYPE_BOTH,
			 SPECIALISATION_WORK_WITH_BOTH,
			 SPECIALISATION_WORK_WITH_BOTH_PURE,
			 SYNTAX_ERROR_AT,
			 SYNTAX_ERROR_AT_SIMPLE,
			 SYNTAX_ERROR_IF_ON_NON_TEMPLATE,
			 SYNTAX_ERROR_MISSING_TEMPL_PAR,
			 TEMPLATE_IN_TRAIT,
			 TEMPLATE_RECURSION,
			 TEMPLATE_REST,
			 TEMPLATE_TEST_FAILED,
			 TEMPLATE_VALUE_TRY,
			 THROWS,
			 THROWS_IN_LAMBDA,
			 THROWS_NOT_DECLARED,
			 THROWS_NOT_USED,
			 TRAIT_NO_METHOD,
			 TYPE_NO_FIELD,
			 UFC_REWRITING,
			 UNDEFINED_BIN_OP,
			 UNDEFINED_BRACKETS_OP,
			 UNDEFINED_CA,
			 UNDEFINED_CALL_OP,
			 UNDEFINED_CAST_OP,
			 UNDEFINED_ESCAPE,
			 UNDEFINED_FIELD_FOR,
			 UNDEFINED_MACRO_OP,
			 UNDEFINED_SCOPE_GUARD,
			 UNDEFINED_SUB_PART_FOR,
			 UNDEFINED_TEMPLATE_OP,
			 UNDEFINED_UN_OP,
			 UNDEF_MACRO_EVAL,
			 UNDEF_MACRO_EVAL_IN,
			 UNDEF_TYPE,
			 UNDEF_VAR,
			 UNINIT_FIELD,
			 UNION_CST_MULT,
			 UNION_INIT_FIELD,
			 UNKNOWN_LAMBDA_TYPE,
			 UNKNOWN_OPTION_NAME,
			 UNKOWN_PRAGMA,
			 UNREACHBLE_STATEMENT,
			 UNRESOLVED_TEMPLATE,
			 UNTERMINATED_SEQUENCE,			 
			 UNTRUSTED_CONTEXT,
			 UNUSED_MATCH_CALL_OP,
			 USELESS_CATCH,
			 USELESS_DECORATOR,
			 USELESS_EXPR,
			 USE_AS_TYPE,
			 USE_AS_VALUE,
			 USE_UNIT_FOR_VOID,
			 VALIDATING,
			 VAR_DECL_IN_TRAIT,
			 VAR_DECL_WITHOUT_VALUE,			 
			 VAR_DECL_WITH_NOTHING,
			 VOID_VALUE,
			 VOID_VAR,
			 VOID_VAR_VALUE,
			 WRONG_IMPLEMENT,
			 WRONG_MODULE_NAME,
    );
    
    // 	    SYNTAX_ERROR_FOR = "%%% unexpected when analysing statement %%%",
    // 	    SYNTAX_ERROR_SIMPLE = "%%% unexpected",
    // 	    LAST_ERROR
    // 	    };
        
}
