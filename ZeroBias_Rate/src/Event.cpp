/**
 *  @file  Event.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    15/09/2014
 *
 *  @internal
 *     Created :  15/09/2014
 * Last update :  15/09/2014 17:56:17
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */







#include <iostream>
#include <fstream>
#include "Event.h"
#include "TriggerTools.h"

#include "L1Trigger/L1TCalorimeter/interface/CaloTools.h"


using namespace AnHiMa;
using namespace std;

/*****************************************************************/
Event::Event():EventZeroBias()
/*****************************************************************/
{


}



/*****************************************************************/
Event::~Event()
/*****************************************************************/
{
}


/*****************************************************************/
void Event::connectVariables(TChain* inputChain)
/*****************************************************************/
{
    EventZeroBias::connectVariables(inputChain);

    registerCallback((void*)this, Event::callback);
}


/*****************************************************************/
bool Event::passSelection(int selection)
/*****************************************************************/
{
    return EventZeroBias::passSelection(selection);
}


/*****************************************************************/
void Event::callback(void* object)
/*****************************************************************/
{
    Event* myself = reinterpret_cast<Event*>(object);
    myself->updateSelectedObjects();
}


/*****************************************************************/
void Event::updateSelectedObjects()
/*****************************************************************/
{
    m_egammasStage1_Ecorr.clear();
    m_egammasStage1_Sort.clear();
    m_egammasStage1_Iso.clear();
    m_egammas_Ecorr.clear();
    m_egammas_Sort.clear();
    m_egammas_FG.clear();
    m_egammas_HoE.clear();
    m_egammas_Shape.clear();
    m_egammas_FG_HoE.clear();
    m_egammas_FG_HoE_Shape.clear();
    m_egammas_FG_HoE_Shape_Iso.clear();
    //
    m_egammas_EE.clear();
    m_egammas_EE_FG.clear();
    m_egammas_EE_HoE.clear();
    m_egammas_EE_Shape.clear();
    m_egammas_EE_FG_HoE.clear();
    m_egammas_EE_FG_HoE_Shape.clear();
    m_egammas_EE_FG_HoE_Shape_Iso.clear();

    /// Stage-1
    // correct energy
    for(auto egItr=egammasStage1().cbegin(); egItr!=egammasStage1().end(); egItr++ )
    {
        const L1EGamma& l1eg = *egItr;
        //if(abs(l1eg.hwEta()))
        m_egammasStage1_Ecorr.push_back(l1eg);
        double corr = 1.;
        if(m_applyCorr95 && m_corrVsEta95pcStage1.find(abs(l1eg.hwEta()))!=m_corrVsEta95pcStage1.end()) corr = m_corrVsEta95pcStage1[ abs(l1eg.hwEta()) ];
        math::PtEtaPhiMLorentzVector corrP4(l1eg.pt()*corr, l1eg.eta(), l1eg.phi(), 0.);
        m_egammasStage1_Ecorr.back().setP4(corrP4);
    }
    // sort
    for(auto egItr=m_egammasStage1_Ecorr.cbegin(); egItr!=m_egammasStage1_Ecorr.end(); egItr++ )
    {
        const L1EGamma& l1eg = *egItr;
        m_egammasStage1_Sort.push_back( &l1eg );
    }
    sort(m_egammasStage1_Sort.begin(), m_egammasStage1_Sort.end(), L1EGammaSort);
    // apply selections
    for(auto egItr=egammasStage1Sorted().cbegin(); egItr!=egammasStage1Sorted().end(); egItr++ )
    {
        const L1EGamma* l1eg = *egItr;
        if( l1eg->hwIso() ) 
        {
            m_egammasStage1_Iso.push_back(l1eg);
        }
    }


    /// Stage-2
    // correct energy
    for(auto egItr=egammas().cbegin(); egItr!=egammas().end(); egItr++ )
    {
        const L1EGamma& l1eg = *egItr;
        //if(abs(l1eg.hwEta()))
        m_egammas_Ecorr.push_back(l1eg);
        double corr = 1.;
        if(m_applyCorr95 && m_corrVsEta95pcStage2.find(abs(l1eg.hwEta()))!=m_corrVsEta95pcStage2.end()) corr = m_corrVsEta95pcStage2[ abs(l1eg.hwEta()) ];
        math::PtEtaPhiMLorentzVector corrP4(l1eg.pt()*corr, l1eg.eta(), l1eg.phi(), 0.);
        m_egammas_Ecorr.back().setP4(corrP4);
    }
    // sort
    for(auto egItr=m_egammas_Ecorr.cbegin(); egItr!=m_egammas_Ecorr.end(); egItr++ )
    {
        const L1EGamma& l1eg = *egItr;
        m_egammas_Sort.push_back( &l1eg );
    }
    sort(m_egammas_Sort.begin(), m_egammas_Sort.end(), L1EGammaSort);

    // apply selections
    for(auto egItr=egammasSorted().cbegin(); egItr!=egammasSorted().end(); egItr++ )
    {
        const L1EGamma* l1eg = *egItr;
        if(fabs(l1eg->eta())>1.479) m_egammas_EE.push_back(l1eg);
        if(l1eg->hwQual()&0x1) // check FG bit 
        {
            m_egammas_FG.push_back(l1eg);
            if(fabs(l1eg->eta())>1.479) m_egammas_EE_FG.push_back(l1eg);
        }
        if(l1eg->hwQual()&0x2) // check H/E bit 
        //if( passHoE(*l1eg) ) 
        {
            m_egammas_HoE.push_back(l1eg);
            if(fabs(l1eg->eta())>1.479) m_egammas_EE_HoE.push_back(l1eg);
        }
        //if(l1eg->hwQual()&0x4) // check shape bit 
        //if( passShape(*l1eg) ) 
        if( passShape(*l1eg, m_shapeWorkingPoint) ) // check shape new
        {
            m_egammas_Shape.push_back(l1eg);
            if(fabs(l1eg->eta())>1.479) m_egammas_EE_Shape.push_back(l1eg);
        }
        if(l1eg->hwQual()&0x1 && l1eg->hwQual()&0x2) // check FG and H/E bits
        //if( (l1eg->hwQual()&0x1) && passHoE(*l1eg) ) 
        {
            m_egammas_FG_HoE.push_back(l1eg);
            if(fabs(l1eg->eta())>1.479) m_egammas_EE_FG_HoE.push_back(l1eg);
        }
        //if(l1eg->hwQual()&0x1 && l1eg->hwQual()&0x2 && l1eg->hwQual()&0x4) // check FG, H/E and shape bits
        //if( (l1eg->hwQual()&0x1) && passHoE(*l1eg) && passShape(*l1eg) ) 
        if( (l1eg->hwQual()&0x1) && (l1eg->hwQual()&0x2) && passShape(*l1eg,m_shapeWorkingPoint) )
        {
            m_egammas_FG_HoE_Shape.push_back(l1eg);
            if(fabs(l1eg->eta())>1.479) m_egammas_EE_FG_HoE_Shape.push_back(l1eg);
        }
        //if( (l1eg->hwQual()&0x1) && (l1eg->hwQual()&0x2) && (l1eg->hwQual()&0x4) && l1eg->hwIso() ) 
        //if( (l1eg->hwQual()&0x1) && passHoE(*l1eg) && passShape(*l1eg) && l1eg->hwIso() ) 
        if( (l1eg->hwQual()&0x1) && (l1eg->hwQual()&0x2) && passShape(*l1eg, m_shapeWorkingPoint) && l1eg->hwIso() )
        {
            m_egammas_FG_HoE_Shape_Iso.push_back(l1eg);
            if(fabs(l1eg->eta())>1.479) m_egammas_EE_FG_HoE_Shape_Iso.push_back(l1eg);
        }
    }
}


/*****************************************************************/
bool Event::passHoE(const L1EGamma& l1eg)
/*****************************************************************/
{
    int ieta = abs(l1eg.hwEta());
    int et = l1eg.hwPt();
    const l1t::CaloTower& seedTower = l1t::CaloTools::getTower(towers(), l1eg.hwEta(), l1eg.hwPhi());
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
bool Event::passShape(const L1EGamma& l1eg, double eff)
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
void Event::readShapeLUT(const string& file)
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


