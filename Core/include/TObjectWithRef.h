/**
 *  @file  TObjectWithRef.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/30/2014
 *
 *  @internal
 *     Created :  07/30/2014
 * Last update :  07/30/2014 12:02:23 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */



#ifndef TOBJECTWITHREF_H
#define TOBJECTWITHREF_H


#include <string>
#include <map>

namespace AnHiMa
{
    template<class Tbase, class Tref> class TObjectWithRef: public Tbase
    {
        public:
            // no default constructor. Otherwise the reference would be invalid.
            TObjectWithRef(const Tref& ref):Tbase(),m_ref(ref) {};
            TObjectWithRef(const Tbase& base, const Tref& ref):Tbase(base),m_ref(ref) {};
            TObjectWithRef(const TObjectWithRef& obj):Tbase(obj),m_ref(obj.ref()), m_parameters(obj.parameters()) {};
            ~TObjectWithRef() {};

            void setParameter(const std::string& key, const std::string& value) {m_parameters[key] = value;}

            const Tref& ref() const {return m_ref;}
            const std::map<std::string, std::string>& parameters() const {return m_parameters;}
            const std::string parameter(const std::string& key) const
            {
                auto parItr = m_parameters.find(key);
                if(parItr!=m_parameters.end()) return parItr->second;
                return std::string("");
            }

        private:
            const Tref& m_ref;
            std::map<std::string, std::string> m_parameters;
    };

}


#endif
