/**
 *  @file  TriggerTools.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/30/2014
 *
 *  @internal
 *     Created :  07/30/2014
 * Last update :  07/30/2014 03:41:32 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */



#include "TriggerTools.h"
#include "Electron.h"
#include "GenParticle.h"

#include "L1Trigger/L1TCalorimeter/interface/CaloStage2Nav.h"
#include "L1Trigger/L1TCalorimeter/interface/CaloTools.h"

#include <algorithm>
#include <set>
#include <iostream>
#include <sstream>
#include <bitset>

using namespace AnHiMa;
using namespace std;

const AnHiMa::L1EGamma AnHiMa::TriggerTools::m_nullEGamma( l1t::CaloTools::getCluster(vector<l1t::CaloCluster>(), 1, 1) );
const AnHiMa::RecoL1Electron AnHiMa::TriggerTools::m_nullRecoElectron( AnHiMa::TriggerTools::m_nullEGamma );


/*****************************************************************/
const RecoL1Electron& TriggerTools::match(const GenParticle& electron, const vector<RecoL1Electron>& recoElectrons)
/*****************************************************************/
{
    int recoIndex = electron.recoMatch();
    if(recoIndex==-1) return m_nullRecoElectron;
    if(recoIndex>=(int)recoElectrons.size()) 
    {
        return m_nullRecoElectron;
        cout<<"WARNING: reco electron index matched to gen electron is larger than number of reco electrons\n";
    }
    return recoElectrons[recoIndex];
}


/*****************************************************************/
const L1EGamma& TriggerTools::match(const Electron& electron, const vector<L1EGamma>& egammas, bool rctLevel)
/*****************************************************************/
{
    vector<L1EGamma>::const_iterator bestMatch = egammas.end();
    int maxOverlaps = 0;
    stringstream buffer;
    // First try to find overlapping towers/RCT between L1 and reco objects
    for(auto egItr=egammas.cbegin(); egItr!=egammas.end(); ++egItr)
    {
        int nOverlaps = (rctLevel ? TriggerTools::overlapRCT(electron, *egItr) : TriggerTools::overlap(electron, *egItr));
        //if(rctLevel)
        //{
        //    cout<<"  Trying to match electron ieta="<<electron.listRCTIeta()[0]<<", iphi="<<electron.listRCTIphi()[0]<<" with L1 cluster ieta="<<egItr->hwEta()<<", iphi="<<egItr->hwPhi()<<", flags="<<std::bitset<15>(egItr->ref().clusterFlags())<<"\n";
        //    cout<<"   size="<<electron.listTTIeta().size()<<", ET="<<electron.Pt()<<" -> Overlap="<<nOverlaps<<"\n";
        //}
        if(!rctLevel) // matching at tower level
        {
            if(nOverlaps>maxOverlaps)
            {
                bestMatch = egItr;
                maxOverlaps = nOverlaps;
            }
            else if(bestMatch!=egammas.end() && nOverlaps==maxOverlaps)
            {
                if(egItr->pt()>bestMatch->pt())
                {
                    bestMatch = egItr;
                    maxOverlaps = nOverlaps;
                }
            }
        }
        else // matching at RCT level
        {
            if( nOverlaps>0 && (bestMatch==egammas.end() || egItr->pt()>bestMatch->pt()) )
            {
                bestMatch = egItr;
                maxOverlaps = nOverlaps;
            }
        }
    }

    vector<L1EGamma>::const_iterator bestMatch2 = egammas.end();
    if(!rctLevel)
    {
        //if(bestMatch==egammas.end())
        //{

        // Try to look at the electron towers around each L1 cluster seed
        // keep only those with a number of matched neighbours larger than the number of overlaps previously found
        for(auto egItr=egammas.cbegin(); egItr!=egammas.end(); ++egItr)
        {
            int neighbours = TriggerTools::neighbours(electron, *egItr);
            if(neighbours>0)
            {
                bestMatch2 = egItr;
                maxOverlaps = neighbours;
            }
            else if(bestMatch!=egammas.end() && neighbours==maxOverlaps)
            {
                if(egItr->pt()>bestMatch2->pt())
                {
                    bestMatch2 = egItr;
                    maxOverlaps = neighbours;
                }
            }
        }
        //}
    }
    else
    {
        if(bestMatch==egammas.end())
        {
            for(auto egItr=egammas.cbegin(); egItr!=egammas.end(); ++egItr)
            {
                int neighbours = TriggerTools::neighboursRCT(electron, *egItr);
                if(neighbours>0 && (bestMatch2==egammas.end() || egItr->pt()>bestMatch2->pt()))
                {
                    bestMatch2 = egItr;
                    maxOverlaps = neighbours;
                }
            }
        }
    }
    // if the two methods give a match, take the one with better pt estimate
    if(bestMatch!=egammas.end() && bestMatch2!=egammas.end())
    {
        if( fabs(bestMatch2->pt() - electron.Pt()) < fabs(bestMatch->pt() - electron.Pt())  ) bestMatch=bestMatch2;
    }
    // if the cluster does not overlap with the reco electron, take the neighbour match
    if(bestMatch==egammas.end() && bestMatch2!=egammas.end())
    {
        bestMatch = bestMatch2;
    }

    //if(rctLevel)
    //{
    //    cout<<"  Max overlap = "<<maxOverlaps<<"\n";
    //    //if(maxOverlaps==0 && electron.Pt()>20.)
    //    //{
    //        //cout<<buffer.str();
    //        //cout<<"   List of electron towers:\n   ";
    //        //    for(unsigned int i=0;i<electron.listTTIeta().size(); i++)
    //        //    {
    //        //        // discard towers with negative energy
    //        //        if(electron.listTTEnergy()[i]<=0.) continue;

    //        //        int ie = electron.listTTIeta()[i];
    //        //        int ip = electron.listTTIphi()[i];
    //        //        cout<<"("<<ie<<","<<ip<<") ";
    //        //    }
    //        cout<<"\n\n\n";
    //    //}
    //}

    if(bestMatch==egammas.end()) return AnHiMa::TriggerTools::m_nullEGamma;
    return *bestMatch;
}



