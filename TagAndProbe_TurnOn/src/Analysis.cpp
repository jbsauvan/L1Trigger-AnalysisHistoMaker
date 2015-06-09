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

#include "TSystem.h"
#include "TVector2.h"
#include "TH2F.h"
#include "TFile.h"

#include "TDecayedParticle.h"
#include "Analysis.h"
#include "TriggerTools.h"

#include "L1Trigger/L1TCalorimeter/interface/CaloTools.h"


using namespace AnHiMa;
using namespace std;



/*****************************************************************/
Analysis::Analysis():IAnalysis(),
    m_cutsHoE(NULL),
    m_discriminantShape(NULL),
    m_shapeWorkingPoint(0.995)
/*****************************************************************/
{

}



/*****************************************************************/
Analysis::~Analysis()
/*****************************************************************/
{
    for(int i=0;i<3;++i)
    {
         m_failAndPassTrees[i]->Write();
    }

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

    const string& cutsHoEFile = m_reader.params().GetValue("CutsHoEFile", "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Identification/data/cutsHoE.root");
    const string& cutsHoEHisto = m_reader.params().GetValue("CutsHoEHisto", "cutsEff05_eta_ET");
    const string& discriminantShapeFile = m_reader.params().GetValue("ShapeDiscriminantFile", "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Identification/data/mergedDiscriminant_eta_et_shape.root");
    const string& discriminantShapeHisto = m_reader.params().GetValue("ShapeDiscriminantHisto", "shapeDiscriminant");
    m_shapeWorkingPoint = m_reader.params().GetValue("ShapeWorkingPoint", 0.995);
    cout<<"INFO: File H/E cuts: "<<cutsHoEFile<<"\n";
    cout<<"INFO: Histo H/E cuts: "<<cutsHoEHisto<<"\n";
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

    event().setEmulatorParameters(emulatorParams);


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


    ///// turn-on trees
    m_thresholds = {10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,27,30,35,40,45,50,55,60};
    m_failAndPassTrees = new TTree*[3];
    m_failAndPassTrees[0] = new TTree("TagAndProbe_TurnOn_Stage2_Rebuilt_failAndPassTree","TagAndProbe_TurnOn_Stage2_Rebuilt_failAndPassTree");
    m_failAndPassTrees[1] = new TTree("TagAndProbe_TurnOn_Stage2_Ref_failAndPassTree", "TagAndProbe_TurnOn_Stage2_Ref_failAndPassTree");
    m_failAndPassTrees[2] = new TTree("TagAndProbe_TurnOn_Stage1_failAndPassTree", "TagAndProbe_TurnOn_Stage1_failAndPassTree");
    m_outputFile->cd();
    for(int i=0;i<3;++i)
    {
        //cerr<<i<<"\n";
        TTree* tree = m_failAndPassTrees[i];
        m_offl_pt[i] = 0.;
        m_offl_eta[i] = 0.;
        stringstream offlptvar, offlptvartype, offletavar, offletavartype;
        offlptvar << "offl_pt";
        offlptvartype << "offl_pt/D";
        offletavar << "offl_eta";
        offletavartype << "offl_eta/D";
        tree->Branch(offlptvar.str().c_str(), &m_offl_pt[i], offlptvartype.str().c_str());
        tree->Branch(offletavar.str().c_str(), &m_offl_eta[i], offletavartype.str().c_str());
        for(unsigned j=0;j<m_thresholds.size();j++)
        {
            int th = m_thresholds[j];
            stringstream pass, passtype, passiso, passisotype;
            pass << "l1_pass_" << th;
            passtype << "l1_pass_" << th<<"/I";
            passiso << "l1_iso_pass_" << th;
            passisotype << "l1_iso_pass_" << th<<"/I";
            m_failAndPassBits[i][th] = 0;
            m_failAndPassBitsIso[i][th] = 0;
            tree->Branch(pass.str().c_str(), &m_failAndPassBits[i][th], passtype.str().c_str());
            tree->Branch(passiso.str().c_str(), &m_failAndPassBitsIso[i][th], passisotype.str().c_str());
        }
    }

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
    fillHistos(2);

}

