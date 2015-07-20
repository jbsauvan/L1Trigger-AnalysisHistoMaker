/**
 *  @file  Analysis.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/30/2014
 *
 *  @internal
 *     Created :  07/30/2014
 * Last update :  07/30/2014 09:31:45 AM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */

#include <iostream>
#include <fstream>

#include "TSystem.h"
#include "TVector2.h"

#include "TDecayedParticle.h"
#include "Analysis.h"
#include "TriggerTools.h"

#include "L1Trigger/L1TCalorimeter/interface/CaloTools.h"


using namespace AnHiMa;
using namespace std;



/*****************************************************************/
Analysis::Analysis():IAnalysis()
/*****************************************************************/
{

}



/*****************************************************************/
Analysis::~Analysis()
/*****************************************************************/
{

}



/*****************************************************************/
bool Analysis::initialize(const string& parameterFile)
/*****************************************************************/
{
    gSystem->Load("../TriggerCommon/lib/libDictionary_C.so");

    bool status = IAnalysis::initialize(parameterFile);
    if(!status) return status;

    const string& emulatorParams = m_reader.params().GetValue("EmulatorParameters", "/home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/data/caloStage2Params.txt");
    cout<<"INFO: EmulatorParameters: "<<emulatorParams<<"\n";
    event().setEmulatorParameters(emulatorParams);

    event().connectVariables(m_inputChain);
    
    return true;
}


/*****************************************************************/
void Analysis::execute()
/*****************************************************************/
{
    //cout<<">> EVENT "<<event().event()<<"\n";
    event().update();

    if(!event().passSelection()) return;
    for(int hTag=0; hTag<7; hTag++)
    {
        fillHistos(hTag);
    }


}