/*****************************************************************/
int TriggerTools::overlap(const Electron& electron, const L1EGamma& egamma)
/*****************************************************************/
{
    int nOverlaps = 0;
    const l1t::CaloCluster& cluster = egamma.ref();
    int iEta = egamma.hwEta();
    int iPhi = egamma.hwPhi();
    set< pair<int,int> > listIetaIphi;
    l1t::CaloStage2Nav caloNav;
    int iEtaP  = caloNav.offsetIEta(iEta, 1);
    int iEtaM  = caloNav.offsetIEta(iEta, -1);
    int iPhiP  = caloNav.offsetIEta(iPhi, 1);
    int iPhiM  = caloNav.offsetIEta(iPhi, -1);
    int iPhiP2 = caloNav.offsetIEta(iPhi, 2);
    int iPhiM2 = caloNav.offsetIEta(iPhi, -2);

    listIetaIphi.insert( std::make_pair(iEta,iPhi) ); // seed always included
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_NW)) listIetaIphi.insert( std::make_pair(iEtaM,iPhiM ) );
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_N )) listIetaIphi.insert( std::make_pair(iEta ,iPhiM ) );
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_NE)) listIetaIphi.insert( std::make_pair(iEtaP,iPhiM ) );
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_E )) listIetaIphi.insert( std::make_pair(iEtaP,iPhi  ) );
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_SE)) listIetaIphi.insert( std::make_pair(iEtaP,iPhiP ) );
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_S )) listIetaIphi.insert( std::make_pair(iEta ,iPhiP ) );
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_SW)) listIetaIphi.insert( std::make_pair(iEtaM,iPhiP ) );
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_NN)) listIetaIphi.insert( std::make_pair(iEta ,iPhiP2) );
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_SS)) listIetaIphi.insert( std::make_pair(iEta ,iPhiM2) );

    for(unsigned int i=0;i<electron.listTTIeta().size(); i++)
    {
        // discard towers with negative energy
        if(electron.listTTEnergy()[i]<=0.) continue;

        int ie = electron.listTTIeta()[i];
        int ip = electron.listTTIphi()[i];
        if(listIetaIphi.find( std::make_pair(ie,ip) )!=listIetaIphi.end() )
        {
            nOverlaps++;
        }
    }
    return nOverlaps;
}

/*****************************************************************/
int TriggerTools::overlapRCT(const Electron& electron, const L1EGamma& egamma)
/*****************************************************************/
{
    int nOverlaps = 0;
    int iEta = TriggerTools::gtToRctEta(egamma.hwEta());
    int iPhi = egamma.hwPhi();

    //cout<<"Electron RCTs: ";
    for(unsigned int i=0;i<electron.listRCTIeta().size(); i++)
    {
        int ie = electron.listRCTIeta()[i];
        int ip = electron.listRCTIphi()[i];
        //cout<<"("<<ie<<","<<ip<<") ";
        if(ie==iEta && ip==iPhi )
        {
            nOverlaps++;
        }
    }
    //cout<<"\n";
    return nOverlaps;
}


