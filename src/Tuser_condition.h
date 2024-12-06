/***************************************************************************
                          Tuser_condition.h  -  description
                             -------------------
    begin                : Thu Sep 4 2003
    copyright            : (C) 2003 by dr Jerzy Grebosz, IFJ   Cracow
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/
#ifndef TUSER_CONDITION_H
#define TUSER_CONDITION_H


/**
  *@author dr Jerzy Grebosz, IFJ   Cracow
  */
#include "Tcondition_description.h"
#include "Tnamed_pointer.h"

// FOR GLOBAL FUNCITONS ( read gate, read banana)
#include "TjurekPolyCond.h"


//#include "Go4Analysis/TGo4Analysis.h"  // for registering conditions
struct Tcondition_item_1D
{
    bool must_be_valid ;
    Tnamed_pointer named_ptr ;
    double min_value, max_value ;

    int check_item_AND()
    {
        return named_ptr.is_in_AND_gate(min_value, max_value, must_be_valid) ;
    }

    int check_item_OR()
    {
        return named_ptr.is_in_OR_gate(min_value, max_value, must_be_valid) ;
    }

};


class TGalileoAnalysis ;
//////////////////////////////////////////////////////////////////////
class Tuser_condition
{
    std::vector<Tcondition_item_1D>  condition_item_1D_AND ;
    std::vector<Tcondition_item_1D>  condition_item_1D_OR ;

    struct Tcondition_item_2D
    {
        bool must_be_valid ;
        Tnamed_pointer x_named_ptr ;
        Tnamed_pointer y_named_ptr ;
        std::string polygon_name;
        TjurekPolyCond * polygon ;

        //      int check_item_AND()
        //      {
        //        return named_ptr.is_in_AND_gate(min_value, max_value, must_be_valid) ;
        //      }

        //      int check_item_OR()
        //      {
        //        return named_ptr.is_in_OR_gate(min_value, max_value, must_be_valid) ;
        //      }

    };

    std::vector<Tcondition_item_2D>  condition_item_2D_AND ;
    std::vector<Tcondition_item_2D>  condition_item_2D_OR ;


    std::vector< Tuser_condition* > other_condition_AND_ptr;
    std::vector<Tuser_condition* > other_condition_OR_ptr;
    std::vector<Tuser_condition* > other_condition_NAND_ptr;
    std::vector<Tuser_condition* > other_condition_NOR_ptr;

    // for statistics
    int how_many_tested;
    int how_many_true ;


public:

    static TGalileoAnalysis *owner;
    //Tuser_condition();
    /** No descriptions */
    void read_in_parameters(std::string s);
    /**  Checking is done before working with any (all) user spectrum */
    void calculate_value_of_condition();
    /** No descriptions */

    bool check_1D_OR();
    bool check_1D_AND();
    bool check_2D_OR();
    bool check_2D_AND();
    //  /** No descriptions */
    //  bool check_1D_AND_multi(int row);
    //  /** No descriptions */
    //  bool check_1D_AND_single(int row);
    //  /** No descriptions */
    //  bool check_1D_OR_multi(int row);
    //  /** No descriptions */
    //  bool check_1D_OR_single(int row);
    //  /** No descriptions */

    bool check_2D_AND_multi(int row);
    /** No descriptions */
    bool check_2D_AND_single(int row);
    /** No descriptions */
    bool check_2D_OR_multi(int row);
    /** No descriptions */
    bool check_2D_OR_single(int row);

    Tcondition_description   desc ;   // this, what was defined by Cracow

    /** is condition true or false */
    bool result;
    bool give_result()
    {
        if (!already_calculated)
            calculate_value_of_condition() ;
        return   result ;
    }

    std::string give_name()
    {
        return desc.give_name() ;
    }

    void new_event_init()
    {
        already_calculated = false ;
    }

    int give_how_many_tested()
    {
        return how_many_tested;
    }
    int give_how_many_true()
    {
        return how_many_true;
    }


protected: // Protected methods

    void read_in_AND_node();
    void read_in_OR_node();
    void read_in_NAND_node();
    void read_in_NOR_node();

    bool already_calculated ;

    bool check_nodes_NAND();
    bool check_nodes_NOR();
    bool check_nodes_OR();
    bool check_nodes_AND();
};



//////////////////////////////////////////////////////////////////////
#endif
