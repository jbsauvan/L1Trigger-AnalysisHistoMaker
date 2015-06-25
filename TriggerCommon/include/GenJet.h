/**
 *  @file  GenJet.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    08/04/2014
 *
 *  @internal
 *     Created :  08/04/2014
 * Last update :  08/04/2014 10:58:33 AM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */





#ifndef GENJET_H
#define GENJET_H

#include "TLorentzVector.h"

namespace AnHiMa
{
    class GenJet: public TLorentzVector
    {
        public:
            GenJet();
            ~GenJet();

            double  emEnergy()  const {return m_emEnergy;}
            double  hadEnergy() const {return m_hadEnergy;}
            double  invEnergy() const {return m_invEnergy;}

            void setEmEnergy  (double emEnergy)  {m_emEnergy = emEnergy;}
            void setHadEnergy (double hadEnergy) {m_hadEnergy = hadEnergy;}
            void setInvEnergy (double invEnergy) {m_invEnergy = invEnergy;}


        private:
            int  m_emEnergy;
            int  m_hadEnergy;
            int  m_invEnergy;
    };
}


#endif

