/**
 *  @file  L1EGammaFactory.h
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



#ifndef L1EGAMMAFACTORY_H
#define L1EGAMMAFACTORY_H

#include <vector>
#include "TObjectWithRef.h"
#include "IObjectFactory.h"
#include "L1TStage2Wrapper.h"

namespace AnHiMa
{
    typedef TObjectWithRef<l1t::EGamma, l1t::CaloCluster> L1EGamma;
    typedef std::vector<L1EGamma> L1EGammaCollection;

    bool L1EGammaSort(const L1EGamma* eg1, const L1EGamma* eg2);


    //extern std::auto_ptr<l1t::CaloCluster> g_nullCluster;
    class L1TowerFactory;

    class L1EGammaFactory : public IObjectFactory<L1EGammaCollection>
    {
        public:
            enum EGType
            {
                STAGE1_FROMTREE = 0,
                STAGE2_FROMTREE = 1,
                STAGE2_REBUILD = 2
            };

        public:
            L1EGammaFactory() {};
            ~L1EGammaFactory() {};

            virtual void initialize(IEvent*, TChain*);
            void initialize(IEvent*, TChain*, EGType, const std::string&, const std::vector<l1t::CaloTower>* towers);

        private:
            virtual void connectVariables(TChain*);
            virtual void update();
            static void callback(void*);

            EGType m_type;
            const std::vector<l1t::CaloTower>* m_towers;
            l1twrapper::L1TStage2Wrapper m_emulatorWrapper;

            int                                m_l1t_eg_n;
            std::vector<double>                *m_l1t_eg_pt;
            std::vector<double>                *m_l1t_eg_eta;
            std::vector<double>                *m_l1t_eg_phi;
            std::vector<int>                   *m_l1t_eg_hwPt;
            std::vector<int>                   *m_l1t_eg_hwEta;
            std::vector<int>                   *m_l1t_eg_hwPhi;
            std::vector<int>                   *m_l1t_eg_hwQual;
            std::vector<int>                   *m_l1t_eg_hwIso;
    };
}


#endif
