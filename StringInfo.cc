#include <ymir/semantic/types/_.hh>
#include <ymir/semantic/utils/StringUtils.hh>
#include <ymir/semantic/tree/Generic.hh>
#include <ymir/ast/TreeExpression.hh>
#include <ymir/ast/ParamList.hh>
#include <ymir/errors/Error.hh>
#include <ymir/semantic/pack/FinalFrame.hh>
#include <ymir/semantic/pack/InternalFunction.hh>
#include <ymir/semantic/value/_.hh>
#include <ymir/semantic/utils/ArrayUtils.hh>

namespace semantic {


    namespace StringUtils {
	using namespace syntax;
	using namespace Ymir;

	
	bool isStringType (Tree type) {
	    return type.getTreeCode () == POINTER_TYPE
		&& TYPE_MAIN_VARIANT (TREE_TYPE (type.getTree ())) == char_type_node;
	}


	Tree getLen (location_t loc, Tree tree) {
	    if (tree.getType ().getTreeCode () != RECORD_TYPE) {
		if (isStringType (tree.getType ())) {
		    if (TREE_CONSTANT (tree.getTree ())) {
			auto t = tree.getOperand (0).getOperand (0);
			return build_int_cst_type (long_unsigned_type_node, TREE_STRING_LENGTH (t.getTree ()) - 1);
		    }
		} 
		Ymir::Error::assert ("");
		return Ymir::Tree ();
	    } else {
		return getField (loc, tree, "len");
	    }
	}

	Tree getPtr (location_t loc, Tree tree) {
	    if (tree.getType ().getTreeCode () != RECORD_TYPE) {
		return tree;
	    } else {
		return getField (loc, tree, "ptr");
	    }
	}
	
	Tree buildString (location_t loc, Tree len) {
	    Tree fn = InternalFunction::getMalloc ();
	    auto byteLen = buildTree (
		MULT_EXPR, loc,
		size_type_node,
		fold_convert (size_type_node, len.getTree ()),
		TYPE_SIZE_UNIT (char_type_node)
	    );
	    
	    tree args [] = {byteLen.getTree ()};	    
	    return build_call_array_loc (loc, build_pointer_type (void_type_node), fn.getTree (), 1, args);	    
	}

	Tree copyString (location_t loc, Tree dst, Tree src, Tree len) {
	    auto byteLen = buildTree (
		MULT_EXPR, loc,
		size_type_node,
		fold_convert (size_type_node, len.getTree ()),
		TYPE_SIZE_UNIT (char_type_node)
	    );
	    
	    tree argsMemcpy [] = {dst.getTree (), src.getTree (), byteLen.getTree ()};
	    return build_call_array_loc (loc, void_type_node, InternalFunction::getYMemcpy ().getTree (), 3, argsMemcpy);	    
	}
	
	Tree buildDup (location_t loc, Tree lexp, Tree rexp) {
	    TreeStmtList list;	    
	    Tree lenl = getField (loc, lexp, "len");	    
	    Tree ptrl = getField (loc, lexp, "ptr");
	    Tree len, ptrr;
	    
	    if (rexp.getTreeCode () != CALL_EXPR) {
		len = getLen (loc, rexp);
		ptrr = getPtr (loc, rexp);		
	    } else {
		auto aux = makeAuxVar (loc, ISymbol::getLastTmp (), lexp.getType ());
		list.append (buildTree (
		    MODIFY_EXPR, loc, void_type_node, aux, rexp
		));
		
		len = getField (loc, aux, "len");	    
		ptrr = getField (loc, aux, "ptr");
	    }

	    auto allocRet = buildString (loc, len);
	    list.append (buildTree (
		MODIFY_EXPR, loc, void_type_node, lenl.getTree (), len.getTree ()
	    ));
		
	    list.append (buildTree (
		MODIFY_EXPR, loc, void_type_node, ptrl.getTree (), allocRet
	    ));
		
	    list.append (copyString (loc, ptrl, ptrr, len));
	    
	    getStackStmtList ().back ().append (list.getTree ());
	    return lexp;	    
	}
	
