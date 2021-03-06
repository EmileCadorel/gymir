#include <ymir/syntax/expression/Cast.hh>

namespace syntax {

    Cast::Cast () :
	IExpression (lexing::Word::eof ()),
	_type (Expression::empty ()),
	_content (Expression::empty ())
    {}
    
    Cast::Cast (const lexing::Word & loc, const Expression & type, const Expression & content) :
	IExpression (loc),
	_type (type),
	_content (content)
    {}

    Expression Cast::init (const lexing::Word & location, const Expression & type, const Expression & content) {
	return Expression {new (NO_GC) Cast (location, type, content)};
    }
    
    void Cast::treePrint (Ymir::OutBuffer & stream, int i) const {
	stream.writef ("%*<Cast>", i, '\t');
	this-> _type.treePrint (stream, i + 1);
	this-> _content.treePrint (stream, i + 1);
    }

    const Expression & Cast::getType () const {
	return this-> _type;
    }

    const Expression & Cast::getContent () const {
	return this-> _content;
    }

    std::string Cast::prettyString () const {
	return Ymir::format ("cast!(%) (%)", this-> _type.prettyString(), this-> _content.prettyString ());
    }
}
