#ifndef TJUREKABSTRACTEVENTELEMENT_H
#define TJUREKABSTRACTEVENTELEMENT_H

/**
	@author Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/

#include <string>
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TjurekAbstractEventElement{
 std::string name;
public:
    TjurekAbstractEventElement ( std::string name_ ) : name ( name_ ) {}

    ~TjurekAbstractEventElement();

};

#endif
