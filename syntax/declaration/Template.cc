#include <ymir/syntax/declaration/Template.hh>

namespace syntax {

    Template::Template () :
	_loc (lexing::Word::eof ()),
	_content (Declaration::empty ()),
	_test (Expression::empty ())
    {}
    
    Declaration Template::init () {
	return Declaration {new (Z0) Template ()};
    }

    Declaration Template::init (const Template & tmpl) {
	auto ret = new (Z0) Template ();
	ret-> _loc = tmpl._loc;
	ret-> _parameters = tmpl._parameters;
	ret-> _content = tmpl._content;
	ret-> _test = tmpl._test;
	return Declaration {ret};
    }

    Declaration Template::init (const lexing::Word & loc, const std::vector <Expression> & params, const Declaration & content) {
	auto ret = new (Z0) Template ();
	ret-> _loc = loc;
	ret-> _parameters = params;
	ret-> _content = content;
	return Declaration {ret};
    }
    
    Declaration Template::init (const lexing::Word & loc, const std::vector <Expression> & params, const Declaration & content, const Expression & test) {
	auto ret = new (Z0) Template ();
	ret-> _loc = loc;
	ret-> _parameters = params;
	ret-> _content = content;
	ret-> _test = test;
	return Declaration {ret};
    }

    Declaration Template::clone () const {
	return Template::init (*this);
    }

    void Template::treePrint (Ymir::OutBuffer & stream, int i) const {
	stream.writef ("%*<Template>", i, '\t');
	stream.writeln (this-> _loc);
	stream.writefln ("%*<Test>", i + 1, '\t');
	this-> _test.treePrint (stream, i + 2);
	stream.writefln ("%*<Params>", i + 1, '\t');
	for (auto & it : this-> _parameters)
	    it.treePrint (stream, i + 2);
	stream.writefln ("%*<Content>", i + 1, '\t');
	this-> _content.treePrint (stream, i + 2);
    }	
    
    bool Template::isOf (const IDeclaration * type) const {
	auto vtable = reinterpret_cast <const void* const *> (type) [0];
	Template thisType; // That's why we cannot implement it for all class
	if (reinterpret_cast <const void* const *> (&thisType) [0] == vtable) return true;
	return IDeclaration::isOf (type);
    }	    

    void Template::addParameter (const Expression & param) {
	this-> _parameters.push_back (param);
    }

    void Template::setContent (const Declaration& content) {
	this-> _content = content;
    }

    const lexing::Word & Template::getLocation () const {
	return this-> _loc;
    }

    const std::vector <Expression> & Template::getParams () const {
	return this-> _parameters;
    }

    const Declaration & Template::getContent () const {
	return this-> _content;
    }

    const Expression & Template::getTest () const {
	return this-> _test;
    }    
    
}