	Tree InstAff (Word word, InfoType, Expression left, Expression right) {
	    location_t loc = word.getLocus ();	   
	    auto lexp = left-> toGeneric ();
	    auto rexp = right-> toGeneric ();
	    
	    if (right-> info-> isConst ()) {
		if (auto ref = left-> info-> type-> to <IArrayRefInfo> ()) {
		    if (!ref-> content ()-> isConst ())
			return buildDup (loc, lexp, rexp);
		} else if (!left-> info-> isConst ())
		    return buildDup (loc, lexp, rexp);
	    }

	    if (!isStringType (rexp.getType ())) {
		getStackStmtList ().back ().append (buildTree (
		    MODIFY_EXPR, loc, void_type_node, lexp, rexp
		));
		return lexp;
	    }

	    TreeStmtList list;
	    Tree lenl = getField (loc, lexp, "len");
	    Tree ptrl = getField (loc, lexp, "ptr");	
	    
	    auto lenr = getLen (loc, rexp);
	    auto ptrr = getPtr (loc, rexp);	    
	    
	    list.append (buildTree (
		MODIFY_EXPR, loc, void_type_node, lenl, lenr)
	    );
	    
	    list.append (buildTree (
		MODIFY_EXPR, loc, void_type_node, ptrl, ptrr)
	    );
	    
	    getStackStmtList ().back ().append (list.getTree ());
		
	    return lexp;			    
	}
	
	Tree InstPtr (Word locus, InfoType, Expression expr, Expression) {
	    location_t loc = locus.getLocus ();
	    return getPtr (loc, expr-> toGeneric ());
	}

	Tree InstLen (Word locus, InfoType, Expression expr, Expression) {
	    location_t loc = locus.getLocus ();
	    return getLen (loc, expr-> toGeneric ());
	}
	
	Tree InstToString (Word locus, InfoType, Expression elem, Expression type) {
	    if (elem-> info-> isConst ()) {
		return InstToArray (locus, NULL, elem, type);
	    } else {
		auto rexp = elem-> toGeneric ();
		if (rexp.getTreeCode () == CALL_EXPR)
		    return rexp;
		
		return rexp;
	    }
	}

	Tree InstToArray (Word locus, InfoType, Expression elem, Expression type) {
	    auto rexp = elem-> toGeneric ();
	    if (rexp.getTreeCode () == CALL_EXPR)
		return rexp;
	    
	    location_t loc = locus.getLocus ();
	    if (!type-> info-> isConst () && elem-> info-> isConst ()) {
		if (auto ref = type-> info-> type-> to <IArrayRefInfo> ()) {
		    if (!ref-> content ()-> isConst ()) {
			Tree auxVar = makeAuxVar (loc, ISymbol::getLastTmp (), type-> info-> type-> toGeneric ());
			return buildDup (loc, auxVar, rexp);
		    }
		} else {
		    Tree auxVar = makeAuxVar (loc, ISymbol::getLastTmp (), type-> info-> type-> toGeneric ());
		    return buildDup (loc, auxVar, rexp);
		}
	    } 

	    TreeStmtList list;
	    Tree lenr = getLen (loc, rexp);
	    Tree ptrr = getPtr (loc, rexp);

	    Tree aux = makeAuxVar (loc, ISymbol::getLastTmp (), type-> info-> type-> toGeneric ());

	    Tree lenl = getField (loc, aux, "len");
	    Tree ptrl = getField (loc, aux, "ptr");
	    list.append (buildTree (
		MODIFY_EXPR, loc, void_type_node, lenl, lenr)
	    );
		    
	    list.append (buildTree (
		MODIFY_EXPR, loc, void_type_node, ptrl, ptrr)
	    );

	    getStackStmtList ().back ().append (list.getTree ());
	    return aux;
	}
	
		
	Tree InstConcat (Word locus, InfoType, Expression left, Expression right) {
	    location_t loc = locus.getLocus ();
	    Ymir::TreeStmtList list; 
	    auto lexp = left-> toGeneric ();
	    auto rexp = right-> toGeneric ();

	    auto lenl = getLen (loc, lexp);
	    auto lenr = getLen (loc, rexp);
	    auto ptrl = getPtr (loc, lexp);
	    auto ptrr = getPtr (loc, rexp);
	    
	    Ymir::Tree aux = makeAuxVar (loc, ISymbol::getLastTmp (), IStringInfo::toGenericStatic ());	    

	    auto auxPtr = getField (loc, aux, "ptr");
	    auto auxLen = getField (loc, aux, "len");
	    auto len = buildTree (PLUS_EXPR, loc, lenl.getType (), lenl, lenr);    
	    auto allocRet = buildString (loc, len);
	    
	    list.append (buildTree (
		MODIFY_EXPR, loc, void_type_node, auxLen, len
	    ));
	    
	    list.append (buildTree (
		MODIFY_EXPR, loc, void_type_node, auxPtr, allocRet
	    ));

	    list.append (copyString (loc, auxPtr, ptrl, lenl));
	    list.append (copyString (loc, build2 (POINTER_PLUS_EXPR, auxPtr.getType ().getTree (),
				auxPtr.getTree (), convert_to_ptrofftype (lenl.getTree ())),
			ptrr, lenr)
	    );
	    
	    Ymir::getStackStmtList ().back ().append (list.getTree ());
	    return aux;
	}

