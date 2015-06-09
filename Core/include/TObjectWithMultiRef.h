/**
 *  @file  TObjectWithMultiRef.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    08/04/2014
 *
 *  @internal
 *     Created :  08/04/2014
 * Last update :  08/04/2014 04:56:46 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */





#ifndef TOBJECTWITHMULTIREF_H
#define TOBJECTWITHMULTIREF_H

#include <vector>

namespace AnHiMa
{
    template<class Tbase, class Tref> class TObjectWithMultiRef: public Tbase
    {
        public:
            // no default constructor. At least one ref is mandatory
            TObjectWithMultiRef(const Tref& ref):Tbase(),m_refs(1) {m_refs[0]=&ref;};
            TObjectWithMultiRef(const std::vector<const Tref*>& refs):Tbase(),m_refs(refs) {};
            TObjectWithMultiRef(const Tbase& base, const Tref& ref):Tbase(base),m_refs(1) {m_refs[0]=&ref;};
            TObjectWithMultiRef(const Tbase& base, const std::vector<const Tref*>& refs):Tbase(base),m_refs(refs) {};
            TObjectWithMultiRef(const TObjectWithMultiRef& obj):Tbase(obj),m_refs(obj.refs()) {};
            ~TObjectWithMultiRef() {};

            unsigned int nRefs() const {return m_refs.size();}
            const Tref& ref(unsigned int i=0) const {return (i<m_refs.size() ? *m_refs[i] : *m_refs.back());}
            const std::vector<const Tref*>& refs() const {return m_refs;}

        private:
            std::vector<const Tref*> m_refs;
    };

}


#endif
