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
Analysis::Analysis():IAnalysis(),
    m_random(123456),
    m_applyCorr95(false)
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


    // identification parameters
    const string& cutsHoEFile = m_reader.params().GetValue("CutsHoEFile", "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Identification/data/cutsHoE.root");
    const string& cutsHoEHisto = m_reader.params().GetValue("CutsHoEHisto", "cutsEff05_eta_ET");
    const string& cutsShapeFile = m_reader.params().GetValue("CutsShapeFile", "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Identification/data/cutsShape_NoETdependency.root");
    const string& cutsShapeHisto = m_reader.params().GetValue("CutsShapeHisto", "cutsEff05_eta");
    cout<<"INFO: File H/E cuts: "<<cutsHoEFile<<"\n";
    cout<<"INFO: Histo H/E cuts: "<<cutsHoEHisto<<"\n";
    cout<<"INFO: File shape cuts: "<<cutsShapeFile<<"\n";
    cout<<"INFO: Histo shape cuts: "<<cutsShapeHisto<<"\n";

    TFile* file = TFile::Open(cutsHoEFile.c_str());
    if(!file) return false;
    m_cutsHoE = (TH2F*)file->Get(cutsHoEHisto.c_str());
    if(!m_cutsHoE) return false;
    m_cutsHoE->SetDirectory(0);
    file->Close();

    file = TFile::Open(cutsShapeFile.c_str());
    if(!file) return false;
    m_cutsShape = (TH2F*)file->Get(cutsShapeHisto.c_str());
    if(!m_cutsShape) return false;
    m_cutsShape->SetDirectory(0);
    file->Close();

    // Calibration wrt 95% efficiency
    m_applyCorr95 = m_reader.params().GetValue("ApplyCorr95", false);
    const string& corr95EtaStage2File = m_reader.params().GetValue("Corr95EtaStage2File", "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Resolution/data/corr_vs_eta_Stage2_95pcEff.txt");
    const string& corr95EtaStage1File = m_reader.params().GetValue("Corr95EtaStage1File", "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Resolution/data/corr_vs_eta_Stage1_95pcEff.txt");
    cout<<"INFO: Apply 95\% eff calibration: "<<m_applyCorr95<<"\n";
    cout<<"INFO: Stage-1 95\% calibration corrections: "<<corr95EtaStage1File<<"\n";
    cout<<"INFO: Stage-2 95\% calibration corrections: "<<corr95EtaStage2File<<"\n";

    ifstream streamStage1(corr95EtaStage1File, ifstream::in);
    if(!streamStage1.is_open()) cout<<"ERROR: Cannot open correction file "<<corr95EtaStage1File<<"\n";
    int ieta=0;
    double corr=1.; 
    while(!streamStage1.eof()) 
    {
        streamStage1>>ieta>>corr;
        m_corrVsEta95pcStage1[ieta] = corr;
    }
    streamStage1.close();

    ifstream streamStage2(corr95EtaStage2File, ifstream::in);
    if(!streamStage2.is_open()) cout<<"ERROR: Cannot open correction file "<<corr95EtaStage2File<<"\n";
    ieta=0;corr=1.;
    while(!streamStage2.eof()) 
    {
        streamStage2>>ieta>>corr;
        m_corrVsEta95pcStage2[ieta] = corr;
    }
    streamStage2.close();


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
    fillHistos(1);


}

