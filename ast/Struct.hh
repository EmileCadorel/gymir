#pragma once

#include "Declaration.hh"
#include "../errors/_.hh"
#include "../semantic/_.hh"
#include "../syntax/Word.hh"
#include <ymir/utils/Array.hh>

namespace syntax {

    class IStruct : public IDeclaration {
	
	Word ident;
	std::vector<Var> params;
	std::vector<Expression> tmps;
	std::vector <Word> _udas;
	bool _isUnion;
	
    public:

	IStruct (Word ident, std::vector <Expression> tmps, std::vector <Var> params, std::vector <Word> udas, bool isUnion = false);
	
	void declare () override;
	
	void declare (semantic::Module) override;
	
	void declareAsExtern (semantic::Module) override;
	
	Declaration templateDeclReplace (const std::map <std::string, Expression> &); 

	void print (int nb = 0) override ;
	
	virtual ~IStruct ();

    private:

	bool verifUdas ();
	
    };

    typedef IStruct* Struct;

}
