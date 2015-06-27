/**
 *  @file  EventTagAndProbeWithJetsEGamma.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/29/2014
 *
 *  @internal
 *     Created :  07/29/2014
 * Last update :  07/29/2014 06:13:15 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */



#include <iostream>

#include <TChain.h>

#include "EventTagAndProbeWithJets.h"


using namespace AnHiMa;
using namespace std;

/*****************************************************************/
EventTagAndProbeWithJets::EventTagAndProbeWithJets():EventTagAndProbe()
/*****************************************************************/
{

}



/*****************************************************************/
EventTagAndProbeWithJets::~EventTagAndProbeWithJets()
/*****************************************************************/
{
}


/*****************************************************************/
void EventTagAndProbeWithJets::connectVariables(TChain* inputChain)
/*****************************************************************/
{
    EventTagAndProbe::connectVariables(inputChain);

    m_genJetFactory.initialize(this, inputChain);
}


/*****************************************************************/
bool EventTagAndProbeWithJets::passSelection(int selection)
/*****************************************************************/
{
    return EventTagAndProbe::passSelection(selection);
}


/*****************************************************************/
void EventTagAndProbeWithJets::update()
/*****************************************************************/
{
    EventTagAndProbe::update();
    removeJetElectronOverlap();
}

/*****************************************************************/
void EventTagAndProbeWithJets::removeJetElectronOverlap()
/*****************************************************************/
{
    m_genJetsWoElectrons.clear();
    for(const auto& jet : genJets())
    {
        double mindr = 999.;
        for(const auto& ele : genElectrons())
        {
            double dr = jet.DeltaR(ele);
            if(dr<mindr) mindr = dr;
        }       
        if(jet.Pt()>30 && abs(jet.Eta())<4.5 && !(jet.emEnergy()/jet.Energy()>0.5 && mindr<0.4)) m_genJetsWoElectrons.push_back(&jet);
    }

}



