#pragma once

#include <ymir/semantic/value/Value.hh>
#include <ymir/ast/Constante.hh>

namespace semantic {

    union signedun {
	long l;
	ulong ul;
    };
    
    class IFixedValue : public IValue {

	signedun value;
	FixedConst type;
	
    public:

	IFixedValue (FixedConst cst, ulong ul, long l);

	ulong& getUValue ();

	long& getValue ();

	Value BinaryOp (Word , Value ) override;

	Value UnaryOp (Word) override;
	
	const char* getId () override;

	static const char* id () {
	    return "IFixedValue";
	}

	std::string toString () override;

	syntax::Expression toYmir (Symbol sym) override;
	
	bool equals (Value) override;

	Value clone () override;
	
    private :

	Value add (Word, Value other);
	Value sub (Word, Value other);
	Value div (Word, Value other);
	Value mul (Word, Value other);
	Value lor (Value other);
	Value land (Value other);
	Value lshift (Value other);
	Value rshift (Value other);
	Value lxor (Value other);
	Value mod (Value other);
	Value dand (Value other);
	Value dor (Value other);
	Value inf (Value other);
	Value sup (Value other);
	Value infeq (Value other);
	Value supeq (Value other);
	Value neq (Value other);
	Value eq (Value other);

	void check (Word op, long value, FixedConst cst);
	void check (Word op, ulong value, FixedConst cst);
	
    };

}
