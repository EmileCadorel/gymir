#include "syntax/Keys.hh"
#include <ymir/utils/Version.hh>
#include <ymir/utils/Options.hh>

std::string	Keys::IMPORT	= "import";
std::string	Keys::STRUCT	= "struct";
std::string     Keys::UNION     = "union";
std::string     Keys::DEF	= "def";
std::string     Keys::IF	= "if";
std::string     Keys::RETURN	= "return";
std::string     Keys::FOR	= "for";
std::string     Keys::WHILE	= "while";
std::string     Keys::BREAK	= "break";
std::string     Keys::MATCH	= "match";
std::string     Keys::IN	= "in";
std::string     Keys::ELSE	= "else";
std::string     Keys::TRUE_	= "true";
std::string     Keys::FALSE_	= "false";
std::string     Keys::NULL_	= "null";
std::string     Keys::CAST	= "cast";
std::string     Keys::FUNCTION	= "fn";
std::string     Keys::DELEGATE	= "dg";
std::string     Keys::LET	= "let";
std::string     Keys::IS	= "is";
std::string     Keys::NOT_IS	= "!is";
std::string     Keys::ANTI	= "\\";
std::string     Keys::LX	= "x";
std::string     Keys::EXTERN	= "extern";
std::string     Keys::MAIN	= "main";
std::string     Keys::PUBLIC	= "public";
std::string     Keys::PRIVATE	= "private";
std::string     Keys::EXPAND	= "expand";
std::string     Keys::ENUM	= "enum";
std::string     Keys::UNDER	= "_";
std::string     Keys::OPBINARY	= "opBinary";
std::string     Keys::OPBINARYR = "opBinaryRight";
std::string     Keys::ASSERT	= "assert";
std::string     Keys::STATIC    = "static";
std::string     Keys::OPACCESS	= "opIndex";
std::string     Keys::OPAPPLYBEGIN   = "opIter";
std::string     Keys::OPAPPLYITER   = "next";
std::string     Keys::OPAPPLYGET   = "get";
std::string     Keys::OPAPPLYEND   = "isEnd";
std::string     Keys::OPRANGE	= "opRange";
std::string     Keys::OPTEST	= "opTest";
std::string     Keys::OPUNARY	= "opUnary";
std::string     Keys::OPEQUAL	= "opEquals";
std::string     Keys::OPCALL	= "opCall";
std::string     Keys::TYPEOF	= "typeof";
std::string     Keys::STRINGOF	= "stringof";
std::string     Keys::CONST	= "const";
std::string     Keys::IMMUTABLE = "cte";
std::string     Keys::MUTABLE   = "mut";
std::string     Keys::REF	= "ref";
std::string     Keys::PROTECTED = "protected";
std::string     Keys::MIXIN	= "mixin";
std::string     Keys::OF	= "of";
std::string     Keys::SELF	= "self";
std::string     Keys::OPASSIGN	= "opAssign";
std::string     Keys::TRAIT	= "trait";
std::string     Keys::IMPL	= "impl";
std::string     Keys::TUPLE	= "tuple";
std::string     Keys::OVER	= "over";
std::string     Keys::FROM	= "from";
std::string     Keys::DLANG	= "D";
std::string     Keys::CLANG      = "C";
std::string     Keys::CPPLANG      = "C++";
std::string     Keys::DPAR      = "()";
std::string     Keys::MOD       = "mod";
std::string     Keys::USE       = "use";
std::string     Keys::SCOPE     = "on";
std::string     Keys::PRAGMA    = "__pragma";
std::string     Keys::MACRO     = "__macro"; 
std::string     Keys::MACRO_IDENT     = "ident";
std::string     Keys::MACRO_EXPR      = "expr";
std::string     Keys::MACRO_BLOCK     = "block";
std::string     Keys::MACRO_TOKEN     = "token";
std::string     Keys::INLINE    = "inline";
std::string     Keys::PACKED    = "packed";
std::string     Keys::SAFE      = "safe";
std::string     Keys::NOGC      = "nogc";
std::string     Keys::FUNCTIONAL = "fnal";
std::string     Keys::TRUSTED    = "trusted";
std::string     Keys::PRE        = "pre";
std::string     Keys::POST       = "post";
std::string     Keys::BODY       = "body";
std::string     Keys::NEW        = "new";
std::string     Keys::ATTRIBUTE  = "attrib";
std::string     Keys::MOVE       = "move";
std::string     Keys::VERSION    = "version";
std::string     Keys::TYPE       = "type";
std::string     Keys::EXIT       = "exit";
std::string     Keys::INIT       = "init";
std::string     Keys::DELETE     = "delete";
std::string     Keys::VTABLE_FIELD = "0__vtbl";
std::string     Keys::SUPER      = "super";
std::string     Keys::DISPOSE    = "~self";

std::string Version::DEBUG = "debug";
std::string Version::UNIX = "unix";
std::string Version::x84_64 = "x86_64";
std::string Version::LINUX = "linux";
std::string Version::GNU_LINUX = "gnu_linux";
std::string Version::AMD64 = "amd64";


bool Version::isOn (const std::string & version) {
    if (version == Version::DEBUG)
	return Options::instance ().isDebug ();    
    return false;
}


