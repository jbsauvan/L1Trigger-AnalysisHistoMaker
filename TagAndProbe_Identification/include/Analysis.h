/**
 *  @file  Analysis.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/30/2014
 *
 *  @internal
 *     Created :  07/30/2014
 * Last update :  07/30/2014 10:10:28 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */




#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "IAnalysis.h"
#include "EventTagAndProbe.h"
#include "EventAware.h"

#include <string>
#include <map>


namespace AnHiMa
{

    class Analysis: public IAnalysis, EventAware<EventTagAndProbe>
    {
        public:


            Analysis();
            ~Analysis();

            bool initialize(const std::string& parameterFile);

            void execute();

        private:
            void fillHistos(int hTag);
            void readShapeLUT(const std::string& file);

        private:
            std::map<unsigned int, unsigned int> m_shapeLUT;

    };
}


#endif
