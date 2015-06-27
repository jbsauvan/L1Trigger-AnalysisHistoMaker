/**
 *  @file  EventTagAndProbeWithJets.h
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





#ifndef EVENTTAGANDPROBEWITHJETS_H
#define EVENTTAGANDPROBEWITHJETS_H

#include "EventTagAndProbe.h"
#include "GenJetFactory.h"


namespace AnHiMa
{


    class EventTagAndProbeWithJets: public EventTagAndProbe
    {
        public:
            EventTagAndProbeWithJets();
            virtual ~EventTagAndProbeWithJets();

            bool passSelection(int selection=0);
            void connectVariables(TChain* inputChain);
            void update();
            
            const std::vector<GenJet>& genJets() const {return m_genJetFactory.data();}
            const std::vector<const GenJet*>& genJetsWoElectrons() const {return m_genJetsWoElectrons;}

        private:
            void removeJetElectronOverlap();

            GenJetFactory m_genJetFactory;
            std::vector<const GenJet*> m_genJetsWoElectrons;


    };
}

#endif
