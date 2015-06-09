/**
 *  @file  L1TowerFactory.h
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



#ifndef L1TOWERFACTORY_H
#define L1TOWERFACTORY_H

#include <vector>
#include "IObjectFactory.h"
#include "L1TStage2Wrapper.h"

namespace AnHiMa
{
    typedef std::vector<l1t::CaloTower> L1TowerCollection;



    class L1TowerFactory : public IObjectFactory<L1TowerCollection>
    {
        public:
            L1TowerFactory() {};
            ~L1TowerFactory() {};

            virtual void initialize(IEvent*, TChain*);

        private:
            virtual void connectVariables(TChain*);
            virtual void update();
            static void callback(void*);


            int                                m_l1t_tower_n;
            std::vector<int>                   *m_l1t_tower_hwEta;
            std::vector<int>                   *m_l1t_tower_hwPhi;
            std::vector<int>                   *m_l1t_tower_hwEtEm;
            std::vector<int>                   *m_l1t_tower_hwEtHad;
            std::vector<int>                   *m_l1t_tower_hwEtRatio;
            std::vector<int>                   *m_l1t_tower_hwQual;

    };
}


#endif
