/**
 *  @file  Event.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/29/2014
 *
 *  @internal
 *     Created :  07/29/2014
 * Last update :  07/29/2014 09:41:18 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */





#include <iostream>
#include "Event.h"


using namespace AnHiMa;
using namespace std;

/*****************************************************************/
Event::Event():EventEGamma()
/*****************************************************************/
{


}



/*****************************************************************/
Event::~Event()
/*****************************************************************/
{
}


/*****************************************************************/
void Event::connectVariables(TChain* inputChain)
/*****************************************************************/
{
    EventEGamma::connectVariables(inputChain);
}


/*****************************************************************/
bool Event::passSelection(int selection)
/*****************************************************************/
{
    return true;
}