/*****************************************************************/
void Analysis::fillHistos(int hTag)
/*****************************************************************/
{

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
        m_histos.FillHisto(25+hoffset, event().npu(), weight, sysNum);

        m_histos.FillHisto(30+hoffset, z.M(), weight, sysNum);
        m_histos.FillHisto(31+hoffset, z.Pt(), weight, sysNum);
        m_histos.FillHisto(32+hoffset, z.Rapidity(), weight, sysNum);
        m_histos.FillHisto(33+hoffset, z.Phi(), weight, sysNum);

        // check if electron is matched to a L1 cluster
        double l1_pt = 0.;
        bool l1_pass_FG = false;
        bool l1_pass_HoE = false;
        bool l1_pass_shape = false;
        bool l1_pass = false;
        bool l1_pass_iso = false;
        if(probe.ref(hTag).hwPt()>0)
        {
            const L1EGamma& l1eg = probe.ref(hTag);
            double l1egPt = l1eg.pt();
            if(m_applyCorr95)
            {
                double corr = 1.;
                if(hTag==0 || hTag==1)
                {
                    if(m_corrVsEta95pcStage2.find(abs(l1eg.hwEta()))!=m_corrVsEta95pcStage2.end()) 
                    {
                        corr = m_corrVsEta95pcStage2[ abs(l1eg.hwEta()) ];
                    }
                }
                else if(hTag==2)
                {
                    if(m_corrVsEta95pcStage1.find(abs(l1eg.hwEta()))!=m_corrVsEta95pcStage1.end()) 
                    {
                        corr = m_corrVsEta95pcStage1[ abs(l1eg.hwEta()) ];
                    }
                }
                l1egPt *= corr;
            }


            //cout<<"Shape "<<(bool)(l1eg.hwQual()&0x4)<<"!="<<(passVeto==1)<<"(shape="<<sortedShape<<")\n";
            unsigned int clusterShape = TriggerTools::convertClusterShapeTo8Bits(l1eg.ref());
            unsigned int sortedShape = clusterShape;
            //if(m_shapeLUT.find(clusterShape)!=m_shapeLUT.end())
            //{
                //sortedShape = m_shapeLUT[clusterShape];
            //}
            m_histos.FillHisto(2+hoffset, sortedShape, weight, sysNum);

            m_histos.FillHisto(100+hoffset, probe.Pt(), weight, sysNum);
            m_histos.FillHisto(101+hoffset, probe.Eta(), weight, sysNum);
            m_histos.FillHisto(102+hoffset, probe.Phi(), weight, sysNum);
            m_histos.FillHisto(104+hoffset, probe.bdt(), weight, sysNum);
            m_histos.FillHisto(105+hoffset, event().npu(), weight, sysNum);
            //
            if(l1eg.hwQual()&0x1) // check FG bit
            {
                m_histos.FillHisto(200+hoffset, probe.Pt(), weight, sysNum);
                m_histos.FillHisto(201+hoffset, probe.Eta(), weight, sysNum);
                m_histos.FillHisto(202+hoffset, probe.Phi(), weight, sysNum);
                m_histos.FillHisto(204+hoffset, probe.bdt(), weight, sysNum);
                m_histos.FillHisto(205+hoffset, event().npu(), weight, sysNum);
            }
            if( (l1eg.hwQual()&0x1) && (l1eg.hwQual()&0x2) ) // check FG+H/E bits
            {
                m_histos.FillHisto(300+hoffset, probe.Pt(), weight, sysNum);
                m_histos.FillHisto(301+hoffset, probe.Eta(), weight, sysNum);
                m_histos.FillHisto(302+hoffset, probe.Phi(), weight, sysNum);
                m_histos.FillHisto(304+hoffset, probe.bdt(), weight, sysNum);
                m_histos.FillHisto(305+hoffset, event().npu(), weight, sysNum);
            }
            if( (l1eg.hwQual()&0x1) && (l1eg.hwQual()&0x2) && (l1eg.hwQual()&0x4) ) // check FG+H/E+shape bits
            {
                m_histos.FillHisto(400+hoffset, probe.Pt(), weight, sysNum);
                m_histos.FillHisto(401+hoffset, probe.Eta(), weight, sysNum);
                m_histos.FillHisto(402+hoffset, probe.Phi(), weight, sysNum);
                m_histos.FillHisto(404+hoffset, probe.bdt(), weight, sysNum);
                m_histos.FillHisto(405+hoffset, event().npu(), weight, sysNum);
            }
            if( l1eg.hwIso() ) // check iso bit 
            {
                m_histos.FillHisto(500+hoffset, probe.Pt(), weight, sysNum);
                m_histos.FillHisto(501+hoffset, probe.Eta(), weight, sysNum);
                m_histos.FillHisto(502+hoffset, probe.Phi(), weight, sysNum);
                m_histos.FillHisto(504+hoffset, probe.bdt(), weight, sysNum);
                m_histos.FillHisto(505+hoffset, event().npu(), weight, sysNum);
            }
            //if( (hTag==0 && (l1eg.hwQual()&0x1) && passHoE(l1eg) && passShape(l1eg) && l1eg.hwIso() )
            if( (hTag==0 && (l1eg.hwQual()&0x1) && (l1eg.hwQual()&0x2) && passShape(l1eg, m_shapeWorkingPoint) && l1eg.hwIso() )
            //if( (hTag==0 && (l1eg.hwQual()&0x1) && (l1eg.hwQual()&0x2) && (l1eg.hwQual()&0x4) && l1eg.hwIso() )
                || (hTag==2 && l1eg.hwIso()) ) // check FG+H/E+shape+iso bits
            {
                m_histos.FillHisto(510+hoffset, probe.Pt(), weight, sysNum);
                m_histos.FillHisto(511+hoffset, probe.Eta(), weight, sysNum);
                m_histos.FillHisto(512+hoffset, probe.Phi(), weight, sysNum);
                m_histos.FillHisto(514+hoffset, probe.bdt(), weight, sysNum);
                m_histos.FillHisto(515+hoffset, event().npu(), weight, sysNum);
            }
            //if( hTag==0 && passHoE(l1eg) ) // check H/E new
            if( hTag==0 && (l1eg.hwQual()&0x2) )
            {
                m_histos.FillHisto(600+hoffset, probe.Pt(), weight, sysNum);
                m_histos.FillHisto(601+hoffset, probe.Eta(), weight, sysNum);
                m_histos.FillHisto(602+hoffset, probe.Phi(), weight, sysNum);
                m_histos.FillHisto(604+hoffset, probe.bdt(), weight, sysNum);
                m_histos.FillHisto(605+hoffset, event().npu(), weight, sysNum);
            }
            if( hTag==0 && passShape(l1eg,m_shapeWorkingPoint) ) // check shape new
            //if( hTag==0 && (l1eg.hwQual()&0x4) )
            {
                m_histos.FillHisto(610+hoffset, probe.Pt(), weight, sysNum);
                m_histos.FillHisto(611+hoffset, probe.Eta(), weight, sysNum);
                m_histos.FillHisto(612+hoffset, probe.Phi(), weight, sysNum);
                m_histos.FillHisto(614+hoffset, probe.bdt(), weight, sysNum);
                m_histos.FillHisto(615+hoffset, event().npu(), weight, sysNum);
            }
            //if( hTag==0 && passHoE(l1eg) && passShape(l1eg) ) // check H/E and shape new
            if( hTag==0 && (l1eg.hwQual()&0x2) && passShape(l1eg,m_shapeWorkingPoint) )
            //if( hTag==0 && (l1eg.hwQual()&0x2) && (l1eg.hwQual()&0x4) )
            {
                m_histos.FillHisto(620+hoffset, probe.Pt(), weight, sysNum);
                m_histos.FillHisto(621+hoffset, probe.Eta(), weight, sysNum);
                m_histos.FillHisto(622+hoffset, probe.Phi(), weight, sysNum);
                m_histos.FillHisto(624+hoffset, probe.bdt(), weight, sysNum);
                m_histos.FillHisto(625+hoffset, event().npu(), weight, sysNum);
            }
            //if( hTag==0 && (l1eg.hwQual()&0x1) && passHoE(l1eg) && passShape(l1eg) ) // check FG,  H/E new and shape new
            if( hTag==0 && (l1eg.hwQual()&0x1) && (l1eg.hwQual()&0x2) && passShape(l1eg,m_shapeWorkingPoint) )
            //if( hTag==0 && (l1eg.hwQual()&0x1) && (l1eg.hwQual()&0x2) && (l1eg.hwQual()&0x4) )
            {
                m_histos.FillHisto(630+hoffset, probe.Pt(), weight, sysNum);
                m_histos.FillHisto(631+hoffset, probe.Eta(), weight, sysNum);
                m_histos.FillHisto(632+hoffset, probe.Phi(), weight, sysNum);
                m_histos.FillHisto(634+hoffset, probe.bdt(), weight, sysNum);
                m_histos.FillHisto(635+hoffset, event().npu(), weight, sysNum);
            }

            double ptRatio = (l1egPt-probe.Pt())/probe.Pt();
            double etRatio = (l1egPt-probe.caloEt())/probe.caloEt();
            //double etRatio = ((double)l1eg.hwPt()/2.-probe.caloEt())/probe.caloEt();
            double deta = l1eg.eta()-probe.Eta();
            double dphi = TVector2::Phi_mpi_pi(l1eg.phi()-probe.Phi());
            m_histos.Fill1BinHisto(1000+hoffset, fabs(probe.superClusterEta()), ptRatio, weight, sysNum);
            m_histos.Fill1BinHisto(1010+hoffset, probe.Pt(), ptRatio, weight, sysNum);
            m_histos.Fill1BinHisto(1020+hoffset, fabs(probe.superClusterEta()), etRatio, weight, sysNum);
            m_histos.Fill1BinHisto(1030+hoffset, probe.Pt(), etRatio, weight, sysNum);
            m_histos.Fill1BinHisto(1040+hoffset, fabs(probe.superClusterEta()), deta, weight, sysNum);
            m_histos.Fill1BinHisto(1050+hoffset, probe.Pt(), deta, weight, sysNum);
            m_histos.Fill1BinHisto(1060+hoffset, fabs(probe.superClusterEta()), dphi, weight, sysNum);
            m_histos.Fill1BinHisto(1070+hoffset, probe.Pt(), dphi, weight, sysNum);


            l1_pass_FG    = ( (hTag==0 && (l1eg.hwQual()&0x1)) || hTag==2);
            //l1_pass_HoE   = ( (hTag==0 && passHoE(l1eg)) || hTag==2);
            l1_pass_HoE   = ( (hTag==0 && (l1eg.hwQual()&0x2)) || hTag==2);
            l1_pass_shape = ( (hTag==0 && passShape(l1eg,m_shapeWorkingPoint)) || hTag==2);
            //l1_pass_shape = ( (hTag==0 && (l1eg.hwQual()&0x4)) || hTag==2);
            //l1_pass       = ( (hTag==0 && (l1eg.hwQual()&0x1) && passHoE(l1eg) && passShape(l1eg)) || hTag==2);
            l1_pass       = ( (hTag==0 && (l1eg.hwQual()&0x1) && (l1eg.hwQual()&0x2) && passShape(l1eg,m_shapeWorkingPoint)) || hTag==2);
            //l1_pass       = ( (hTag==0 && (l1eg.hwQual()&0x1) && (l1eg.hwQual()&0x2) && (l1eg.hwQual()&0x4)) || hTag==2);
            l1_pass_iso   = (l1_pass && l1eg.hwIso());
            l1_pt         = l1egPt;
        }
        //else
        //{
        //    if(hTag==2)
        //    {
        //        vector<int> ttieta = probe.listTTIeta();
        //        vector<int> ttiphi = probe.listTTIphi();
        //        vector< pair<int,int> > ttpair;
        //        vector<int> rctieta = probe.listRCTIeta();
        //        vector<int> rctiphi = probe.listRCTIphi();
        //        vector< pair<int,int> > rctpair;
        //        for(unsigned i=0;i<ttieta.size();i++)
        //        {
        //            ttpair.push_back(std::make_pair(ttieta[i],ttiphi[i]));
        //        }
        //        for(unsigned i=0;i<rctieta.size();i++)
        //        {
        //            rctpair.push_back(std::make_pair(rctieta[i],rctiphi[i]));
        //        }
        //        cout<<"Probe not matched. pt="<<probe.Pt()<<", eta="<<probe.Eta()<<", bdt="<<probe.bdt()<<"\n";
        //        cout<<"  Number of Z candidates = "<<event().ZCandidates().size()<<"\n";
        //        cout<<"  TOWERS:\n";
        //        TriggerTools::printTowers(event().towers(), ttpair);
        //        cout<<"############################################\n";
        //        cout<<"\n";
        //        cout<<"  CLUSTERS:\n";
        //        TriggerTools::printStage1Clusters(event().egammasStage1(), rctpair);
        //        cout<<"\n\n";
        //    }
        //}
        //m_histos.FillHisto(2000+hoffset, probe.Pt(), weight, sysNum);
        //m_histos.FillHisto(2001+hoffset, probe.Eta(), weight, sysNum);
        //m_histos.FillHisto(2002+hoffset, l1_pt, weight, sysNum);
        //m_histos.FillHisto(2003+hoffset, l1_pass_FG, weight, sysNum);
        //m_histos.FillHisto(2004+hoffset, l1_pass_HoE, weight, sysNum);
        //m_histos.FillHisto(2005+hoffset, l1_pass_shape, weight, sysNum);
        //
        m_offl_pt[hTag] = probe.Pt();
        m_offl_eta[hTag] = probe.Eta();
        for(unsigned i=0;i<m_thresholds.size();i++)
        {
            int th = m_thresholds[i];
            m_failAndPassBits[hTag][th] = (l1_pt>=(double)th && l1_pass);
            m_failAndPassBitsIso[hTag][th] = (l1_pt>=(double)th && l1_pass_iso);
        }

        //m_histos.FillNtuple(2100+hoffset, (double)event().run(), (double)event().event(), weight, sysNum);
        m_failAndPassTrees[hTag]->Fill();
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

    //if( (bool)(l1eg.hwQual()&0x2) !=  (seedHoE<=cut))
    //{
    //    cout<<"Diff H/E "<<(bool)(l1eg.hwQual()&0x2)<<"!="<<(seedHoE<=cut)<<"(H/E="<<seedHoE<<"->"<<l1eg.ref().hOverE()<<",cut="<<cut<<")\n";
    //}

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

