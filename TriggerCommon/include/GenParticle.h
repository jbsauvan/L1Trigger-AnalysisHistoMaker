/**
 *  @file  GenParticle.h
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





#ifndef GENPARTICLE_H
#define GENPARTICLE_H

#include "TLorentzVector.h"

namespace AnHiMa
{
    class GenParticle: public TLorentzVector
    {
        public:
            GenParticle();
            ~GenParticle();

            int  charge()    const {return m_charge;}
            int  pid()       const {return m_pid;}
            int  recoMatch() const {return m_recoMatch;}

            void setCharge    (int charge)      {m_charge = charge;}
            void setPid       (int pid)        {m_pid = pid;}
            void setRecoMatch (int recoMatch) {m_recoMatch = recoMatch;}


        private:
            int  m_charge;
            int  m_pid;
            int  m_recoMatch;
    };
}


#endif

