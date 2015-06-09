
#ifndef TDECAYEDPARTICLE_H
#define TDECAYEDPARTICLE_H

#include <vector>
#include "TLorentzVector.h"


namespace AnHiMa
{
    template<class T, class Tbase=TLorentzVector> class TDecayedParticle: public Tbase
    {
        public:
            TDecayedParticle(const TDecayedParticle<T,Tbase>& p):Tbase(p)
            {
                m_constituents.push_back(&p[0]);
                m_constituents.push_back(&p[1]);
            }
            TDecayedParticle(const T& p1, const T& p2):Tbase(p1)
            {
                (*this) += static_cast<const Tbase&>(p2); 
                m_constituents.push_back(&p1);
                m_constituents.push_back(&p2);
            }
            ~TDecayedParticle() {};


            const T& operator[](std::size_t i) const {return *(m_constituents[i]);}
            const T& decay(std::size_t i) const {return *(m_constituents[i]);}

        private:
            std::vector<const T*> m_constituents;
    };
}

#endif
