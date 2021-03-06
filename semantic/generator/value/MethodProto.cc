#include <ymir/semantic/generator/value/MethodProto.hh>
#include <ymir/semantic/generator/type/Void.hh>
#include <ymir/semantic/generator/value/ProtoVar.hh>
#include <ymir/semantic/generator/type/LambdaType.hh>
#include <ymir/utils/Path.hh>

namespace semantic {

    namespace generator {

	MethodProto::MethodProto () :
	    FrameProto (),
	    _isMut (false),
	    _classType (Generator::empty ()),
	    _trait (Generator::empty ())
	{}

	MethodProto::MethodProto (const lexing::Word & loc, const std::string & comments, const std::string & name, const Generator & type, const std::vector<Generator> & params, bool isCVariadic, const Generator& classType, bool isMutable, bool isEmptyFrame, bool isFinal, bool isSafe, const Generator & trait, const std::vector <Generator> & throwers) :
	    FrameProto (loc, name, type, params, isCVariadic, isSafe, throwers),
	    _isMut (isMutable),
	    _classType (classType),
	    _isEmptyFrame (isEmptyFrame),
	    _isFinal (isFinal),
	    _trait (trait),
	    _comments (comments)
	{}

	Generator MethodProto::init (const lexing::Word & loc, const std::string & comments, const std::string & name, const Generator & type, const std::vector<Generator> & params, bool isCVariadic, const Generator& classType, bool isMutable, bool isEmptyFrame, bool isFinal, bool isSafe, const Generator & trait, const std::vector <Generator> & throwers) {
	    return Generator {new (NO_GC) MethodProto (loc, comments, name, type, params, isCVariadic, classType, isMutable, isEmptyFrame, isFinal, isSafe, trait, throwers)};
	}
	
	Generator MethodProto::clone () const {
	    return Generator {new (NO_GC) MethodProto (*this)};
	}

	bool MethodProto::equals (const Generator & gen) const {
	    if (!gen.is <MethodProto> ()) return false;
	    auto fr = gen.to<MethodProto> ();
	    if (fr.getMangledName () != this-> getMangledName ()) return false;
	    if (fr.getParameters ().size () != this-> _params.size ()) return false;
	    if (!fr.getReturnType ().equals (this-> _type)) return false;
	    for (auto it : Ymir::r (0, this-> _params.size ())) {
		if (!fr.getParameters () [it].equals (this-> _params [it])) return false;
	    }
	    return true;
	}

	std::string MethodProto::prettyString () const {
	    auto buf = FrameProto::prettyString ();
	    
	    if (this-> _isMut) buf = "(mut self) => " + buf;
	    else
		buf = "(const self) => " + buf;
	    return buf;
	}

	const Generator & MethodProto::getClassType () const {
	    return this-> _classType;
	}

	bool MethodProto::isMutable () const {
	    return this-> _isMut;
	}

	bool MethodProto::isEmptyFrame () const {
	    return this-> _isEmptyFrame;
	}

	bool MethodProto::isFinal () const {
	    return this-> _isFinal;
	}
	
	const Generator & MethodProto::getTrait () const {
	    return this-> _trait;
	}

	const std::string & MethodProto::getComments () const {
	    return this-> _comments;
	}
	
    }
    
}
