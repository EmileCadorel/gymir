#pragma once

#include "Declaration.hh"
#include "Expression.hh"
#include "../errors/_.hh"
#include "../semantic/_.hh"
#include "../syntax/Word.hh"
#include <ymir/utils/Array.hh>

namespace syntax {
       
    enum class MacroVarConst {
	EXPR,
	IDENT,
	BLOCK
    };

    class IMacroElement : public IExpression {
    public: 

	IMacroElement (Word ident) : IExpression (ident) {}
	
	virtual IMacroElement* clone () = 0;
	
    };
    
    class IMacroExpr {	
	std::vector <IMacroElement*> elements;
    public:
	IMacroExpr (Word begin, Word end, std::vector <IMacroElement*> elements);

	std::vector <IMacroElement*> & getElements ();
	
    };

    class IMacroToken : public IMacroElement {
	std::string value;
    public:
	IMacroToken (Word tok, std::string value);
	
	IMacroToken* clone () override;

	std::string & getValue ();
	
	static const char* id ();
	
	std::vector <std::string> getIds () override;
    };

    
    class IMacroRepeat : public IMacroElement {
	IMacroExpr* content;
	IMacroToken * pass;
	bool oneTime;
    public :
	Word ident;
	
	IMacroRepeat (Word ident, IMacroExpr * content, IMacroToken * pass, bool atLeastOneTime);

	IMacroRepeat* clone () override;

	static const char* id ();

	std::vector <std::string> getIds () override;
       
    };
    
    class IMacroVar : public IMacroElement {

	MacroVarConst type;	
	Expression content;
	
    public:
	Word name;
	
	IMacroVar (Word name, MacroVarConst type);

	IMacroVar* clone () override;

	Expression templateExpReplace (const std::map <std::string, Expression> &) override;

	Expression expression ();
	
	MacroVarConst getType ();
	
	void setContent (Expression content);
	
	static const char* id ();

	std::vector <std::string> getIds () override;

    };
    

    class IMacro : public IDeclaration {
	Word ident;
	std::vector <IMacroExpr*> _exprs;
	std::vector <Block> _blocks;
	
    public:

	IMacro (Word ident, std::vector <IMacroExpr*> _exprs, std::vector <Block> _blocks);
	void declare () override;
	
	void declare (semantic::Module) override;
	
	void declareAsExtern (semantic::Module) override;
	
	std::vector <IMacroExpr*>& getExprs ();

	std::vector <Block>& getBlocks ();
	
    };

    class IMacroCall : public IExpression {
	Word end;
	Expression left;
	std::vector <Word> content;
	Block bl;
	Expression expr;
	
    public :

	IMacroCall (Word begin, Word end, Expression left, std::vector<Word> content);
	
	Expression expression () override;
	
	Expression templateExpReplace (const std::map <std::string, Expression>&) override;

	std::vector <Word> & getTokens ();	
	
	Ymir::Tree toGeneric () override;

	static const char * id () {
	    return TYPEID (IMacroCall);
	}
	
	std::vector <std::string> getIds () override;	
	
    };
	    
    typedef IMacro* Macro;
    typedef IMacroExpr* MacroExpr;
    typedef IMacroCall* MacroCall;
    typedef IMacroElement* MacroElement;
    typedef IMacroVar* MacroVar;
    typedef IMacroToken* MacroToken;
    typedef IMacroRepeat* MacroRepeat;
}