/*****************************************************************/
int TriggerTools::neighbours(const Electron& electron, const L1EGamma& egamma)
/*****************************************************************/
{
    int neighbours = 0;
    int iEta = egamma.hwEta();
    int iPhi = egamma.hwPhi();

    l1t::CaloStage2Nav caloNav;
    int iEtaP  = caloNav.offsetIEta(iEta, 1);
    int iEtaM  = caloNav.offsetIEta(iEta, -1);
    int iPhiP  = caloNav.offsetIEta(iPhi, 1);
    int iPhiM  = caloNav.offsetIEta(iPhi, -1);

    set< pair<int,int> > listIetaIphi;
    listIetaIphi.insert( std::make_pair(iEta ,iPhi  ) ); 
    listIetaIphi.insert( std::make_pair(iEtaM,iPhiM ) );
    listIetaIphi.insert( std::make_pair(iEta ,iPhiM ) );
    listIetaIphi.insert( std::make_pair(iEtaP,iPhiM ) );
    listIetaIphi.insert( std::make_pair(iEtaP,iPhi  ) );
    listIetaIphi.insert( std::make_pair(iEtaP,iPhiP ) );
    listIetaIphi.insert( std::make_pair(iEta ,iPhiP ) );
    listIetaIphi.insert( std::make_pair(iEtaM,iPhiP ) );
    listIetaIphi.insert( std::make_pair(iEtaM,iPhi  ) );

    for(unsigned int i=0;i<electron.listTTIeta().size(); i++)
    {
        // discard towers with negative energy
        if(electron.listTTEnergy()[i]<=0.) continue;

        int ie = electron.listTTIeta()[i];
        int ip = electron.listTTIphi()[i];
        if(listIetaIphi.find( std::make_pair(ie,ip) )!=listIetaIphi.end() )
        {
            neighbours++;
        }
    }
    return neighbours;
}

/*****************************************************************/
int TriggerTools::neighboursRCT(const Electron& electron, const L1EGamma& egamma)
/*****************************************************************/
{
    int nOverlaps = 0;
    int iEta = TriggerTools::gtToRctEta(egamma.hwEta());
    int iPhi = egamma.hwPhi();

    l1t::CaloStage2Nav caloNav;
    // loop over electron trigger towers
    //cout<<"Try to match ("<<iEta<<", "<<iPhi<<") with neighbours\n";
    for(unsigned int i=0;i<electron.listTTIeta().size(); i++)
    {
        int ie = electron.listTTIeta()[i];
        int ip = electron.listTTIphi()[i];
        int ieP  = caloNav.offsetIEta(ie, 1);
        int ieM  = caloNav.offsetIEta(ie, -1);
        int ipP  = caloNav.offsetIEta(ip, 1);
        int ipM  = caloNav.offsetIEta(ip, -1);
        set< pair<int,int> > tts;
        tts.insert( std::make_pair(ie ,ip  ) ); 
        tts.insert( std::make_pair(ieM,ipM ) );
        tts.insert( std::make_pair(ie ,ipM ) );
        tts.insert( std::make_pair(ieP,ipM ) );
        tts.insert( std::make_pair(ieP,ip  ) );
        tts.insert( std::make_pair(ieP,ipP ) );
        tts.insert( std::make_pair(ie ,ipP ) );
        tts.insert( std::make_pair(ieM,ipP ) );
        tts.insert( std::make_pair(ieM,ip  ) );
        // loop over neighbour towers and find the corresponding RCT region
        // Then try to match with the egamma candidate
        //cout<<"  ";
        for(auto tt=tts.begin(); tt!=tts.end(); tt++)
        {
            int ierct = getRCTRegionEta(tt->first);
            int iprct = getRCTRegionPhi(tt->second);
            //cout<<"("<<ierct<<","<<iprct<<") ";
            if(ierct==iEta && iprct==iPhi )
            {
                nOverlaps++;
                break;
            }
        }
        //cout<<"\n";
    }
    return nOverlaps;
}



