#pragma once

#include "Frame.hh"
#include "FrameProto.hh"
#include <ymir/ast/LambdaFunc.hh>
#include <ymir/ast/ParamList.hh>

namespace semantic {

    class ILambdaFrame : public IFrame {

	syntax::LambdaFunc frame;
	std::string name;

    public:

	ILambdaFrame (Namespace space, std::string& name, syntax::LambdaFunc func);

	FrameProto validate (syntax::ParamList params) override;

	FrameProto validate (const std::vector<InfoType> & params) override;

	ApplicationScore isApplicable (syntax::ParamList params) override;
	
	static const char * id () {
	    return "ILambdaFrame";
	}

	std::string getName ();
	
	syntax::LambdaFunc func ();

	std::string toString () override;
	
	virtual const char* getId ();	
	
    };

    typedef ILambdaFrame* LambdaFrame;
}