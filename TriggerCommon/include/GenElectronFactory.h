/**
 *  @file  GenElectronFactory.h
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





#ifndef GENELECTRONFACTORY_H
#define GENELECTRONFACTORY_H

#include <vector>
#include "IObjectFactory.h"
#include "TObjectWithRef.h"
#include "GenParticle.h"
#include "ElectronFactory.h"

namespace AnHiMa
{
    typedef TObjectWithRef<GenParticle, RecoL1Electron> GenElectron;
    typedef std::vector<GenElectron> GenElectronCollection;

    class GenElectronFactory : public IObjectFactory<GenElectronCollection>
    {
        public:
            GenElectronFactory() {};
            ~GenElectronFactory() {};

            virtual void initialize(IEvent*, TChain*);
            void initialize(IEvent*, TChain*, const std::vector<RecoL1Electron>*);

        private:
            virtual void connectVariables(TChain*);
            virtual void update();
            static void callback(void*);

            const std::vector<RecoL1Electron>* m_recoElectrons;

            int                                m_ele_n;
            std::vector<double>                *m_ele_pt;
            std::vector<double>                *m_ele_E;
            std::vector<double>                *m_ele_eta;
            std::vector<double>                *m_ele_phi;
            std::vector<int>                   *m_ele_charge;
            std::vector<int>                   *m_ele_recoMatch;
    };
}


#endif