/*****************************************************************/
void TriggerTools::printTowers(const std::vector<l1t::CaloTower>& towers, const std::vector< std::pair<int,int> >& highlights)
/*****************************************************************/
{
    int values[56][72];
    for(unsigned iphi=0; iphi<72; iphi++)
    {
        for(unsigned ieta=0; ieta<56; ieta++)
        {
            values[ieta][iphi] = 0;
        }
    }
    for(auto itr=towers.cbegin();itr!=towers.end();itr++)
    {
        int ieta = (itr->hwEta()<0 ? itr->hwEta()+28 : itr->hwEta()+27);
        int iphi = itr->hwPhi()-1;
        if(ieta<0 || ieta>=56 || iphi<0 || iphi>=72) continue;
        values[ieta][iphi] = itr->hwEtEm();
    }
    std::cout<<"   ";
    for(unsigned ieta=0; ieta<56; ieta++)
    {
        int hwEta = (ieta<28 ? ieta-28 : ieta-27);
        std::cout<<std::setw(3)<<abs(hwEta);
    }
    std::cout<<"\n";
    for(unsigned iphi=0; iphi<72; iphi++)
    {
        int hwPhi = iphi+1;
        std::cout<<std::setw(3)<<hwPhi;
        for(unsigned ieta=0; ieta<56; ieta++)
        {
            int hwEta = (ieta<28 ? ieta-28 : ieta-27);
            int value = values[ieta][iphi];
            bool highlight = ( find(highlights.begin(),highlights.end(), make_pair(hwEta,hwPhi))!=highlights.end() );
            if(highlight) std::cout<<"\033[1;31m";
            if(value>0) std::cout<<std::setw(3)<<value;
            else std::cout<<std::setw(3)<<"-";
            if(highlight) std::cout<<"\033[0m";
        }
        std::cout<<"\n";
    }
    std::cout<<"\n\n";

}


/*****************************************************************/
void TriggerTools::printClusters(const std::vector<L1EGamma>& clusters, const std::vector< std::pair<int,int> >& highlights)
/*****************************************************************/
{
    int values[56][72];
    for(unsigned iphi=0; iphi<72; iphi++)
    {
        for(unsigned ieta=0; ieta<56; ieta++)
        {
            values[ieta][iphi] = 0;
        }
    }
    for(auto itr=clusters.cbegin();itr!=clusters.end();itr++)
    {
        int ieta = (itr->hwEta()<0 ? itr->hwEta()+28 : itr->hwEta()+27);
        int iphi = itr->hwPhi()-1;
        if(ieta<0 || ieta>=56 || iphi<0 || iphi>=72) continue;
        values[ieta][iphi] = itr->hwPt();
    }
    std::cout<<"   ";
    for(unsigned ieta=0; ieta<56; ieta++)
    {
        int hwEta = (ieta<28 ? ieta-28 : ieta-27);
        std::cout<<std::setw(3)<<abs(hwEta);
    }
    std::cout<<"\n";
    for(unsigned iphi=0; iphi<72; iphi++)
    {
        int hwPhi = iphi+1;
        std::cout<<std::setw(3)<<hwPhi;
        for(unsigned ieta=0; ieta<56; ieta++)
        {
            int hwEta = (ieta<28 ? ieta-28 : ieta-27);
            int value = values[ieta][iphi];
            bool highlight = ( find(highlights.begin(),highlights.end(), make_pair(hwEta,hwPhi))!=highlights.end() );
            if(highlight) std::cout<<"\033[1;31m";
            if(value>0) std::cout<<std::setw(3)<<value;
            else std::cout<<std::setw(3)<<"-";
            if(highlight) std::cout<<"\033[0m";
        }
        std::cout<<"\n";
    }
    std::cout<<"\n\n";

}

