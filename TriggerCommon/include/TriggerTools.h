/**
 *  @file  TriggerTools.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/30/2014
 *
 *  @internal
 *     Created :  07/30/2014
 * Last update :  07/30/2014 03:27:22 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */



#ifndef TRIGGERTOOLS_H
#define TRIGGERTOOLS_H

#include "L1EGammaFactory.h"
#include "ElectronFactory.h"
#include "DataFormats/L1TCalorimeter/interface/CaloTower.h"

#include <vector>


namespace AnHiMa
{
    class Electron;
    class GenParticle;

    class TriggerTools
    {
        private:
            TriggerTools() {};
            ~TriggerTools() {};

        public:
            static const RecoL1Electron& match(const GenParticle&, const std::vector<RecoL1Electron>&);
            static const L1EGamma& match(const Electron&, const std::vector<L1EGamma>&, bool rctLevel=false);

            static void printTowers(const std::vector<l1t::CaloTower>& towers, const std::vector< std::pair<int,int> >& highlights);
            static void printClusters(const std::vector<L1EGamma>& clusters, const std::vector< std::pair<int,int> >& highlights);
            static void printStage1Clusters(const std::vector<L1EGamma>& clusters, const std::vector< std::pair<int,int> >& highlights);

            static int getRCTRegionPhi(int ttphi);
            static int getRCTRegionEta(int tteta);
            static unsigned int rctToGtEta(unsigned int iEta);
            static int gtToRctEta(int iEta);

            static unsigned int convertClusterShapeTo8Bits(const l1t::CaloCluster& cluster);
            static std::map<unsigned int, float> towerEnergyFractions(const l1t::CaloCluster& cluster, const std::vector<l1t::CaloTower>& towers);


        private:
            static int overlap(const Electron&, const L1EGamma&);
            static int overlapRCT(const Electron&, const L1EGamma&);
            static int neighbours(const Electron&, const L1EGamma&);
            static int neighboursRCT(const Electron&, const L1EGamma&);

        private:
            static const L1EGamma m_nullEGamma;
            static const RecoL1Electron m_nullRecoElectron;

    };
}


#endif
