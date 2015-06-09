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
#include "TH2F.h"
#include "TH3C.h"



namespace AnHiMa
{
    class Event: public EventZeroBias
    {
        public:
            Event();
            ~Event();

            bool passSelection(int selection=0);
            void connectVariables(TChain* inputChain);

            /// Identification and energy correction setters 
            void setShapeLUT(const std::map<unsigned int, unsigned int>& shapeLUT) {m_shapeLUT = shapeLUT;}
            void setCutsHoE(TH2F* cutsHoE) {m_cutsHoE = cutsHoE;}
            void setShapeDiscriminant(TH3F* discriminantShape) {m_discriminantShape = discriminantShape;}
            void setShapeWorkingPoint(float shapeWorkingPoint) {m_shapeWorkingPoint = shapeWorkingPoint;}
            void setApplyCorr95(bool applyCorr95) {m_applyCorr95 = applyCorr95;}
            void setCorrVsEta95pcStage2(const std::map<int,double>& corrVsEta95pcStage2) {m_corrVsEta95pcStage2 = corrVsEta95pcStage2;}
            void setCorrVsEta95pcStage1(const std::map<int,double>& corrVsEta95pcStage1) {m_corrVsEta95pcStage1 = corrVsEta95pcStage1;}

            // Stage-1 sorted objects retriever
            const std::vector<const L1EGamma*>& egammasStage1Sorted()        const {return m_egammasStage1_Sort;}
            const std::vector<const L1EGamma*>& egammasStage1_Iso()        const {return m_egammasStage1_Iso;}

            // Stage-2 sorted objects retriever
            const std::vector<const L1EGamma*>& egammasSorted()        const {return m_egammas_Sort;}
            const std::vector<const L1EGamma*>& egammas_FG()           const {return m_egammas_FG;}
            const std::vector<const L1EGamma*>& egammas_HoE()          const {return m_egammas_HoE;}
            const std::vector<const L1EGamma*>& egammas_Shape()        const {return m_egammas_Shape;}
            const std::vector<const L1EGamma*>& egammas_FG_HoE()       const {return m_egammas_FG_HoE;}
            const std::vector<const L1EGamma*>& egammas_FG_HoE_Shape() const {return m_egammas_FG_HoE_Shape;}
            const std::vector<const L1EGamma*>& egammas_ID()           const {return m_egammas_FG_HoE_Shape;}
            const std::vector<const L1EGamma*>& egammas_Iso()           const {return m_egammas_FG_HoE_Shape_Iso;}
            // Stage-2 EE sorted objects retriever
            const std::vector<const L1EGamma*>& egammas_EE()              const {return m_egammas_EE;}
            const std::vector<const L1EGamma*>& egammas_EE_FG()           const {return m_egammas_EE_FG;}
            const std::vector<const L1EGamma*>& egammas_EE_HoE()          const {return m_egammas_EE_HoE;}
            const std::vector<const L1EGamma*>& egammas_EE_Shape()        const {return m_egammas_EE_Shape;}
            const std::vector<const L1EGamma*>& egammas_EE_FG_HoE()       const {return m_egammas_EE_FG_HoE;}
            const std::vector<const L1EGamma*>& egammas_EE_FG_HoE_Shape() const {return m_egammas_EE_FG_HoE_Shape;}
            const std::vector<const L1EGamma*>& egammas_EE_ID()           const {return m_egammas_EE_FG_HoE_Shape;}
            const std::vector<const L1EGamma*>& egammas_EE_Iso()          const {return m_egammas_EE_FG_HoE_Shape_Iso;}

        private:
            static void callback(void* object);
            void updateSelectedObjects();

            void readShapeLUT(const std::string& file);
            bool passHoE(const L1EGamma& ); // Temporary
            bool passShape(const L1EGamma&, double eff=0.995); // Temporary

        private:
            // Identification and energy correction stuff
            std::map<unsigned int, unsigned int> m_shapeLUT;
            TH2F* m_cutsHoE; // Temporary
            TH3F* m_discriminantShape; // Temporary
            float m_shapeWorkingPoint; // Temporary

            // calibration wrt 95% eff
            bool m_applyCorr95;
            std::map<int,double> m_corrVsEta95pcStage2;
            std::map<int,double> m_corrVsEta95pcStage1;


            // Sorted objects
            // Stage-1
            std::vector<L1EGamma> m_egammasStage1_Ecorr;
            std::vector<const L1EGamma*> m_egammasStage1_Sort;
            std::vector<const L1EGamma*> m_egammasStage1_Iso;
            // Stage-2 
            std::vector<L1EGamma> m_egammas_Ecorr;
            std::vector<const L1EGamma*> m_egammas_Sort;
            std::vector<const L1EGamma*> m_egammas_FG;
            std::vector<const L1EGamma*> m_egammas_HoE;
            std::vector<const L1EGamma*> m_egammas_Shape;
            std::vector<const L1EGamma*> m_egammas_FG_HoE;
            std::vector<const L1EGamma*> m_egammas_FG_HoE_Shape;
            std::vector<const L1EGamma*> m_egammas_FG_HoE_Shape_Iso;
            // Stage-2 in EE
            std::vector<const L1EGamma*> m_egammas_EE;
            std::vector<const L1EGamma*> m_egammas_EE_FG;
            std::vector<const L1EGamma*> m_egammas_EE_HoE;
            std::vector<const L1EGamma*> m_egammas_EE_Shape;
            std::vector<const L1EGamma*> m_egammas_EE_FG_HoE;
            std::vector<const L1EGamma*> m_egammas_EE_FG_HoE_Shape;
            std::vector<const L1EGamma*> m_egammas_EE_FG_HoE_Shape_Iso;
    };
}

#endif
