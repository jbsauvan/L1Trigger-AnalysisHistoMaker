/**
 *  @file  EventTagAndProbeEGamma.cpp
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

#include "EventTagAndProbe.h"


using namespace AnHiMa;
using namespace std;

/*****************************************************************/
EventTagAndProbe::EventTagAndProbe():EventEGamma()
/*****************************************************************/
{

}



/*****************************************************************/
EventTagAndProbe::~EventTagAndProbe()
/*****************************************************************/
{
}


/*****************************************************************/
void EventTagAndProbe::connectVariables(TChain* inputChain)
/*****************************************************************/
{
    EventEGamma::connectVariables(inputChain);


}


/*****************************************************************/
bool EventTagAndProbe::passSelection(int selection)
/*****************************************************************/
{
    if(m_ZCandidates.size()==0) return false;
    return true;
}


/*****************************************************************/
void EventTagAndProbe::update()
/*****************************************************************/
{
    EventEGamma::update();
    fillTagAndProbe();
}


/*****************************************************************/
void EventTagAndProbe::fillTagAndProbe()
/*****************************************************************/
{
    m_ZCandidates.clear();
    // keep "good" electrons
    std::vector<const RecoL1Electron*> selectedElectrons;
    for(auto eleItr=electrons().cbegin(); eleItr!=electrons().end();eleItr++)
    {
        const RecoL1Electron& electron = *eleItr;
        if(electron.Pt()<7.) continue;
        if(fabs(electron.superClusterEta())>2.5) continue;
        if(!electron.pass()) continue;

        selectedElectrons.push_back(&electron);
    }
    sort(selectedElectrons.begin(), selectedElectrons.end(), RecoL1ElectronSort); 
    reverse(selectedElectrons.begin(), selectedElectrons.end());
    // keep only the 2 leading electrons
    if(selectedElectrons.size()>2) selectedElectrons.resize(2);

    // build pairs from the 2 leading good electrons
    // Tag is electron1, Probe is electron2. A given electron is allowed to be the tag or the probe
    for(auto eleItr1=selectedElectrons.cbegin();eleItr1!=selectedElectrons.end(); ++eleItr1)
    {
        const RecoL1Electron& electron1 = **eleItr1;
        // Tag requirements
        if(electron1.Pt()<20.) continue;
        if( fabs(electron1.superClusterEta())>1.4442 && fabs(electron1.superClusterEta())<1.566 ) continue; // remove tags in the transition region ??
        // maybe require more stringent ID cut
        for(auto eleItr2=selectedElectrons.cbegin();eleItr2!=selectedElectrons.end(); ++eleItr2)
        {
            if(eleItr1==eleItr2) continue; 
            const RecoL1Electron& electron2 = **eleItr2;       
            TDecayedParticle<RecoL1Electron> candidate(electron1, electron2);

            // pair requirements
            if(electron1.charge()*electron2.charge()>=0) continue;
            if(candidate.M()<70. || candidate.M()>110.) continue;

            m_ZCandidates.push_back( candidate );
        }
    }

}

