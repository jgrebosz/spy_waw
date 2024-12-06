#ifndef TJUREKPOLYCOND_H
#define TJUREKPOLYCOND_H

/**
    @author Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/

#include <string>
#include <vector>

///////////////////////////////////////////////////////////////////////////////////////////
class TjurekPolyCond
{
    std::string name;
    int how_many_points;
    std::vector<double> x_points;
    std::vector<double> y_points;

    bool enabled;
public:
    TjurekPolyCond();
    TjurekPolyCond(std::string name_, std::string title = "Go4 polygon condition")
    : name(name_)
    { }
    
    

    ~TjurekPolyCond();

    /** Test if X,Y are inside. */
    //virtual
    bool Test(double x, double y);

    /** Delete old cut and create a new cut with X,Y values. */
    //virtual
    void SetValues(double * x, double * y, int len);
    //----------------
    void Enable()
    {
        enabled = true;
    }
    std::string give_name() { return name;}


private:
    bool IsInside(double xp, double yp) ;
};

#endif
