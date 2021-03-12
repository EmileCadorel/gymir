#include <ymir/syntax/visitor/Keys.hh>

const std::string       Keys::AKA          = "aka";
const std::string       Keys::ATOMIC       = "atomic";
const std::string	Keys::IMPORT	   = "import";
const std::string	Keys::STRUCT	   = "struct";
const std::string	Keys::ABSTRACT     = "abstract";
const std::string	Keys::ALIAS	   = "alias";
const std::string	Keys::ANTI	   = "\\";
const std::string	Keys::ASSERT	   = "assert";
const std::string	Keys::ATTRIBUTE	   = "attrib";
const std::string	Keys::BODY	   = "body";
const std::string	Keys::BREAK	   = "break";
const std::string	Keys::C16	   = "c16";
const std::string	Keys::C32	   = "c32";
const std::string	Keys::C8	   = "c8";
const std::string	Keys::CAST	   = "cast";
const std::string	Keys::CATCH	   = "catch";
const std::string	Keys::CLANG	   = "C";
const std::string	Keys::CLASS        = "class";
const std::string	Keys::COMPILE	   = "compile";
const std::string	Keys::CONST	   = "const";
const std::string	Keys::COPY	   = "copy";
const std::string	Keys::CPPLANG      = "C++";
const std::string	Keys::DCOPY	   = "dcopy";
const std::string	Keys::DEF	   = "def";
const std::string	Keys::DELEGATE	   = "dg";
const std::string	Keys::DLANG	   = "D";
const std::string	Keys::DMUTABLE	   = "dmut";
const std::string	Keys::DO	   = "do";
const std::string	Keys::DPAR	   = "()";
const std::string	Keys::DYNAMIC	   = "dynamic";
const std::string	Keys::ELSE	   = "else";
const std::string	Keys::ENUM	   = "enum";
const std::string	Keys::EXIT	   = "exit";
const std::string	Keys::EXPAND	   = "expand";
const std::string	Keys::EXTERN	   = "extern";
const std::string	Keys::FAILURE      = "failure";
const std::string	Keys::FALSE_	   = "false";
const std::string	Keys::FINAL_	   = "final";
const std::string	Keys::FLOAT_S	   = "f";
const std::string	Keys::FOR	   = "for";
const std::string	Keys::FROM	   = "from";
const std::string	Keys::FUNCTION	   = "fn";
const std::string	Keys::I16	   = "i16";
const std::string	Keys::I32	   = "i32";
const std::string	Keys::I64	   = "i64";
const std::string	Keys::I8	   = "i8";
const std::string	Keys::IF	   = "if";
const std::string	Keys::IMMUTABLE	   = "cte";
const std::string	Keys::IMPL	   = "impl";
const std::string	Keys::IN	   = "in";
const std::string	Keys::INIT	   = "init";
const std::string	Keys::INLINE	   = "inline";
const std::string	Keys::IS	   = "is";
const std::string	Keys::ISIZE	   = "is";
const std::string	Keys::LET	   = "let";
const std::string	Keys::LOOP	   = "loop";
const std::string	Keys::LX	   = "x";
const std::string	Keys::MACRO	   = "macro"; 
const std::string	Keys::MACRO_BLOCK  = "block";
const std::string	Keys::MACRO_EXPR   = "expr";
const std::string	Keys::MACRO_IDENT  = "ident";
const std::string	Keys::MACRO_TOKEN  = "token";
const std::string	Keys::MAIN	   = "main";
const std::string	Keys::MATCH	   = "match";
const std::string	Keys::MOD	   = "mod";
const std::string	Keys::MOVE	   = "move";
const std::string	Keys::MUTABLE	   = "mut";
const std::string	Keys::NEW	   = "new";
const std::string	Keys::NOGC	   = "nogc";
const std::string	Keys::NOT_IN	   = "!in";
const std::string	Keys::NOT_IS	   = "!is";
const std::string	Keys::NOT_OF	   = "!of";
const std::string	Keys::NULL_	   = "null";
const std::string	Keys::OF	   = "of";
const std::string	Keys::OPACCESS	   = "opIndex";
const std::string	Keys::OPAPPLYBEGIN = "opIter";
const std::string	Keys::OPAPPLYEND   = "isEnd";
const std::string	Keys::OPAPPLYGET   = "get";
const std::string	Keys::OPAPPLYITER  = "next";
const std::string	Keys::OPASSIGN	   = "opAssign";
const std::string	Keys::OPBINARY	   = "opBinary";
const std::string	Keys::OPBINARYR	   = "opBinaryRight";
const std::string	Keys::OPCALL	   = "opCall";
const std::string	Keys::OPEQUAL	   = "opEquals";
const std::string	Keys::OPRANGE	   = "opRange";
const std::string	Keys::OPTEST	   = "opTest";
const std::string	Keys::OPUNARY	   = "opUnary";
const std::string	Keys::OVER	   = "over";
const std::string	Keys::PACKED	   = "packed";
const std::string	Keys::POST	   = "out";
const std::string	Keys::PRAGMA	   = "__pragma";
const std::string	Keys::PRE	   = "in";
const std::string	Keys::PRIVATE	   = "prv";
const std::string	Keys::PROTECTED	   = "prot";
const std::string	Keys::PUBLIC	   = "pub";
const std::string	Keys::PURE	   = "pure";
const std::string	Keys::REF	   = "ref";
const std::string	Keys::RETURN	   = "return";
const std::string	Keys::S16	   = "s16";
const std::string	Keys::S32	   = "s32";
const std::string	Keys::S8	   = "s8";
const std::string	Keys::SCOPE	   = "on";
const std::string	Keys::SELF	   = "self";
const std::string	Keys::SELF_TILDE   = "self~";
const std::string	Keys::SIZEOF	   = "sizeof";
const std::string	Keys::SKIPS        = "skips";
const std::string	Keys::STATIC	   = "static";
const std::string	Keys::STRINGOF	   = "stringof";
const std::string	Keys::SUCCESS      = "success";
const std::string	Keys::SUPER	   = "super";
const std::string	Keys::TEMPLATE     = "is";
const std::string	Keys::THROWS	   = "throws";
const std::string	Keys::THROW_K	   = "throw";
const std::string	Keys::TRAIT	   = "trait";
const std::string	Keys::TRUE_	   = "true";
const std::string	Keys::TRUSTED	   = "trusted";
const std::string	Keys::TUPLE	   = "tuple";
const std::string	Keys::TYPEOF	   = "typeof";
const std::string	Keys::U16	   = "u16";
const std::string	Keys::U32	   = "u32";
const std::string	Keys::U64	   = "u64";
const std::string	Keys::U8	   = "u8";
const std::string	Keys::UNDER	   = "_";
const std::string	Keys::UNION	   = "union";
const std::string	Keys::USE	   = "use";
const std::string	Keys::USIZE	   = "us";
const std::string	Keys::VERSION	   = "__version";
const std::string	Keys::VTABLE_FIELD = "0__vtbl";
const std::string	Keys::WHILE	   = "while";
const std::string	Keys::WITH         = "with";
const std::string	Keys::YLANG	   = "Y";
const std::string	Keys::OPERATOR     = "operator";
const std::string	Keys::LEN          = "len";
const std::string       Keys::MANGLE       = "mangle";
const std::string       Keys::OK_          = "Ok";
const std::string       Keys::ERR_         = "Err";
