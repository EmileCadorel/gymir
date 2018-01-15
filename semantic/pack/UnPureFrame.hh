#pragma once

#include "Frame.hh"
#include "FrameProto.hh"
#include <ymir/ast/Function.hh>
#include <ymir/ast/ParamList.hh>

namespace semantic {

    class IUnPureFrame : public IFrame {

	std::string name;

    public:

	IUnPureFrame (Namespace space, syntax::Function func);

	FrameProto validate (syntax::ParamList params) override;

	FrameProto validate (std::vector <InfoType> params) override;

	static const char * id () {
	    return "IUnPureFrame";
	}

	virtual const char* getId ();	
	
    };

    typedef IUnPureFrame* UnPureFrame;
}