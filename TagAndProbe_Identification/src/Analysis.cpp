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
    const string& shapeLUTFile = m_reader.params().GetValue("ShapeLUT", "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Resolution/data/sortedShapes.txt");
    cout<<"INFO: Sorted shapes LUT: "<<shapeLUTFile<<"\n";
    readShapeLUT(shapeLUTFile);

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
    fillHistos(0);


}

/*****************************************************************/
void Analysis::fillHistos(int hTag)
/*****************************************************************/
{
    short sysNum = 0;
    float weight = 1.;
    int hoffset  = 0;

    m_histos.FillHisto(0+hoffset, 0.5, weight, sysNum);
    m_histos.FillHisto(1+hoffset, event().npv(), weight, sysNum);

    for(auto eItr=event().genElectrons().cbegin(); eItr!=event().genElectrons().end(); eItr++)
    {
        const GenElectron& genEle = *eItr;
        const RecoL1Electron& recoEle = genEle.ref();
        double pRatio = (recoEle.E()-genEle.E())/genEle.E();
        m_histos.Fill1BinHisto(700+hoffset, fabs(recoEle.superClusterEta()), pRatio, weight, sysNum);
    }

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
        if(probe.ref().hwPt()>0)
        {
            const L1EGamma& l1eg = probe.ref();
            m_histos.FillHisto(100+hoffset, probe.Pt(), weight, sysNum);
            m_histos.FillHisto(101+hoffset, probe.Eta(), weight, sysNum);
            m_histos.FillHisto(102+hoffset, probe.Phi(), weight, sysNum);
            m_histos.FillHisto(104+hoffset, probe.bdt(), weight, sysNum);

            double ptRatio = (l1eg.pt()-probe.Pt())/probe.Pt();
            double etRatio = (l1eg.pt()-probe.caloEt())/probe.caloEt();
            
            m_histos.Fill1BinHisto(1000+hoffset, abs(l1eg.hwEta()), ptRatio, weight, sysNum);
            m_histos.Fill2BinHisto(1030+hoffset, abs(l1eg.hwEta()), probe.Pt(), ptRatio, weight, sysNum);
            m_histos.Fill1BinHisto(1050+hoffset, abs(l1eg.hwEta()), etRatio, weight, sysNum);
            m_histos.Fill2BinHisto(1080+hoffset, abs(l1eg.hwEta()), probe.Pt(), etRatio, weight, sysNum);

            // for regression tree
            unsigned int clusterShape = TriggerTools::convertClusterShapeTo8Bits(l1eg.ref());
            unsigned int sortedShape = clusterShape;
            if(m_shapeLUT.find(clusterShape)!=m_shapeLUT.end())
            {
                sortedShape = m_shapeLUT[clusterShape];
            }
            // get seed tower
            const l1t::CaloTower& seedTower = l1t::CaloTools::getTower(event().towers(), l1eg.hwEta(), l1eg.hwPhi());
            int seedE = seedTower.hwEtEm();
            int seedH = seedTower.hwEtHad();
            double seedHoE = (double)seedH/(double)seedE;
            int seedFG = ( (seedTower.hwQual() & (0x1<<2)) ? 1 : 0);

            m_histos.FillHisto(2000+hoffset, seedE, weight, sysNum);
            m_histos.FillHisto(2001+hoffset, seedH, weight, sysNum);
            m_histos.FillHisto(2002+hoffset, l1eg.hwPt(), weight, sysNum);
            m_histos.FillHisto(2003+hoffset, l1eg.hwEta(), weight, sysNum);
            m_histos.FillHisto(2004+hoffset, seedHoE, weight, sysNum);
            m_histos.FillHisto(2005+hoffset, seedFG, weight, sysNum);
            //m_histos.FillHisto(2006+hoffset, sortedShape, weight, sysNum);
            m_histos.FillHisto(2006+hoffset, clusterShape, weight, sysNum);
            m_histos.FillHisto(2007+hoffset, probe.Pt(), weight, sysNum);

            m_histos.FillNtuple(3000+hoffset, (double)event().run(), (double)event().event(), weight, sysNum);

        }
    }

}



/*****************************************************************/
void Analysis::readShapeLUT(const string& file)
/*****************************************************************/
{
    ifstream stream(file);
    if(!stream.is_open())
    {
        cout<<"ERROR: Cannot open file "<<file<<"\n";
        return;
    }
    while(!stream.eof())
    {
        unsigned int a=0, b=0;
        stream >> a >> b;
        m_shapeLUT[a] = b;
    }
    stream.close();
}
