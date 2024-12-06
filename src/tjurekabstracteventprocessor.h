#ifndef TJUREKABSTRACTEVENTPROCESSOR_H
#define TJUREKABSTRACTEVENTPROCESSOR_H

/**
    @author Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/

// abstract class which represents the step of analysis

#include <string>
////////////////////////////////////////////////////////////////////////////////////////////////////
class TjurekAbstractEventProcessor
{
        std::string name;
    public:
//    TjurekAbstractEventProcessor();
        TjurekAbstractEventProcessor ( std::string name_ ) : name ( name_ ) {} ;
        ~TjurekAbstractEventProcessor();

        virtual void preLoop() =0;
        virtual void postLoop() =0;
        void *   GetInputEvent() { return NULL; }
        virtual void Process() = 0;
    private:

};

#endif
