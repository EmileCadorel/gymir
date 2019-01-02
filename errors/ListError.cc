#include <ymir/errors/ListError.hh>

namespace Ymir {    
    DECLARE_ENUM_WITH_TYPE (ExternalError, std::string,			         
			    SYNTAX_ERROR_AT = "[%] expected, when %(y) found", // syntax error a specific location
			    NO_SUCH_FILE = "%(y) no such file, or permission denied",
			    COMPILATION_END = ""
    );
}