/*****************************************************************/
void TriggerTools::printStage1Clusters(const std::vector<L1EGamma>& clusters, const std::vector< std::pair<int,int> >& highlights)
/*****************************************************************/
{
    int values[14][18];
    for(unsigned iphi=0; iphi<18; iphi++)
    {
        for(unsigned ieta=0; ieta<14; ieta++)
        {
            values[ieta][iphi] = 0;
        }
    }
    for(auto itr=clusters.cbegin();itr!=clusters.end();itr++)
    {
        int ieta = TriggerTools::gtToRctEta(itr->hwEta())-4;
        int iphi = itr->hwPhi();
        if(ieta<0 || ieta>=14 || iphi<0 || iphi>=18) continue;
        if(values[ieta][iphi]<itr->hwPt()) values[ieta][iphi] = itr->hwPt(); // take most energetic candidate in RCT region
    }
    std::cout<<"   ";
    //for(unsigned ieta=0; ieta<14; ieta++)
    //{
    //    int hwEta = ieta+4;
    //    std::cout<<std::setw(3)<<abs(hwEta);
    //}
    for(unsigned ieta=0; ieta<56; ieta++)
    {
        int hwEta = (ieta<28 ? ieta-28 : ieta-27);
        std::cout<<std::setw(3)<<abs(hwEta);
    }
    std::cout<<"\n";
    for(unsigned iphi=0; iphi<72; iphi++)
    {
        int hwPhi = iphi+1;
        std::cout<<std::setw(3)<<hwPhi;
        for(unsigned ieta=0; ieta<56; ieta++)
        {
            int hwEta = (ieta<28 ? ieta-28 : ieta-27);
            int rctEta = TriggerTools::getRCTRegionEta(hwEta);
            int rctPhi = TriggerTools::getRCTRegionPhi(hwPhi);
            int value = values[rctEta-4][rctPhi];
            bool highlight = ( find(highlights.begin(),highlights.end(), make_pair(rctEta,rctPhi))!=highlights.end() );
            if(highlight) std::cout<<"\033[1;31m";
            if(value>0) std::cout<<std::setw(3)<<value;
            else std::cout<<std::setw(3)<<"-";
            if(highlight) std::cout<<"\033[0m";
        }
        std::cout<<"\n";
    }
    //for(unsigned iphi=0; iphi<18; iphi++)
    //{
    //    //int hwPhi = iphi;
    //    int hwPhi = iphi*4+1;
    //    for(int ip=0;ip<4;ip++)
    //    {
    //        std::cout<<std::setw(3)<<hwPhi+ip;
    //        for(unsigned ieta=0; ieta<14; ieta++)
    //        {
    //            int hwEta = ieta+4;
    //            int value = values[ieta][iphi];
    //            bool highlight = ( find(highlights.begin(),highlights.end(), make_pair(hwEta,hwPhi))!=highlights.end() );
    //            for(int ie=0;ie<4;ie++)
    //            {
    //                if(highlight) std::cout<<"\033[1;31m";
    //                if(value>0) std::cout<<std::setw(3)<<value;
    //                else std::cout<<std::setw(3)<<"-";
    //                if(highlight) std::cout<<"\033[0m";
    //            }
    //        }
    //        std::cout<<"\n";
    //    }
    //    //std::cout<<"\n\n\n";
    //}
    std::cout<<"\n\n";

}


/*****************************************************************/
int TriggerTools::getRCTRegionPhi(int ttphi)
/*****************************************************************/
{
  int rctphi=0;
  rctphi = (ttphi+1)/4;
  if(ttphi<=2) rctphi=0;
  if(ttphi>=71) rctphi=0;
  
  return rctphi;
}

/*****************************************************************/
int TriggerTools::getRCTRegionEta(int tteta)
/*****************************************************************/
{
  int rcteta = 0;
  
  if(tteta>0) rcteta = (tteta-1)/4 + 11;
  else if(tteta<0) rcteta = (tteta+1)/4 + 10;
  
  return rcteta;
}



/*****************************************************************/
unsigned int TriggerTools::rctToGtEta(unsigned int iEta)
/*****************************************************************/
{
    unsigned rctEta = (iEta<11 ? 10-iEta : iEta-11);
    return (((rctEta % 7) & 0x7) | (iEta<11 ? 0x8 : 0));
}

/*****************************************************************/
int TriggerTools::gtToRctEta(int iEta)
/*****************************************************************/
{
    return (iEta<7 ? iEta+11 : 18-iEta);
}




/*****************************************************************/
unsigned int TriggerTools::convertClusterShapeTo8Bits(const l1t::CaloCluster& cluster)
/*****************************************************************/
{
    unsigned int shape = 0;
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_N)) shape |= 0x1;
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_S)) shape |= 0x2;
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_NN)) shape |= 0x20;
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_SS)) shape |= 0x40;
    if(cluster.checkClusterFlag(l1t::CaloCluster::TRIM_LEFT))
    {
        if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_E))  shape |= 0x4;
        if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_NE)) shape |= 0x8;
        if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_SE)) shape |= 0x10;
    }
    else
    {
        if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_W))  shape |= 0x4;
        if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_NW)) shape |= 0x8;
        if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_SW)) shape |= 0x10;
    }
    return shape;
}



