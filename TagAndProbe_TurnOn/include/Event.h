/**
 *  @file  Event.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/29/2014
 *
 *  @internal
 *     Created :  07/29/2014
 * Last update :  07/29/2014 09:41:33 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */







#ifndef EVENT_H
#define EVENT_H

#include "EventEGamma.h"


namespace AnHiMa
{
    class Event: public EventEGamma
    {
        public:
            Event();
            ~Event();

            bool passSelection(int selection=0);
            void connectVariables(TChain* inputChain);

        private:

    };
}

#endif
