#pragma once

#include "Frame.hh"
#include "FrameProto.hh"
#include "UnPureFrame.hh"
#include "PureFrame.hh"
#include "TemplateFrame.hh"
#include <ymir/ast/ParamList.hh>

namespace syntax {
    class ITypeConstructor;
    typedef ITypeConstructor* TypeConstructor;

    class ITypeDestructor;
    typedef ITypeDestructor* TypeDestructor;
    
    class IParamList;
    typedef IParamList* ParamList; 
    
}


namespace semantic {
    
    class IMethodFrame : public IFrame {
	semantic::InfoType _info;
	syntax::TypeConstructor _const;
	syntax::TypeDestructor _dest;
	std::string _name;
	FrameProto proto;
	
    public:

	IMethodFrame (Namespace, std::string, InfoType, syntax::TypeConstructor);

	IMethodFrame (Namespace, std::string, InfoType, syntax::TypeDestructor);
	
	FrameProto validate (const std::vector <InfoType> & params) override;

	FrameProto validate (syntax::ParamList params) override;

	FrameProto validate () override;
	
	ApplicationScore isApplicable (syntax::ParamList params) override;

	ApplicationScore isApplicable (const std::vector <InfoType> & params) override;

	static const char * id () {
	    return "IMethodFrame";
	}
	
	virtual const char* getId ();	
	
    };
	
}
