/**
 *  @file  ElectronFactory.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/29/2014
 *
 *  @internal
 *     Created :  07/29/2014
 * Last update :  07/29/2014 03:05:55 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */



#ifndef ELECTRONFACTORY_H
#define ELECTRONFACTORY_H

#include <vector>
#include "IObjectFactory.h"
#include "L1EGammaFactory.h"
#include "TObjectWithMultiRef.h"
#include "Electron.h"

namespace AnHiMa
{
    typedef TObjectWithMultiRef<Electron, L1EGamma> RecoL1Electron;
    typedef std::vector<RecoL1Electron> ElectronCollection;

    bool RecoL1ElectronSort(const RecoL1Electron* eg1, const RecoL1Electron* eg2);

    class ElectronFactory : public IObjectFactory<ElectronCollection>
    {
        public:
            ElectronFactory() {};
            ~ElectronFactory() {};

            virtual void initialize(IEvent*, TChain*);
            void initialize(IEvent*, TChain*, const std::vector< const std::vector<L1EGamma>*>&);

        private:
            virtual void connectVariables(TChain*);
            virtual void update();
            static void callback(void*);

            std::vector< const std::vector<L1EGamma>*> m_l1EGammas;

            int                                m_ele_n;
            std::vector<double>                *m_ele_pt;
            std::vector<double>                *m_ele_et;
            std::vector<double>                *m_ele_E;
            std::vector<double>                *m_ele_eta;
            std::vector<double>                *m_ele_phi;
            std::vector<int>                   *m_ele_charge;
            std::vector<double>                *m_ele_sc_E;
            std::vector<double>                *m_ele_sc_eta;
            std::vector<double>                *m_ele_sc_phi;
            std::vector<int>                   *m_ele_ncl;
            std::vector<int>                   *m_ele_class;
            std::vector<bool>                  *m_ele_isEB;
            std::vector<bool>                  *m_ele_isEE;
            std::vector<bool>                  *m_ele_isEBEEGap;
            std::vector<bool>                  *m_ele_isEBGap;
            std::vector<bool>                  *m_ele_isEEGap;
            std::vector<double>                *m_ele_bdt;
            std::vector<bool>                  *m_ele_pass;
            std::vector<int>                   *m_ele_seedtt_ieta;
            std::vector<int>                   *m_ele_seedtt_iphi;
            std::vector< std::vector<int> >    *m_ele_tt_ieta;
            std::vector< std::vector<int> >    *m_ele_tt_iphi;
            std::vector< std::vector<float> >  *m_ele_tt_E;
            std::vector< std::vector<int> >    *m_ele_rct_ieta;
            std::vector< std::vector<int> >    *m_ele_rct_iphi;

    };
}


#endif
