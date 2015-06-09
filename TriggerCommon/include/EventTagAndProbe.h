/**
 *  @file  EventTagAndProbe.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/30/2014
 *
 *  @internal
 *     Created :  07/30/2014
 * Last update :  07/30/2014 09:14:39 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */





#ifndef EVENTTAGANDPROBE_H
#define EVENTTAGANDPROBE_H

#include "EventEGamma.h"
#include "TDecayedParticle.h"


namespace AnHiMa
{


    class EventTagAndProbe: public EventEGamma
    {
        public:
            EventTagAndProbe();
            virtual ~EventTagAndProbe();

            bool passSelection(int selection=0);
            void connectVariables(TChain* inputChain);
            void update();
            
            const std::vector< TDecayedParticle<RecoL1Electron> >& ZCandidates() const {return m_ZCandidates;}

        private:
            void fillTagAndProbe();

            std::vector< TDecayedParticle<RecoL1Electron> > m_ZCandidates;


    };
}

#endif
