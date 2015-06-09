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


class TH2F;

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
            bool passHoE(const L1EGamma& ); // Temporary
            bool passShape(const L1EGamma& , double eff=0.995); // Temporary

        private:
            std::map<unsigned int, unsigned int> m_shapeLUT;
            TH2F* m_cutsHoE; // Temporary
            TH3F* m_discriminantShape; // Temporary
            double m_shapeWorkingPoint; // Temporary

            // calibration wrt 95% eff
            bool m_applyCorr95;
            std::map<int,double> m_corrVsEta95pcStage2;
            std::map<int,double> m_corrVsEta95pcStage1;

            std::vector<int> m_thresholds;
            TTree** m_failAndPassTrees;
            double m_offl_pt[3];
            double m_offl_eta[3];
            int m_failAndPassBits[3][100];
            int m_failAndPassBitsIso[3][100];

    };
}


#endif