	Tree InstConcatAff (Word locus, InfoType type, Expression left, Expression right) {
	    location_t loc = locus.getLocus ();
	    auto lexp = left-> toGeneric ();
	    auto aux = InstConcat (locus, type, new (Z0)  ITreeExpression (left-> token, left-> info-> type, lexp), right);
	    Ymir::TreeStmtList list;
	    auto lenl = getLen (loc, lexp);
	    auto lenr = getField (loc, aux, "len");
	    auto ptrl = getPtr (loc, lexp);
	    auto ptrr = getField (loc, aux, "ptr");

	    list.append (buildTree (
		MODIFY_EXPR, loc, void_type_node, lenl, lenr
	    ));
	    
	    list.append (buildTree (
		MODIFY_EXPR, loc, void_type_node, ptrl, ptrr
	    ));
	    
	    Ymir::getStackStmtList ().back ().append (list.getTree ());
	    
	    return lexp;
	}

	Ymir::Tree InstAccessInt (Word word, InfoType, Expression left, Expression right) {
	    location_t loc = word.getLocus ();
	    auto lexp = left-> toGeneric ();
	    auto rexp = right-> toGeneric ();
	    if (isStringType (rexp.getType ())) {
		return getPointerUnref (loc, lexp, char_type_node, rexp);
	    } else {
		Ymir::Tree ptrl = Ymir::getField (loc, lexp, "ptr");
		return getPointerUnref (loc, ptrl, char_type_node, rexp);
	    }
	}


	Ymir::Tree InstAddr (Word locus, InfoType, Expression elem, Expression) {
	    return Ymir::getAddr (locus.getLocus (), elem-> toGeneric ());
	}	

	Ymir::Tree InstCall (Word loc, InfoType ret, Expression, Expression paramsExp) {
	    ParamList params = paramsExp-> to <IParamList> ();
	    std::vector <tree> args = params-> toGenericParams (params-> getTreats ());
	    Ymir::TreeStmtList list;
	    auto aux = Ymir::makeAuxVar (loc.getLocus (), ISymbol::getLastTmp (), ret-> toGeneric ());

	    for (auto i : Ymir::r (0, 2)) {
		auto attr = getField (loc.getLocus (), aux, i);
		list.append (buildTree (
		    MODIFY_EXPR, loc.getLocus (), attr.getType (), attr, args [i]
		));
	    }
	    
	    Ymir::getStackStmtList ().back ().append (list.getTree ());
	    return aux;
	}
	
    }


    
    bool IStringInfo::__initStringTypeNode__ = false;
    
    IStringInfo::IStringInfo (bool isConst) :
	IInfoType (isConst)
    {}

