/**
 *  @file  Analysis.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    15/09/2014
 *
 *  @internal
 *     Created :  15/09/2014
 * Last update :  15/09/2014 17:44:25
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */





#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "IAnalysis.h"
#include "Event.h"
#include "EventAware.h"

#include "TRandom3.h"

#include <string>
#include <map>



namespace AnHiMa
{

    class Analysis: public IAnalysis, EventAware<Event>
    {
        public:


            Analysis();
            ~Analysis();

            bool initialize(const std::string& parameterFile);

            void execute();

        private:
            void fillHistos(int hTag);

            void readShapeLUT(const std::string& file);
            bool passHoE(const L1EGamma& ); // Temporary
            bool passShape(const L1EGamma&, double eff=0.995); // Temporary

        private:
            std::map<unsigned int, unsigned int> m_shapeLUT;
            TH2F* m_cutsHoE; // Temporary
            TH3F* m_discriminantShape; // Temporary
            float m_shapeWorkingPoint;

            // calibration wrt 95% eff
            bool m_applyCorr95;
            std::map<int,double> m_corrVsEta95pcStage2;
            std::map<int,double> m_corrVsEta95pcStage1;
    };
}


#endif