/*****************************************************************/
std::map<unsigned int, float> TriggerTools::towerEnergyFractions(const l1t::CaloCluster& cluster, const std::vector<l1t::CaloTower>& towers)
/*****************************************************************/
{
    l1t::CaloStage2Nav caloNav;
    int iEta = cluster.hwEta();
    int iPhi = cluster.hwPhi();
    int iPhiP = caloNav.offsetIPhi(iPhi, 1);
    int iPhiM = caloNav.offsetIPhi(iPhi, -1);
    int iPhiP2 = caloNav.offsetIPhi(iPhi, 2);
    int iPhiM2 = caloNav.offsetIPhi(iPhi, -2);
    int iEtaP = caloNav.offsetIEta(iEta, 1); 
    if(!cluster.checkClusterFlag(l1t::CaloCluster::TRIM_LEFT))
    {
        iEtaP = caloNav.offsetIEta(iEta, -1); 
    }
    const l1t::CaloTower& towerN  = l1t::CaloTools::getTower(towers, iEta , iPhiM);
    const l1t::CaloTower& towerS  = l1t::CaloTools::getTower(towers, iEta , iPhiP);
    const l1t::CaloTower& towerP  = l1t::CaloTools::getTower(towers, iEtaP, iPhi );
    const l1t::CaloTower& towerNP = l1t::CaloTools::getTower(towers, iEtaP, iPhiM);
    const l1t::CaloTower& towerSP = l1t::CaloTools::getTower(towers, iEtaP, iPhiP);
    const l1t::CaloTower& towerNN = l1t::CaloTools::getTower(towers, iEta , iPhiM2);
    const l1t::CaloTower& towerSS = l1t::CaloTools::getTower(towers, iEta , iPhiP2);
    float fractionN  =  (double)towerN .hwEtEm()/(double)cluster.hwPt();
    float fractionS  =  (double)towerS .hwEtEm()/(double)cluster.hwPt();
    float fractionP  =  (double)towerP .hwEtEm()/(double)cluster.hwPt();
    float fractionNP =  (double)towerNP.hwEtEm()/(double)cluster.hwPt();
    float fractionSP =  (double)towerSP.hwEtEm()/(double)cluster.hwPt();
    float fractionNN =  (double)towerNN.hwEtEm()/(double)cluster.hwPt();
    float fractionSS =  (double)towerSS.hwEtEm()/(double)cluster.hwPt();

    //int totalEnergy = 0;
    //const l1t::CaloTower& towerSeed  = l1t::CaloTools::getTower(towers, iEta , iPhi);
    //totalEnergy += towerSeed.hwEtEm();
    //if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_N))  totalEnergy += towerN .hwEtEm();
    //if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_S))  totalEnergy += towerS .hwEtEm();
    //if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_NN)) totalEnergy += towerNN.hwEtEm();
    //if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_SS)) totalEnergy += towerSS.hwEtEm();
    //if(cluster.checkClusterFlag(l1t::CaloCluster::TRIM_LEFT))
    //{
    //    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_E))  totalEnergy += towerP .hwEtEm();
    //    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_NE)) totalEnergy += towerNP.hwEtEm();
    //    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_SE)) totalEnergy += towerSP.hwEtEm();
    //}
    //else
    //{
    //    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_W))  totalEnergy += towerP .hwEtEm();
    //    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_NW)) totalEnergy += towerNP.hwEtEm();
    //    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_SW)) totalEnergy += towerSP.hwEtEm();
    //}
    //if(totalEnergy!=cluster.hwPt()) cout<<"Cluster energy = "<<cluster.hwPt()<<", Towers energy = "<<totalEnergy<<"\n";


    map<unsigned int, float> energyFractions;
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_N)) energyFractions[0] = fractionN;
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_S)) energyFractions[1] = fractionS;
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_NN)) energyFractions[5] = fractionNN;
    if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_SS)) energyFractions[6] = fractionSS;
    if(cluster.checkClusterFlag(l1t::CaloCluster::TRIM_LEFT))
    {
        if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_E)) energyFractions[2] = fractionP;
        if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_NE)) energyFractions[3] = fractionNP;
        if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_SE)) energyFractions[4] = fractionSP;
    }
    else
    {
        if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_W)) energyFractions[2] = fractionP;
        if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_NW)) energyFractions[3] = fractionNP;
        if(cluster.checkClusterFlag(l1t::CaloCluster::INCLUDE_SW)) energyFractions[4] = fractionSP;
    }
    return energyFractions;
}

