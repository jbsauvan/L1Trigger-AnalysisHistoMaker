/**
 *  @file  Event.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    15/09/2014
 *
 *  @internal
 *     Created :  15/09/2014
 * Last update :  15/09/2014 17:56:17
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */







#include <iostream>
#include "Event.h"


using namespace AnHiMa;
using namespace std;

/*****************************************************************/
Event::Event():EventZeroBias()
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
    EventZeroBias::connectVariables(inputChain);

    registerCallback((void*)this, Event::callback);
}


/*****************************************************************/
bool Event::passSelection(int selection)
/*****************************************************************/
{
    return EventZeroBias::passSelection(selection);
}


/*****************************************************************/
void Event::callback(void* object)
/*****************************************************************/
{
    Event* myself = reinterpret_cast<Event*>(object);
    myself->updateSelectedObjects();
}


/*****************************************************************/
void Event::updateSelectedObjects()
/*****************************************************************/
{
    m_egammas_Sort.clear();
    m_egammas_FG.clear();
    m_egammas_HoE.clear();
    m_egammas_Shape.clear();
    m_egammas_FG_HoE.clear();
    m_egammas_FG_HoE_Shape.clear();

    for(auto egItr=egammas().cbegin(); egItr!=egammas().end(); egItr++ )
    {
        const L1EGamma& l1eg = *egItr;
        if(abs(l1eg.hwEta()))
        m_egammas_Sort.push_back(&l1eg);
    }
    sort(m_egammas_Sort.begin(), m_egammas_Sort.end(), L1EGammaSort);

    for(auto egItr=egammasSorted().cbegin(); egItr!=egammasSorted().end(); egItr++ )
    {
        const L1EGamma* l1eg = *egItr;
        if(l1eg->hwQual()&0x1) // check FG bit 
        {
            m_egammas_FG.push_back(l1eg);
        }
        if(l1eg->hwQual()&0x2) // check H/E bit 
        {
            m_egammas_HoE.push_back(l1eg);
        }
        if(l1eg->hwQual()&0x4) // check shape bit 
        {
            m_egammas_Shape.push_back(l1eg);
        }
        if(l1eg->hwQual()&0x1 && l1eg->hwQual()&0x2) // check FG and H/E bits
        {
            m_egammas_FG_HoE.push_back(l1eg);
        }
        if(l1eg->hwQual()&0x1 && l1eg->hwQual()&0x2 && l1eg->hwQual()&0x4) // check FG, H/E and shape bits
        {
            m_egammas_FG_HoE_Shape.push_back(l1eg);
        }
    }
}