    bool IStringInfo::isSame (InfoType other) {
	return other-> is<IStringInfo> ();
    }

    std::string IStringInfo::innerTypeString () {
	return "string";
    }

    std::string IStringInfo::innerSimpleTypeString () {
	return "Aya";
    }

    InfoType IStringInfo::BinaryOp (Word op, syntax::Expression right) {
	if (op == Token::EQUAL) return Affect (right);
	else if (op == Token::TILDE) return Concat (op, right);
	else if (op == Token::TILDE_EQUAL) return ConcatAff (right);
	else return NULL;
    }
    
    InfoType IStringInfo::BinaryOpRight (Word op, syntax::Expression left) {
	if (op == Token::EQUAL) return AffectRight (left);
	return NULL;
    }

    ApplicationScore IStringInfo::CallOp (Word token, syntax::ParamList params) {
	if (params-> getParams ().size () != 2) 
	    return NULL;
	
	auto fst = params-> getParams ()[0];
	auto scd = params-> getParams ()[1];

	auto score = new (Z0) IApplicationScore (token);
	score-> treat.push_back (fst-> info-> type-> CompOp (new (Z0) IFixedInfo (true, FixedConst::ULONG)));
	if (!score-> treat.back ()) return NULL;

	score-> treat.push_back (scd-> info-> type-> CompOp (new (Z0) IPtrInfo (false, new (Z0) ICharInfo (false))));
	if (!score-> treat.back ()) return NULL;

	auto res = new (Z0) IStringInfo (false);
	res-> multFoo = &StringUtils::InstCall;
	score-> dyn = true;
	score-> ret = res;
	return score;
    }
    
    InfoType IStringInfo::AccessOp (Word, syntax::ParamList left, std::vector <InfoType> & treats) {
	if (left-> getParams ().size () == 1) {
	    return Access (left-> getParams () [0], treats [0]);
	}
	return NULL;
    }
    
    InfoType IStringInfo::Access (syntax::Expression expr, InfoType& treat) {
	treat = expr-> info-> type-> CompOp (new (Z0)  IFixedInfo (true, FixedConst::LONG));
	if (treat == NULL) {
	    treat = expr-> info-> type-> CompOp (new (Z0)  IFixedInfo (true, FixedConst::ULONG));
	}
	
	if (treat) {	    
	    auto ch = new (Z0)  ICharInfo (this-> isConst ());
	    ch-> binopFoo = &StringUtils::InstAccessInt;
	    return ch;
	}
	return NULL;
    }
        
    InfoType IStringInfo::onClone () {
	auto ret = new (Z0)  IStringInfo (this-> isConst ());
	if (this-> value ())
	    ret-> value () = this-> value ()-> clone ();
	return ret;
    }

    InfoType IStringInfo::DotOp (syntax::Var var) {
	if (var-> hasTemplate ()) return NULL;
	if (var-> token == "ptr") return Ptr ();
	if (var-> token == "len") return Length ();
	return NULL;
    }

    InfoType IStringInfo::DColonOp (syntax::Var var) {
	if (var-> hasTemplate ()) return NULL;
	if (var-> token == "typeid") return StringOf ();
	return NULL;
    }
    
    InfoType IStringInfo::CastOp (InfoType other) {
	if (auto arr = other-> to <IArrayInfo> ()) {
	    if (arr-> content ()-> is <ICharInfo> ()) {
		auto ret = other-> clone ();
		ret-> binopFoo = &StringUtils::InstToArray;
		return ret;
	    }
	}
	return NULL;
    }
    
    InfoType IStringInfo::CompOp (InfoType other) {
	if (other-> is <IUndefInfo> () || this-> isSame (other)) {
	    auto ret = this-> clone ();
	    ret-> binopFoo = &StringUtils::InstToString;
	    return ret;
	} else if (auto ref = other-> to<IRefInfo> ()) {
	    if (!this-> isConst () && this-> isSame (ref-> content ())) {
		auto ret = new (Z0)  IRefInfo (false, this-> clone ());
		ret-> binopFoo = &StringUtils::InstAddr;
		return ret;
	    }
	}
	return NULL;
    }

