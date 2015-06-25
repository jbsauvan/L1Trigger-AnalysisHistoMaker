/**
 *  @file  GenJetFactory.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    08/04/2014
 *
 *  @internal
 *     Created :  08/04/2014
 * Last update :  08/04/2014 09:52:55 AM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */





#ifndef GENJETFACTORY_H
#define GENJETFACTORY_H

#include <vector>
#include "IObjectFactory.h"
#include "GenJet.h"

namespace AnHiMa
{
    typedef std::vector<GenJet> GenJetCollection;

    class GenJetFactory : public IObjectFactory<GenJetCollection>
    {
        public:
            GenJetFactory() {};
            ~GenJetFactory() {};

            virtual void initialize(IEvent*, TChain*);

        private:
            virtual void connectVariables(TChain*);
            virtual void update();
            static void callback(void*);


            int                                m_jet_n;
            std::vector<double>                *m_jet_pt;
            std::vector<double>                *m_jet_E;
            std::vector<double>                *m_jet_eta;
            std::vector<double>                *m_jet_phi;
            std::vector<double>                *m_jet_emEnergy;
            std::vector<double>                *m_jet_hadEnergy;
            std::vector<double>                *m_jet_invEnergy;
    };
}


#endif
