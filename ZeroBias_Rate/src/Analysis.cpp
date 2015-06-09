/**
 *  @file  Analysis.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    15/09/2014
 *
 *  @internal
 *     Created :  15/09/2014
 * Last update :  15/09/2014 17:44:46
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */

#include <iostream>
#include <fstream>

#include "TSystem.h"
#include "TVector2.h"

#include "Analysis.h"
#include "TriggerTools.h"

#include "L1Trigger/L1TCalorimeter/interface/CaloTools.h"


using namespace AnHiMa;
using namespace std;



/*****************************************************************/
Analysis::Analysis():IAnalysis(),
    m_applyCorr95(false)
/*****************************************************************/
{

}



/*****************************************************************/
Analysis::~Analysis()
/*****************************************************************/
{
    m_cutsHoE->Delete();
    m_discriminantShape->Delete();

}



/*****************************************************************/
bool Analysis::initialize(const string& parameterFile)
/*****************************************************************/
{
    gSystem->Load("../TriggerCommon/lib/libDictionary_C.so");

    bool status = IAnalysis::initialize(parameterFile);
    if(!status) return status;

    const string& emulatorParams = m_reader.params().GetValue("EmulatorParameters", "/home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/data/caloStage2Params.txt");
    cout<<"INFO: EmulatorParameters: '"<<emulatorParams<<"'\n";
    const string& shapeLUTFile = m_reader.params().GetValue("ShapeLUT", "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Resolution/data/sortedShapes.txt");
    cout<<"INFO: Sorted shapes LUT: '"<<shapeLUTFile<<"'\n";
    readShapeLUT(shapeLUTFile);
    event().setEmulatorParameters(emulatorParams);


    // identification parameters
    const string& cutsHoEFile = m_reader.params().GetValue("CutsHoEFile", "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Identification/data/cutsHoE.root");
    const string& cutsHoEHisto = m_reader.params().GetValue("CutsHoEHisto", "cutsEff05_eta_ET");
    const string& discriminantShapeFile = m_reader.params().GetValue("ShapeDiscriminantFile", "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Identification/data/mergedDiscriminant_eta_et_shape.root");
    const string& discriminantShapeHisto = m_reader.params().GetValue("ShapeDiscriminantHisto", "shapeDiscriminant");
    m_shapeWorkingPoint = m_reader.params().GetValue("ShapeWorkingPoint", 0.995);
    cout<<"INFO: File H/E cuts: '"<<cutsHoEFile<<"'\n";
    cout<<"INFO: Histo H/E cuts: '"<<cutsHoEHisto<<"'\n";
    cout<<"INFO: File shape discriminant: "<<discriminantShapeFile<<"\n";
    cout<<"INFO: Histo shape discriminant: "<<discriminantShapeHisto<<"\n";
    cout<<"INFO: Histo shape eff. WP: "<<m_shapeWorkingPoint<<"\n";

    TFile* file = TFile::Open(cutsHoEFile.c_str());
    if(!file) return false;
    m_cutsHoE = (TH2F*)file->Get(cutsHoEHisto.c_str());
    if(!m_cutsHoE) return false;
    m_cutsHoE->SetDirectory(0);
    file->Close();


    file = TFile::Open(discriminantShapeFile.c_str());
    if(!file) return false;
    m_discriminantShape = (TH3F*)file->Get(discriminantShapeHisto.c_str());
    if(!m_discriminantShape) return false;
    m_discriminantShape->SetDirectory(0);
    file->Close();


    // Calibration wrt 95% efficiency
    m_applyCorr95 = m_reader.params().GetValue("ApplyCorr95", false);
    const string& corr95EtaStage2File = m_reader.params().GetValue("Corr95EtaStage2File", "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Resolution/data/corr_vs_eta_Stage2_95pcEff.txt");
    const string& corr95EtaStage1File = m_reader.params().GetValue("Corr95EtaStage1File", "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Resolution/data/corr_vs_eta_Stage1_95pcEff.txt");
    cout<<"INFO: Apply 95\% eff calibration: "<<m_applyCorr95<<"\n";
    cout<<"INFO: Stage-1 95\% calibration corrections: '"<<corr95EtaStage1File<<"'\n";
    cout<<"INFO: Stage-2 95\% calibration corrections: '"<<corr95EtaStage2File<<"'\n";


    ifstream streamStage1(corr95EtaStage1File, ifstream::in);
    if(!streamStage1.is_open()) cout<<"ERROR: Cannot open correction file '"<<corr95EtaStage1File<<"'\n";
    int ieta=0;
    double corr=1.; 
    while(!streamStage1.eof()) 
    {
        streamStage1>>ieta>>corr;
        m_corrVsEta95pcStage1[ieta] = corr;
    }
    streamStage1.close();

    ifstream streamStage2(corr95EtaStage2File, ifstream::in);
    if(!streamStage2.is_open()) cout<<"ERROR: Cannot open correction file '"<<corr95EtaStage2File<<"'\n";
    ieta=0;corr=1.;
    while(!streamStage2.eof()) 
    {
        streamStage2>>ieta>>corr;
        m_corrVsEta95pcStage2[ieta] = corr;
    }
    streamStage2.close();


    event().connectVariables(m_inputChain);

    event().setShapeLUT(m_shapeLUT);
    event().setCutsHoE(m_cutsHoE);
    event().setShapeDiscriminant(m_discriminantShape);
    event().setShapeWorkingPoint(m_shapeWorkingPoint);
    event().setApplyCorr95(m_applyCorr95);
    event().setCorrVsEta95pcStage1(m_corrVsEta95pcStage1);
    event().setCorrVsEta95pcStage2(m_corrVsEta95pcStage2);
    
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

    // stage-1
    const L1EGamma* l1egMax_Stage1_all    = (event().egammasStage1Sorted().size()>0  ?  event().egammasStage1Sorted().back() : NULL);
    const L1EGamma* l1egMax_Stage1_Iso    = (event().egammasStage1_Iso().size()>0    ?  event().egammasStage1_Iso().back()   : NULL);
    // stage-2
    const L1EGamma* l1egMax_all           = (event().egammasSorted().size()>0  ?  event().egammasSorted().back()   : NULL);
    const L1EGamma* l1egMax_FG            = (event().egammas_FG().size()>0     ?  event().egammas_FG().back()      : NULL);
    const L1EGamma* l1egMax_FG_HoE        = (event().egammas_FG_HoE().size()>0 ?  event().egammas_FG_HoE().back()  : NULL);
    const L1EGamma* l1egMax_ID            = (event().egammas_ID().size()>0     ?  event().egammas_ID().back()      : NULL);
    const L1EGamma* l1egMax_Iso           = (event().egammas_Iso().size()>0    ?  event().egammas_Iso().back()     : NULL);
    // stage-2 endcaps
    const L1EGamma* l1egMax_EE_all           = (event().egammas_EE().size()>0        ?  event().egammas_EE().back()   : NULL);
    const L1EGamma* l1egMax_EE_FG            = (event().egammas_EE_FG().size()>0     ?  event().egammas_EE_FG().back()      : NULL);
    const L1EGamma* l1egMax_EE_FG_HoE        = (event().egammas_EE_FG_HoE().size()>0 ?  event().egammas_EE_FG_HoE().back()  : NULL);
    const L1EGamma* l1egMax_EE_ID            = (event().egammas_EE_ID().size()>0     ?  event().egammas_EE_ID().back()      : NULL);
    const L1EGamma* l1egMax_EE_Iso           = (event().egammas_EE_Iso().size()>0    ?  event().egammas_EE_Iso().back()     : NULL);
    //
    const L1EGamma* l1egScd_EE_all           = (event().egammas_EE().size()>1        ?  event().egammas_EE().at(event().egammas_EE().size()-2)               : NULL);
    const L1EGamma* l1egScd_EE_FG            = (event().egammas_EE_FG().size()>1     ?  event().egammas_EE_FG().at(event().egammas_EE_FG().size()-2)         : NULL);
    const L1EGamma* l1egScd_EE_FG_HoE        = (event().egammas_EE_FG_HoE().size()>1 ?  event().egammas_EE_FG_HoE().at(event().egammas_EE_FG_HoE().size()-2) : NULL);
    const L1EGamma* l1egScd_EE_ID            = (event().egammas_EE_ID().size()>1     ?  event().egammas_EE_ID().at(event().egammas_EE_ID().size()-2)         : NULL);
    const L1EGamma* l1egScd_EE_Iso           = (event().egammas_EE_Iso().size()>1    ?  event().egammas_EE_Iso().at(event().egammas_EE_Iso().size()-2)       : NULL);

    // stage-2
    m_histos.FillHisto(100+hoffset, (l1egMax_all    ? l1egMax_all->pt()   : 0.) , weight, sysNum);
    m_histos.FillHisto(101+hoffset, (l1egMax_FG     ? l1egMax_FG->pt()    : 0.) , weight, sysNum);
    m_histos.FillHisto(102+hoffset, (l1egMax_FG_HoE ? l1egMax_FG_HoE->pt(): 0.) , weight, sysNum);
    m_histos.FillHisto(105+hoffset, (l1egMax_ID     ? l1egMax_ID->pt()    : 0.) , weight, sysNum);
    m_histos.FillHisto(106+hoffset, (l1egMax_Iso    ? l1egMax_Iso->pt()   : 0.) , weight, sysNum);
    // stage-1
    m_histos.FillHisto(120+hoffset, (l1egMax_Stage1_all ? l1egMax_Stage1_all->pt()    : 0.) , weight, sysNum);
    m_histos.FillHisto(121+hoffset, (l1egMax_Stage1_Iso ? l1egMax_Stage1_Iso->pt()    : 0.) , weight, sysNum);

    // stage-2
    m_histos.FillHisto(200+hoffset, (l1egMax_all    ? l1egMax_all->hwEta()   : 0) , weight, sysNum);
    m_histos.FillHisto(201+hoffset, (l1egMax_FG     ? l1egMax_FG->hwEta()    : 0) , weight, sysNum);
    m_histos.FillHisto(202+hoffset, (l1egMax_FG_HoE ? l1egMax_FG_HoE->hwEta(): 0) , weight, sysNum);
    m_histos.FillHisto(205+hoffset, (l1egMax_ID     ? l1egMax_ID->hwEta()    : 0) , weight, sysNum);
    m_histos.FillHisto(206+hoffset, (l1egMax_Iso    ? l1egMax_Iso->hwEta()   : 0) , weight, sysNum);
    // stage-1
    m_histos.FillHisto(220+hoffset, (l1egMax_Stage1_all ? l1egMax_Stage1_all->hwEta()    : 0.) , weight, sysNum);
    m_histos.FillHisto(221+hoffset, (l1egMax_Stage1_Iso ? l1egMax_Stage1_Iso->hwEta()    : 0.) , weight, sysNum);


    // stage-2 EE
    m_histos.FillHisto(300+hoffset, (l1egMax_EE_all    ? l1egMax_EE_all->pt()   : 0.) , weight, sysNum);
    m_histos.FillHisto(301+hoffset, (l1egMax_EE_FG     ? l1egMax_EE_FG->pt()    : 0.) , weight, sysNum);
    m_histos.FillHisto(302+hoffset, (l1egMax_EE_FG_HoE ? l1egMax_EE_FG_HoE->pt(): 0.) , weight, sysNum);
    m_histos.FillHisto(305+hoffset, (l1egMax_EE_ID     ? l1egMax_EE_ID->pt()    : 0.) , weight, sysNum);
    m_histos.FillHisto(306+hoffset, (l1egMax_EE_Iso    ? l1egMax_EE_Iso->pt()   : 0.) , weight, sysNum);
    // double e/g
    m_histos.FillHisto(320+hoffset, (l1egScd_EE_all    ? l1egScd_EE_all->pt()   : 0.) , weight, sysNum);
    m_histos.FillHisto(321+hoffset, (l1egScd_EE_FG     ? l1egScd_EE_FG->pt()    : 0.) , weight, sysNum);
    m_histos.FillHisto(322+hoffset, (l1egScd_EE_FG_HoE ? l1egScd_EE_FG_HoE->pt(): 0.) , weight, sysNum);
    m_histos.FillHisto(325+hoffset, (l1egScd_EE_ID     ? l1egScd_EE_ID->pt()    : 0.) , weight, sysNum);
    m_histos.FillHisto(326+hoffset, (l1egScd_EE_Iso    ? l1egScd_EE_Iso->pt()   : 0.) , weight, sysNum);

    //cout<<"#e/g        = "<<event().egammas().size()<<"\n";
    //cout<<"#e/g FG     = "<<event().egammas_FG().size()<<"\n";
    //cout<<"#e/g FG+H/E = "<<event().egammas_FG_HoE().size()<<"\n";
    //cout<<"#e/g ID     = "<<event().egammas_ID().size()<<"\n";

    //cout<<"Max pt = "<<l1eg_ID_ptmax<<"\n";
    //for(auto egItr=event().egammas_ID().cbegin(); egItr!=event().egammas_ID().end(); egItr++)
    //{
    //    const L1EGamma& l1eg = **egItr;
    //    cout<<" "<<l1eg.pt();
    //}
    //cout<<"\n\n";
    

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
bool Analysis::passShape(const L1EGamma& l1eg, double eff)
/*****************************************************************/
{
    int ieta = abs(l1eg.hwEta());
    int et = l1eg.hwPt();
    unsigned int clusterShape = TriggerTools::convertClusterShapeTo8Bits(l1eg.ref());
    unsigned int sortedShape = clusterShape;
    //if(m_shapeLUT.find(clusterShape)!=m_shapeLUT.end())
    //{
        //sortedShape = m_shapeLUT[clusterShape];
    //}


    int beta = m_discriminantShape->GetXaxis()->FindBin(ieta);
    int bet  = m_discriminantShape->GetYaxis()->FindBin(et);
    int bshape = m_discriminantShape->GetZaxis()->FindBin(sortedShape);
    if(beta<1) beta = 1;
    if(beta>m_discriminantShape->GetNbinsX()) beta = m_discriminantShape->GetNbinsX();
    if(bet<1) bet = 1;
    if(bet>m_discriminantShape->GetNbinsY()) bet = m_discriminantShape->GetNbinsY();
    if(bshape<1) bshape = 1;
    if(bshape>m_discriminantShape->GetNbinsZ()) bshape = m_discriminantShape->GetNbinsZ();
    bool passVeto = (m_discriminantShape->GetBinContent(beta, bet, bshape)<=eff);
    //if(!(l1eg.hwQual()&0x4))
    //{
    //    cout<<"Shape "<<clusterShape<<", "<<sortedShape<<"\n";
    //}
    //cout<<ieta<<", "<<sortedShape<<", "<<passVeto<<"\n";

    //if( (bool)(l1eg.hwQual()&0x4) !=  (passVeto==1))
    //{
    //    cout<<"Diff shape "<<(bool)(l1eg.hwQual()&0x4)<<"!="<<(passVeto==1)<<"(shape="<<sortedShape<<")\n";
    //}

    //if(!passVeto)
    //{
    //    cout<<"Veto electron ieta="<<ieta<<" ET="<<et<<" shape="<<sortedShape<<"\n";
    //}
    return passVeto;
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
