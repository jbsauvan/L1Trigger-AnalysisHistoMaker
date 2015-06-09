/**
 *  @file  EventZeroBias.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    15/09/2014
 *
 *  @internal
 *     Created :  15/09/2014
 * Last update :  15/09/2014 17:36:21
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */





#ifndef EVENTZEROBIAS_H
#define EVENTZEROBIAS_H

#include "IEvent.h"
#include "L1TowerFactory.h"
#include "L1EGammaFactory.h"


namespace l1t
{
    class CaloTower;
    class Egamma;
};

namespace AnHiMa
{


    class EventZeroBias: public IEvent
    {
        public:
            EventZeroBias();
            virtual ~EventZeroBias();

            bool passSelection(int selection=0);
            void connectVariables(TChain* inputChain);

            void setEmulatorParameters(const std::string& params);

            int event()  const {return m_event;}
            int run()    const {return m_run;}
            int npv()    const {return m_npv;}
            int npu()    const {return m_npu;}
            double rho() const {return m_rho;}


            const std::vector<l1t::CaloTower>& towers() const {return m_towerFactory.data();}
            const std::vector<L1EGamma>& egammasStage1() const {return m_egammaStage1Factory.data();}
            const std::vector<L1EGamma>& egammasRef() const {return m_egammaRefFactory.data();}
            const std::vector<L1EGamma>& egammas() const {return m_egammaFactory.data();}


        private:
            int    m_event;
            int    m_run;
            int    m_npv;
            int    m_npu;
            double m_rho;

            L1TowerFactory m_towerFactory;
            L1EGammaFactory m_egammaStage1Factory;
            L1EGammaFactory m_egammaRefFactory;
            L1EGammaFactory m_egammaFactory;

            std::string m_emulatorParameters;
    };
}

#endif
