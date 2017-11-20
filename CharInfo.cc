#include <ymir/semantic/types/_.hh>

namespace semantic {

    ICharInfo::ICharInfo (bool isConst) :
	IInfoType (isConst)
    {}

    bool ICharInfo::isSame (InfoType other) {
	return other-> is<ICharInfo> ();
    }
	
    InfoType ICharInfo::BinaryOp (Word op, syntax::Expression right) {
	if (op == Token::EQUAL) return Affect (right);
	if (op == Token::MINUS_AFF) return opAff (op, right);
	if (op == Token::PLUS_AFF) return opAff (op, right);
	if (op == Token::INF) return opTest (op, right);
	if (op == Token::SUP) return opTest (op, right);
	if (op == Token::DEQUAL) return opTest (op, right);
	if (op == Token::INF_EQUAL) return opTest (op, right);
	if (op == Token::SUP_EQUAL) return opTest (op, right);
	if (op == Token::NOT_EQUAL) return opTest (op, right);
	if (op == Token::PLUS) return opNorm (op, right);
	if (op == Token::MINUS) return opNorm (op, right);
	return NLL;
    }

    InfoType ICharInfo::BinaryOpRight (Word op, syntax::Expression left) {
	if (op == Token::EQUAL) return AffectRight (left);
	if (op == Token::INF) return opTestRight (op, left);
	if (op == Token::SUP) return opTestRight (op, left);
	if (op == Token::DEQUAL) return opTestRight (op, left);
	if (op == Token::INF_EQUAL) return opTestRight (op, left);
	if (op == Token::SUP_EQUAL) return opTestRight (op, left);
	if (op == Token::NOT_EQUAL) return opTestRight (op, left);
	if (op == Token::PLUS) return opNormRight (op, left);
	if (op == Token::MINUS) return opNormRight (op, left);
	return NULL;
    }

    std::string ICharInfo::innerTypeString () {
	return "char";
    }

    std::string ICharInfo::simpleTypeString () {
	return "a";
    }

    InfoType ICharInfo::DotOp (syntax::Var var) {
	if (var-> hasTemplate ()) return NULL;
	if (var-> token.getStr () == "init") return Init ();
	if (var-> token.getStr () == "sizeof") return SizeOf ();
	if (var-> token.getStr () == "typeid") return StringOf ();
    }

    InfoType ICharInfo::CastOp (InfoType ot) {
	if (other-> is<ICharInfo> ()) return this;
	if (auto ot = other-> to<IFixedInfo> ()) {
	    auto aux = ot-> clone ();
	    //TODO;
	    return aux;
	}
	return NULL;
    }

    InfoType ICharInfo::CompOp (InfoType) {
	if (other-> is<IUndefInfo> () || other-> is<ICharInfo> ()) {
	    auto ch = new CharInfo (this-> isConst ());
	    //TODO
	    return ch;
	} else if (auto en = other-> to<IEnumInfo> ()) {
	    return this-> CompOp (en-> content ());
	}
	return NULL;
    }

    InfoType ICharInfo::clone () {
	auto ret = new ICharInfo (this-> isConst ());
	//TODO
	return ret;
    }
		
    const char* ICharInfo::getId () {
	return ICharInfo::id ();
    }

    InfoType ICharInfo::Affect (syntax::Expression) {
	if (right-> info-> type-> is<ICharInfo> ()) {
	    auto ch = new ICharInfo (this-> isConst ());
	    //ch-> lintInst = CharUtils::InstAffect;
	    return ch;
	}
	return NULL;
    }

    InfoType ICharInfo::AffectRight (syntax::Expression) {
	if (left-> info-> type-> is<IUndefInfo> ()) {
	    auto ch = new ICharInfo (this-> isConst ());
	    //ch-> lintInst = CharUtils::InstAffect;
	    return ch;
	}
	return NULL;
    }

    InfoType ICharInfo::opTest (Word op, syntax::Expression right) {
	if (right-> info-> type-> is<ICharInfo> ()) {
	    auto bl = new IBoolInfo (true);
	    //TODO
	    // if (this-> value)
	    //    ch-> value = this-> value-> BinaryOp (op, right-> info-> type-> value);
	} else if (auto ot = right-> info-> type-> to<IFixedInfo> ()) {
	    if (ot-> type () == FixedConst::UBYTE) {
		auto ch = new IBoolInfo (true);
		//TODO
		// if (this-> value)
		//    ch-> value = this-> value-> BinaryOp (op, right-> info-> type-> value);
		return ch;
	    }
	}
	return NULL;
    }

    InfoType ICharInfo::opTestRight (Word op, syntax::Expression left) {
	if (auto ot = left-> info-> type-> to<IFixedInfo> ()) {
	    if (ot-> type () == FixedConst::UBYTE) {
		auto ch = new IBoolInfo (true);
		//TODO
		// if (this-> value)
		//    ch-> value = this-> value-> BinaryOpRight (op, right-> info-> type-> value);
		return ch;
	    }
	}
	return NULL;
    }

    InfoType ICharInfo::opAff (Word op, syntax::Expression right) {
	if (right-> info-> type-> is<ICharInfo> ()) {
	    auto ch = new ICharInfo (false);
	    //TODO ch-> lintInst = CharUtils::InstOpAff ();
	    return ch;
	} else if (auto ot = right-> info-> type-> to<IFixedInfo> ()) {
	    if (ot-> type () == FixedConst::UBYTE) {
		auto ch = new ICharInfo (false);
		//TODO ch-> lintInst = CharUtils::InstOpAff ();
		return ch;
	    }
	}
	return NULL;
    }

    InfoType ICharInfo::opNorm (Word op, syntax::Expression right) {
	if (right-> info-> type-> is<ICharInfo> ()) {
	    auto ch = new ICharInfo (true);
	    //TODO
	    return ch;
	} else if (auto ot = right-> info-> type-> to<IFixedInfo> ()) {
	    if (ot-> type () == FixedConst::UBYTE) {
		auto ch = new ICharInfo (true);
		return ch;
	    }
	}
	return NULL;
    }

    InfoType ICharInfo::opNormRight (Word op, syntax::Expression left) {
	if (auto ot = left-> info-> type-> to<IFixedInfo> ()) {
	    if (ot-> type () == FixedConst::UBYTE) {
		auto ch = new ICharInfo (true);
		//TODO
		return ch;
	    }
	}
	return NULL;
    }

    InfoType ICharInfo::Init () {
	auto ch = new ICharInfo (true);
	//TODO
	return ch;
    }

    InfoType ICharInfo::SizeOf () {
	auto _in = new IFixedInfo (true, FixedConst::UBYTE);
	//TODO
	return _in;
    }

    InfoType ICharInfo::StringOf () {
	auto str = new IStringInfo (true);
	//TODO
	return str;
    }

}