/*****************************************************************/
void Analysis::fillHistos(int hTag)
/*****************************************************************/
{
    int col = 0;
    if(hTag==6) col = 2;

    short sysNum = 0;
    float weight = 1.;
    int hoffset  = 10000*hTag;


    m_histos.FillHisto(0+hoffset, 0.5, weight, sysNum);
    m_histos.FillHisto(1+hoffset, event().npv(), weight, sysNum);

    for(auto zItr=event().ZCandidates().cbegin(); zItr!=event().ZCandidates().end(); zItr++)
    {
        const TDecayedParticle<RecoL1Electron>& z = *zItr;
        const RecoL1Electron& tag   = z[0];
        const RecoL1Electron& probe = z[1];

        m_histos.FillHisto(10+hoffset, tag.Pt(), weight, sysNum);
        m_histos.FillHisto(11+hoffset, tag.Eta(), weight, sysNum);
        m_histos.FillHisto(12+hoffset, tag.Phi(), weight, sysNum);
        m_histos.FillHisto(13+hoffset, tag.charge(), weight, sysNum);
        m_histos.FillHisto(14+hoffset, tag.bdt(), weight, sysNum);

        m_histos.FillHisto(20+hoffset, probe.Pt(), weight, sysNum);
        m_histos.FillHisto(21+hoffset, probe.Eta(), weight, sysNum);
        m_histos.FillHisto(22+hoffset, probe.Phi(), weight, sysNum);
        m_histos.FillHisto(23+hoffset, probe.charge(), weight, sysNum);
        m_histos.FillHisto(24+hoffset, probe.bdt(), weight, sysNum);

        m_histos.FillHisto(30+hoffset, z.M(), weight, sysNum);
        m_histos.FillHisto(31+hoffset, z.Pt(), weight, sysNum);
        m_histos.FillHisto(32+hoffset, z.Rapidity(), weight, sysNum);
        m_histos.FillHisto(33+hoffset, z.Phi(), weight, sysNum);

        // check if electron is matched to a L1 cluster
        if(probe.ref(col).hwPt()>0)
        {
            const L1EGamma& l1eg = probe.ref(col);
            //double l1egPt = l1eg.pt();

            if( col==2 || ( col==0 && (l1eg.hwQual()&0x1) && (l1eg.hwQual()&0x2) && (l1eg.hwQual()&0x4) ) )
            {

                m_histos.FillHisto(100+hoffset, probe.Pt(), weight, sysNum);
                m_histos.FillHisto(101+hoffset, probe.Eta(), weight, sysNum);
                m_histos.FillHisto(102+hoffset, probe.Phi(), weight, sysNum);
                m_histos.FillHisto(104+hoffset, probe.bdt(), weight, sysNum);

                double deta = 999.;
                double dphi = 999.;
                double detaCalo = 999.;
                double dphiCalo = 999.;
                double clusterEta = 0.;
                double clusterPhi = 0.;
                double refL1Eta = 0.;
                double refL1Phi = 0.;

                if(col==0) 
                {
                    const l1t::CaloCluster& cluster = l1eg.ref();
                    if(cluster.hwEta()==0 && cluster.hwPhi()==0) continue;
                    double seedEta     = l1t::CaloTools::towerEta(cluster.hwEta());
                    double seedEtaSize = l1t::CaloTools::towerEtaSize(cluster.hwEta());
                    double seedPhi     = l1t::CaloTools::towerPhi(cluster.hwEta(), cluster.hwPhi());
                    double seedPhiSize = l1t::CaloTools::towerPhiSize(cluster.hwEta());

                    if(cluster.fgEta()==0)      refL1Eta = seedEta; // center
                    else if(cluster.fgEta()==2) refL1Eta = seedEta + seedEtaSize/3.; // center + 1/3
                    else if(cluster.fgEta()==1) refL1Eta = seedEta - seedEtaSize/3.; // center - 1/3
                    if(cluster.fgPhi()==0)      refL1Phi = seedPhi; // center
                    else if(cluster.fgPhi()==2) refL1Phi = seedPhi + seedPhiSize/3.; // center + 1/3
                    else if(cluster.fgPhi()==1) refL1Phi = seedPhi - seedPhiSize/3.; // center - 1/3


                    if(hTag==0) // take position of the emulator
                    {
                        clusterEta = l1eg.eta();
                        clusterPhi = l1eg.phi();
                    }
                    else if(hTag==1) // granularity of 1/3 of trigger tower
                    {
                        clusterEta = refL1Eta;
                        clusterPhi = refL1Phi;
                    }
                    else if(hTag==2) // granularity of 1/2 of trigger tower
                    {
                        if(cluster.fgEta()==0)      clusterEta = seedEta; // center
                        else if(cluster.fgEta()==2) clusterEta = seedEta + seedEtaSize*0.5; // center + 1/2
                        else if(cluster.fgEta()==1) clusterEta = seedEta - seedEtaSize*0.5; // center - 1/2
                        if(cluster.fgPhi()==0)      clusterPhi = seedPhi; // center
                        else if(cluster.fgPhi()==2) clusterPhi = seedPhi + seedPhiSize*0.5; // center + 1/2
                        else if(cluster.fgPhi()==1) clusterPhi = seedPhi - seedPhiSize*0.5; // center - 1/2
                    }
                    else if(hTag==3) // granularity of 0.087/8
                    {
                        double step = l1t::CaloTools::towerEtaSize(1)/8.;
                        double closestEta = 0.;
                        double diff = fabs(closestEta-fabs(l1eg.eta()));
                        while(closestEta<5.)
                        {
                            double eta = closestEta + step;
                            if(fabs(eta-fabs(l1eg.eta()))>diff)
                            {
                                break;
                            }
                            diff = fabs(eta-fabs(l1eg.eta()));
                            closestEta = eta;
                        }
                        clusterEta = closestEta*((l1eg.eta() < 0.) ? -1. : (float)(l1eg.eta() > 0.));
                        clusterPhi = l1eg.phi();

                    }
                    else if(hTag==4) // granularity of 0.087/4
                    {
                        double step = l1t::CaloTools::towerEtaSize(1)/4.;
                        double closestEta = 0.;
                        double diff = fabs(closestEta-fabs(l1eg.eta()));
                        while(closestEta<5.)
                        {
                            double eta = closestEta + step;
                            if(fabs(eta-fabs(l1eg.eta()))>diff)
                            {
                                break;
                            }
                            diff = fabs(eta-fabs(l1eg.eta()));
                            closestEta = eta;
                        }
                        clusterEta = closestEta*((l1eg.eta() < 0.) ? -1. : (float)(l1eg.eta() > 0.));
                        clusterPhi = l1eg.phi();
                    }
                    else if(hTag==5) // granularity of 0.087/2
                    {
                        double l1egEta = 0.;
                        double l1egPhi = 0.;
                        if(cluster.fgEta()==0)      l1egEta = seedEta; // center
                        else if(cluster.fgEta()==2) l1egEta = seedEta + seedEtaSize/3.; // center + 1/3
                        else if(cluster.fgEta()==1) l1egEta = seedEta - seedEtaSize/3.; // center - 1/3
                        if(cluster.fgPhi()==0)      l1egPhi = seedPhi; // center
                        else if(cluster.fgPhi()==2) l1egPhi = seedPhi + seedPhiSize/3.; // center + 1/3
                        else if(cluster.fgPhi()==1) l1egPhi = seedPhi - seedPhiSize/3.; // center - 1/3

                        double step = l1t::CaloTools::towerEtaSize(1)/2.;
                        double closestEta = 0.;
                        double diff = fabs(closestEta-fabs(l1egEta));
                        while(closestEta<5.)
                        {
                            double eta = closestEta + step;
                            if(fabs(eta-fabs(l1egEta))>diff)
                            {
                                break;
                            }
                            diff = fabs(eta-fabs(l1egEta));
                            closestEta = eta;
                        }
                        clusterEta = closestEta*((l1egEta < 0.) ? -1. : (float)(l1egEta > 0.));
                        clusterPhi = l1egPhi;
                    }
                    deta = clusterEta - probe.Eta();
                    dphi = clusterPhi - probe.Phi();
                    detaCalo = clusterEta - probe.superClusterEta();
                    dphiCalo = clusterPhi - probe.superClusterPhi();
                }

                
                m_histos.Fill1BinHisto(200+hoffset, fabs(probe.Eta()), deta, weight, sysNum);
                m_histos.Fill1BinHisto(250+hoffset, fabs(probe.Eta()), dphi, weight, sysNum);
                m_histos.Fill1BinHisto(300+hoffset, fabs(probe.superClusterEta()), detaCalo, weight, sysNum);
                m_histos.Fill1BinHisto(350+hoffset, fabs(probe.superClusterEta()), dphiCalo, weight, sysNum);
                m_histos.FillHisto(400+hoffset, fabs(probe.superClusterEta()), fabs(clusterEta), weight, sysNum);
                m_histos.FillHisto(401+hoffset, fabs(refL1Eta), fabs(clusterEta), weight, sysNum);

            }

        }
    }

}
