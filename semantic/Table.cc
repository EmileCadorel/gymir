#include <ymir/semantic/Table.hh>

namespace semantic {

    Table::Table (const Symbol & attach) :
	_attached (attach.getPtr ())
    {}

    std::shared_ptr <Table> Table::init (const Symbol & attach) {
	return std::make_shared <Table> (attach);
    }
    
    std::shared_ptr<Table> Table::clone (const Symbol & attach) const {
	Table ret (attach);
	for (auto it : Ymir::r (0, this-> _syms.size ())) {	    
	    auto cl = this-> _syms [it];
	    cl.setReferent (ret._attached);
	    ret. _syms.push_back (cl);
	}

	return std::make_shared<Table> (ret);
    }

    Symbol Table::getAttach () {
	return Symbol {this-> _attached};
    }
    
    void Table::insert (const Symbol & sym) {
	Symbol toInsert = sym;
	toInsert.setReferent (Symbol {this-> _attached});
	this-> _syms.push_back (toInsert);
    }

    void Table::insertTemplate (const Symbol & sym) {
	Symbol toInsert = sym;
	toInsert.setReferent (Symbol {this-> _attached});
	this-> _templates.push_back (toInsert);
    }

    const std::vector <Symbol> & Table::getTemplates () const {
	return this-> _templates;
    }
    
    void Table::replace (const Symbol & sym) {
	Symbol toInsert = sym;
	toInsert.setReferent (Symbol {this-> _attached});
	for (auto it : Ymir::r (0, this-> _syms.size ())) {
	    if (this-> _syms [it].getName ().str == sym.getName ().str) {
		this-> _syms [it] = sym;
		return;
	    }
	}
	this-> _syms.push_back (sym);
    }
    
    std::vector <Symbol> Table::get (const std::string & name) const {
	std::vector <Symbol> rets;
	for (auto & s : this-> _syms) {
	    if (s.getName ().str == name) {
		rets.push_back (s);
	    }
	}
	
	return rets;
    }

    std::vector <Symbol> Table::getPublic (const std::string & name) const {
	std::vector <Symbol> rets;
	for (auto & s : this-> _syms) {
	    if (s.getName ().str == name && s.isPublic ()) {
		rets.push_back (s);
	    }
	}
	
	return rets;
    }
    
    const std::vector <Symbol> & Table::getAll () const {
	return this-> _syms;
    }
}
