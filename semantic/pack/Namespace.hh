#pragma once

#include <ymir/utils/Array.hh>
#include <string>

namespace semantic {

    struct Namespace {
    private:
	
	std::vector <std::string> names;

	Namespace () {}
       	
    public:
	
	Namespace (std::string name);

	Namespace (const Namespace &space, std::string name);

	Namespace (const std::vector <std::string> &names);
	
	friend bool operator== (const Namespace &self, const Namespace &other);

	friend bool operator!= (const Namespace &self, const Namespace &other);

	const std::vector <std::string> & innerMods ();
	
	bool isSubOf (const Namespace& other) const;

	bool isAbsSubOf (const Namespace& other) const;

	std::string toString () const;
	
    };
    
}
