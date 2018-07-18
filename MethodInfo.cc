#include <ymir/semantic/object/MethodInfo.hh>
#include <ymir/ast/ParamList.hh>
#include <ymir/syntax/Keys.hh>
#include <ymir/semantic/pack/Table.hh>
#include <ymir/semantic/tree/Generic.hh>
#include <ymir/semantic/pack/MethodFrame.hh>

namespace semantic {

    namespace MethodUtils {
	using namespace Ymir;
	using namespace syntax;

	Tree InstCall (Word token, InfoType type, Expression left, Expression right, ApplicationScore score) {
	    auto params = right-> to <IParamList> ();
	    
	    std::vector <InfoType> treats (params-> getTreats ().begin () + 1, params-> getTreats ().end ());
	    std::vector <tree> args = params-> toGenericParams (treats);
	    auto ltree = left-> toGeneric ();
	    args.insert (args.begin (), getAddr (ltree).getTree ());
	    std::vector <tree> types;
	    for (auto it : args)
		types.push_back (TREE_TYPE (it));
	    
	    tree ret = type-> toGeneric ().getTree ();
	    auto ptr_type = build_pointer_type (
		build_function_type_array (ret, types.size (), types.data ())
	    );
	    tree access;
	    if (score-> proto == NULL) {
		auto vtable = getField (token.getLocus (), ltree, Keys::VTABLE_FIELD);
		auto nb = score-> methIndex;
		auto padd = build_int_cst_type (long_unsigned_type_node, nb);
		access = Ymir::getPointerUnref (token.getLocus (), vtable, ptr_type, padd).getTree ();
	    } else {
		access = score-> proto-> toGeneric ().getTree ();
	    }
	     
	    return build_call_array_loc (token.getLocus (),
					 type-> toGeneric ().getTree (),
					 access,
					 args.size (),
					 args.data ());	    
	}

    }
    
    IMethodInfo::IMethodInfo (AggregateInfo info, std::string name, const std::vector <Frame> & frames, const std::vector <int> & index, bool isStatic) :
	IInfoType (true),
	_info (info),
	_name (name),
	_frames (frames),
	_index (index),
	_isDynamic (!isStatic)
    {}

    bool IMethodInfo::isSame (InfoType) {
	return false;
    }

    InfoType IMethodInfo::onClone () {
	return new (Z0) IMethodInfo (this-> _info, this-> _name, this-> _frames, this-> _index, !this-> _isDynamic);
    }

    ApplicationScore IMethodInfo::CallAndThrow (Word tok, const std::vector <InfoType> & params, InfoType & retType) {
	std::vector <ApplicationScore> total;
	for (auto it : this-> _frames) {
	    total.push_back (it-> isApplicable (params));
	}

	std::vector <Frame> goods;
	std::vector <ulong> index;
	ApplicationScore right = new (Z0) IApplicationScore ();
	for (uint it = 0 ; it < total.size () ; it ++) {
	    if (total [it]) {
		if (goods.size () == 0 && total [it]-> score != 0) {
		    right = total [it];
		    goods.push_back (this-> _frames [it]);
		    index.push_back (this-> _index [it]);
		} else if (right-> score < total [it]-> score) {
		    goods.clear ();
		    index.clear ();
		    goods.push_back (this-> _frames [it]);
		    index.push_back (this-> _index [it]);
		} else if (right-> score == total [it]-> score && total [it]-> score != 0) {
		    goods.push_back (this-> _frames [it]);
		    index.push_back (this-> _index [it]);
		}		
	    }
	}

	if (goods.size () == 0) return NULL;
	else if (goods.size () != 1) {
	    Ymir::Error::templateSpecialisation (goods [0]-> ident (),
						 goods [1]-> ident ());
	    return NULL;
	}

	right-> methIndex = index.back ();
	if (!Table::instance ().addCall (tok)) return NULL;
	auto fparams = std::vector <InfoType> (right-> treat.begin () + 1, right-> treat.end ());
	if (right-> toValidate) {
	    if (Table::instance ().hasCurrentContext (Keys::SAFE) && !(right-> toValidate-> has (Keys::SAFE) || right-> toValidate-> has (Keys::TRUSTED)))
		Ymir::Error::callUnsafeInSafe (tok);
	    FrameProto info = right-> toValidate-> validate (right, fparams);
	    retType = info-> type ()-> type ();
	    if (!right-> toValidate-> is <IMethodFrame> () || !right-> toValidate-> to <IMethodFrame> ()-> isVirtual () || !this-> _isDynamic) {
		right-> proto = info;
	    }
	    
	    right-> dyn = true;	    
	    right-> isMethod = true;
	} else {
	    if (Table::instance ().hasCurrentContext (Keys::SAFE) && !(goods [0]-> has (Keys::SAFE) || goods [0]-> has (Keys::TRUSTED)))
		Ymir::Error::callUnsafeInSafe (tok);
	    
	    FrameProto info = goods [0]-> validate (right, fparams);
	    retType = info-> type ()-> type ();
	    if (!goods [0]-> is <IMethodFrame> () || !goods [0]-> to<IMethodFrame> ()-> isVirtual () || !this-> _isDynamic) {
		right-> proto = info;
	    }
	    
	    right-> dyn = true;
	    right-> isMethod = true;
	}
	return right;
	
    }
    
    ApplicationScore IMethodInfo::CallOp (Word tok, syntax::ParamList params) {
	InfoType retType;
	auto types = params-> getParamTypes ();
	types.insert (types.begin (), new (Z0) IRefInfo (this-> _info-> isConst (), this-> _info-> clone ()));
	
	auto right = this-> CallAndThrow (tok, types, retType);
	if (right) {
	    right-> ret = retType-> clone ();
	    right-> ret-> value () = retType-> value ();
	    right-> ret-> multFoo = &MethodUtils::InstCall;
	}
	
	return right;
    }

    std::string IMethodInfo::typeString () {
	return this-> innerTypeString ();
    }
    
    std::string IMethodInfo::innerTypeString () {
	return "method <" + this-> _info-> typeString () + "." + this-> _name + ">";
    }

    std::string IMethodInfo::innerSimpleTypeString () {
	Ymir::Error::assert ("TODO");
	return "";
    }

    const char* IMethodInfo::getId () {
	return IMethodInfo::id ();
    }            
    
}