    InfoType IStringInfo::ApplyOp (const std::vector<syntax::Var> & vars) {
	if (vars.size () != 1) return NULL;
	if (this-> isConst ()) {
	    auto content = new (Z0) ICharInfo (false);
	    vars [0]-> info-> type = content-> clone ()-> CompOp (vars [0]-> info-> type);
	} else {
	    auto content = new (Z0) ICharInfo (false);
	    InfoType ref = new (Z0) IRefInfo (false, content);
	    content-> isConst (false);
	    vars [0]-> info-> type = content-> CompOp (ref);	    
	}
	
	if (vars [0]-> info-> type == NULL) return NULL;
	auto ret = this-> clone ();
	ret-> applyFoo = &ArrayUtils::InstApply;
	return ret;
    }
    
    InfoType IStringInfo::Ptr () {	
	auto ret = new (Z0)  IPtrInfo (this-> isConst (), new (Z0)  ICharInfo (this-> isConst ()));
	ret-> binopFoo = &StringUtils::InstPtr;
	return ret;
    }

    InfoType IStringInfo::Length () {
	auto ret = new (Z0)  IFixedInfo (true, FixedConst::ULONG);
	ret-> binopFoo = &StringUtils::InstLen;
	if (this-> value ())
	    ret-> value () = new (Z0)  IFixedValue (FixedConst::ULONG, this-> value ()-> to<IStringValue> ()-> toString ().length (), 0);
	return ret;
    }
    
    InfoType IStringInfo::AffectRight (syntax::Expression left) {
	if (left-> info-> type-> is<IUndefInfo> ()) {
	    auto i = this-> clone ();
	    i-> isConst (false);
	    i-> binopFoo = StringUtils::InstAff;
	    return i;
	}
	return NULL;
    }

    InfoType IStringInfo::Affect (syntax::Expression right) {
	if (right-> info-> type-> is <IStringInfo> ()) {
	    auto i = this-> clone ();
	    i-> binopFoo = &StringUtils::InstAff;
	    return i;
	}
	return NULL;
    }

    InfoType IStringInfo::Concat (Word & op, syntax::Expression right) {
	if (right-> info-> type-> is <IStringInfo> ()) {
	    auto i = this-> clone ();
	    i-> isConst (false);
	    if (this-> value ())
		i-> value () = this-> value ()-> BinaryOp (op, right-> info-> value ());
	    i-> binopFoo = &StringUtils::InstConcat;
	    return i;
	}
	return NULL;
    }

    InfoType IStringInfo::ConcatAff (syntax::Expression right) {
	if (this-> isConst ()) return NULL;
	if (right-> info-> type-> is <IStringInfo> ()) {
	    auto i = this-> clone ();
	    i-> isConst (false);
	    i-> value () = NULL;
	    i-> binopFoo = &StringUtils::InstConcatAff;
	    return i;
	}
	return NULL;
    }
    
    const char* IStringInfo::getId () {
	return IStringInfo::id ();
    }

    InfoType IStringInfo::ConstVerif (InfoType other) {
	if (this-> isConst () && !other-> isConst ()) return NULL;
	return this;	
    }
    
    

    Ymir::Tree IStringInfo::toGeneric () {
	return toGenericStatic ();
    }
    
    Ymir::Tree IStringInfo::toGenericStatic () {
	auto string_type_node = IFinalFrame::getDeclaredType ("string");
	if (string_type_node.isNull ()) {
	    string_type_node = Ymir::makeStructType ("string", 2,
						     get_identifier ("len"),
						     (new (Z0)  IFixedInfo (false, FixedConst::ULONG))-> toGeneric ().getTree (),
						     get_identifier ("ptr"),
						     (new (Z0)  IPtrInfo (false, new (Z0)  ICharInfo (false)))-> toGeneric ().getTree ()
	    ).getTree ();
	    IFinalFrame::declareType ("string", string_type_node);	    
	}
	return string_type_node;
    }

    
}
