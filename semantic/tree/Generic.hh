#pragma once

#include <vector>
#include "Tree.hh"

namespace semantic {    
    class IInfoType;
    typedef IInfoType* InfoType;
}

namespace Ymir {    
        
    Tree makeField (semantic::InfoType type, std::string name);
    
    Tree getField (location_t locus, Tree obj, std::string name);
    
    Tree makeTuple (std::string name, std::vector <semantic::InfoType> types, std::vector<std::string> attrs);

    Tree makeStructType (std::string name, int nbFields, ...);
    
    Tree makeAuxVar (location_t locus, ulong id, Ymir::Tree type);

    Tree getArrayRef (location_t, Tree array, Tree inner, ulong index);

    Tree getPointerUnref (location_t, Tree ptr, Tree inner, ulong index);

    Tree getArrayRef (location_t, Tree array, Tree inner, Tree index);

    Tree getPointerUnref (location_t, Tree ptr, Tree inner, Tree index);
    
}