/*****************************************************************/
void Analysis::fillHistos(int hTag)
/*****************************************************************/
{
    int col = 0;
    if(hTag==1) col = 2;

    short sysNum = 0;
    float weight = 1.;
    int hoffset  = 10000*hTag;


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
        if(probe.ref(col).hwPt()>0)
        {
            const L1EGamma& l1eg = probe.ref(col);
            double l1egPt = l1eg.pt();
            if(m_applyCorr95)
            {
                double corr = 1.;
                if(col==0 || col==1)
                {
                    if(m_corrVsEta95pcStage2.find(abs(l1eg.hwEta()))!=m_corrVsEta95pcStage2.end()) 
                    {
                        corr = m_corrVsEta95pcStage2[ abs(l1eg.hwEta()) ];
                    }
                }
                else if(col==2)
                {
                    if(m_corrVsEta95pcStage1.find(abs(l1eg.hwEta()))!=m_corrVsEta95pcStage1.end()) 
                    {
                        corr = m_corrVsEta95pcStage1[ abs(l1eg.hwEta()) ];
                    }
                }
                l1egPt *= corr;
            }


            //if( col==2 || ( col==0 && (l1eg.hwQual()&0x1) && passHoE(l1eg) && passShape(l1eg) ) ) // check FG,  H/E new and shape new
            if( col==2 || ( col==0 && (l1eg.hwQual()&0x1) && (l1eg.hwQual()&0x2) && (l1eg.hwQual()&0x4) ) )
            {

                m_histos.FillHisto(100+hoffset, probe.Pt(), weight, sysNum);
                m_histos.FillHisto(101+hoffset, probe.Eta(), weight, sysNum);
                m_histos.FillHisto(102+hoffset, probe.Phi(), weight, sysNum);
                m_histos.FillHisto(104+hoffset, probe.bdt(), weight, sysNum);

                double ptRatio = (l1egPt-probe.Pt())/probe.Pt();
                double etRatio = (l1egPt-probe.caloEt())/probe.caloEt();

                bool saturate = false;
                if( (col==0 || col==1) && l1eg.hwPt()>=255 ) saturate = true;
                if( col==2 && l1eg.hwPt()>=63 ) saturate = true;

                
                if(!saturate)
                {
                    m_histos.Fill1BinHisto(1000+hoffset, abs(l1eg.hwEta()), ptRatio, weight, sysNum);
                    m_histos.Fill1BinHisto(1030+hoffset, fabs(probe.Eta()), ptRatio, weight, sysNum);
                    m_histos.Fill2BinHisto(1060+hoffset, abs(l1eg.hwEta()), probe.Pt(), ptRatio, weight, sysNum);
                    m_histos.Fill1BinHisto(1080+hoffset, abs(l1eg.hwEta()), etRatio, weight, sysNum);
                    m_histos.Fill2BinHisto(1110+hoffset, abs(l1eg.hwEta()), probe.Pt(), etRatio, weight, sysNum);
                }

                // for regression tree
                unsigned int group = m_random.Integer(2);
                unsigned int clusterShape = TriggerTools::convertClusterShapeTo8Bits(l1eg.ref());
                unsigned int sortedShape = clusterShape;
                if(m_shapeLUT.find(clusterShape)!=m_shapeLUT.end())
                {
                    sortedShape = m_shapeLUT[clusterShape];
                }
                double target = probe.Pt()/l1egPt;
                m_histos.FillHisto(5000+hoffset, group, weight, sysNum);
                m_histos.FillHisto(5001+hoffset, l1eg.hwEta(), weight, sysNum);
                m_histos.FillHisto(5002+hoffset, l1eg.hwPhi(), weight, sysNum);
                m_histos.FillHisto(5003+hoffset, l1eg.hwPt(), weight, sysNum);
                m_histos.FillHisto(5004+hoffset, sortedShape, weight, sysNum);
                m_histos.FillHisto(5005+hoffset, l1eg.ref().checkClusterFlag(l1t::CaloCluster::TRIM_LEFT), weight, sysNum);
                m_histos.FillHisto(5006+hoffset, target, weight, sysNum);
                m_histos.FillHisto(5007+hoffset, probe.Pt(), weight, sysNum);
                m_histos.FillHisto(5008+hoffset, l1egPt, weight, sysNum);

                m_histos.FillNtuple(5100+hoffset, (double)event().run(), (double)event().event(), weight, sysNum);
            }

        }
    }

}

/*****************************************************************/
bool Analysis::passHoE(const L1EGamma& l1eg)
/*****************************************************************/
{
    int ieta = abs(l1eg.hwEta());
    int et = l1eg.hwPt();
    const l1t::CaloTower& seedTower = l1t::CaloTools::getTower(event().towers(), l1eg.hwEta(), l1eg.hwPhi());
    int seedE = seedTower.hwEtEm();
    int seedH = seedTower.hwEtHad();
    double seedHoE = (double)seedH/(double)seedE;


    int beta = m_cutsHoE->GetXaxis()->FindBin(ieta);
    int bet  = m_cutsHoE->GetYaxis()->FindBin(et);
    //cout<<"ieta,et="<<ieta<<", "<<et<<"\n";
    //cout<<" bin before = "<<beta<<", "<<bet<<"\n";
    if(beta<1) beta = 1;
    if(beta>m_cutsHoE->GetNbinsX()) beta = m_cutsHoE->GetNbinsX();
    if(bet<1) bet = 1;
    if(bet>m_cutsHoE->GetNbinsY()) bet = m_cutsHoE->GetNbinsY();
    //cout<<" bin after = "<<beta<<", "<<bet<<"\n";
    double cut = m_cutsHoE->GetBinContent(beta, bet);

    return (seedHoE<=cut);
}


/*****************************************************************/
bool Analysis::passShape(const L1EGamma& l1eg)
/*****************************************************************/
{
    int ieta = abs(l1eg.hwEta());
    if(ieta>=27) ieta=28;
    int et = l1eg.hwPt();
    unsigned int clusterShape = TriggerTools::convertClusterShapeTo8Bits(l1eg.ref());
    unsigned int sortedShape = clusterShape;
    if(m_shapeLUT.find(clusterShape)!=m_shapeLUT.end())
    {
        sortedShape = m_shapeLUT[clusterShape];
    }


    int beta = m_cutsShape->GetXaxis()->FindBin(ieta);
    int bet  = m_cutsShape->GetYaxis()->FindBin(et);
    int bshape = m_cutsShape->GetZaxis()->FindBin(sortedShape);
    if(beta<1) beta = 1;
    if(beta>m_cutsShape->GetNbinsX()) beta = m_cutsShape->GetNbinsX();
    if(bet<1) bet = 1;
    if(bet>m_cutsShape->GetNbinsY()) bet = m_cutsShape->GetNbinsY();
    if(bshape<1) bshape = 1;
    if(bshape>m_cutsShape->GetNbinsZ()) bshape = m_cutsShape->GetNbinsZ();
    int passVeto = m_cutsShape->GetBinContent(beta, bet, bshape);
    //cout<<ieta<<", "<<sortedShape<<", "<<passVeto<<"\n";

    return (passVeto==1);
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
