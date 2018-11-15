#pragma once

#include "FrameScope.hh"
#include "Namespace.hh"
#include "Scope.hh"
#include "Module.hh"
#include "../../ast/Instruction.hh"
#include "../../ast/Global.hh"

#include <ymir/utils/Array.hh>
#include <list>

namespace semantic {

    class IAggregateCstInfo;
    typedef IAggregateCstInfo* AggregateCstInfo;
    
    class Table {

	std::list <FrameScope> _frameTable;
	Scope _globalScope;
	std::list <Namespace> _templateScope;
	std::vector <Module> _importations;
	std::vector <Namespace> _foreigns;
	Namespace _space;
	Namespace _programSpace;
	std::vector <syntax::Global> _globalVars;
	ulong _nbFrame = 0;
	static const int __maxNbRec__ = 1000;
	std::vector <syntax::Instruction> _staticInits;
	Frame _constructor;
	Frame _destructor;
	
    public:
	
	static Table& instance () {
	    return __instance__;
	}
              
	void enterBlock ();
	
	void enterPhantomBlock ();

	void quitBlock ();

	void setCurrentSpace (Namespace space);

	Namespace getCurrentSpace ();

	bool addCall (Word sym);

	void addGlobal (syntax::Global gl);

	std::vector <syntax::Global> globalVars ();

	void enterFrame (Namespace space, const std::string & name, std::vector <syntax::Expression> &tmps, const std::vector <Word> & context, bool internal);

	bool hasCurrentContext (const std::string & uda);
	
	void quitFrame ();

	Namespace space ();

	Namespace globalNamespace ();

	Namespace& programNamespace ();
	
	Namespace& templateNamespace ();

	void insert (Symbol info);

	void addStaticInit (syntax::Instruction exp);

	std::vector <syntax::Instruction> staticInits ();

	void purge ();

	Symbol get (const std::string &name);

	std::vector <Symbol> getAll (const std::string &name);
	
	/**
	 * \return the inner language definition of TypeInfo, cf. core.info
	 */
	AggregateCstInfo getTypeInfoType ();

	AggregateCstInfo getTypeInfoType (const std::string &name);

	Symbol getTypeInfoSymbol (const std::string &name); 
	
	Symbol getLocal (const std::string &name);

	bool isFrameLocal (Symbol sym);
	
	Symbol local (const std::string &name);

	bool sameFrame (Symbol sym);

	bool parentFrame (Symbol sym);

	Symbol getAlike (const std::string & name);

	FrameReturnInfo& retInfo ();
	
	Module addModule (Namespace space);

	Module getModule (Namespace space);
	
	void addForeignModule (Namespace space);

	std::vector <Module> getAllMod (Namespace space);

	bool isModule (Namespace space);

	void openModuleForSpace (Namespace from, Namespace to);

	void closeModuleForSpace (Namespace from, Namespace to);

	bool verifyClosureLifeTime (ulong id, const std::vector <Symbol> & closure);

	ulong getCurrentLifeTime ();
	
	std::vector <Namespace> modules ();

	std::vector <Namespace> modulesAndForeigns ();

	Frame moduleConstruct (Namespace space);
	
	Frame moduleDestruct (Namespace space);

	Frame & constructor ();

	Frame & destructor ();
	
	bool moduleExists (Namespace name);

	ulong nbRecursive ();
	
    private:

	Table ();
	Table (Table&);
	Table& operator= (Table&);
	
	static Table __instance__;
	
    };
  
}
