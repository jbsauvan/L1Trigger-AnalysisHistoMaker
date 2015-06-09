/**
 *  @file  Event.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    15/09/2014
 *
 *  @internal
 *     Created :  15/09/2014
 * Last update :  15/09/2014 17:55:10
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */





#ifndef EVENT_H
#define EVENT_H

#include "EventZeroBias.h"


namespace AnHiMa
{
    class Event: public EventZeroBias
    {
        public:
            Event();
            ~Event();

            bool passSelection(int selection=0);
            void connectVariables(TChain* inputChain);

            const std::vector<const L1EGamma*>& egammasSorted()        const {return m_egammas_Sort;}
            const std::vector<const L1EGamma*>& egammas_FG()           const {return m_egammas_FG;}
            const std::vector<const L1EGamma*>& egammas_HoE()          const {return m_egammas_HoE;}
            const std::vector<const L1EGamma*>& egammas_Shape()        const {return m_egammas_Shape;}
            const std::vector<const L1EGamma*>& egammas_FG_HoE()       const {return m_egammas_FG_HoE;}
            const std::vector<const L1EGamma*>& egammas_FG_HoE_Shape() const {return m_egammas_FG_HoE_Shape;}
            const std::vector<const L1EGamma*>& egammas_ID()           const {return m_egammas_FG_HoE_Shape;}

        private:
            static void callback(void* object);
            void updateSelectedObjects();

        private:
            std::vector<const L1EGamma*> m_egammas_Sort;
            std::vector<const L1EGamma*> m_egammas_FG;
            std::vector<const L1EGamma*> m_egammas_HoE;
            std::vector<const L1EGamma*> m_egammas_Shape;
            std::vector<const L1EGamma*> m_egammas_FG_HoE;
            std::vector<const L1EGamma*> m_egammas_FG_HoE_Shape;

    };
}

#endif
