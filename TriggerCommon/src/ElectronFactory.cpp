/**
 *  @file  ElectronFactory.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/29/2014
 *
 *  @internal
 *     Created :  07/29/2014
 * Last update :  07/29/2014 09:57:21 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */



#include "ElectronFactory.h"
#include "TriggerTools.h"

using namespace AnHiMa;
using namespace std;


/*****************************************************************/
bool AnHiMa::RecoL1ElectronSort(const RecoL1Electron* eg1, const RecoL1Electron* eg2)
/*****************************************************************/
{
    return eg1->Pt() < eg2->Pt();
};


/*****************************************************************/
void ElectronFactory::initialize(IEvent* event, TChain* chain)
/*****************************************************************/
{
    event->registerCallback((void*)this, ElectronFactory::callback);
    connectVariables(chain);


    m_ele_n           = 0;
    m_ele_pt          = 0;
    m_ele_et          = 0;
    m_ele_E           = 0;
    m_ele_eta         = 0;
    m_ele_phi         = 0;
    m_ele_charge      = 0;
    m_ele_sc_E        = 0;
    m_ele_sc_eta      = 0;
    m_ele_sc_phi      = 0;
    m_ele_ncl         = 0;
    m_ele_class       = 0;
    m_ele_isEB        = 0;
    m_ele_isEE        = 0;
    m_ele_isEBEEGap   = 0;
    m_ele_isEBGap     = 0;
    m_ele_isEEGap     = 0;
    m_ele_bdt         = 0;
    m_ele_pass        = 0;
    m_ele_seedtt_ieta = 0;
    m_ele_seedtt_iphi = 0;
    m_ele_tt_ieta     = 0;
    m_ele_tt_iphi     = 0;
    m_ele_tt_E        = 0;
    m_ele_rct_ieta    = 0;
    m_ele_rct_iphi    = 0;


}

/*****************************************************************/
void ElectronFactory::initialize(IEvent* event, TChain* chain, const vector< const vector<L1EGamma>*>& egammas)
/*****************************************************************/
{
    m_l1EGammas = egammas;
    initialize(event, chain);
}


/*****************************************************************/
void ElectronFactory::connectVariables(TChain* chain)
/*****************************************************************/
{
    chain->SetBranchStatus("ele_n"            , true);
    chain->SetBranchStatus("ele_pt"           , true);
    chain->SetBranchStatus("ele_et"           , true);
    chain->SetBranchStatus("ele_E"            , true);
    chain->SetBranchStatus("ele_eta"          , true);
    chain->SetBranchStatus("ele_phi"          , true);
    chain->SetBranchStatus("ele_charge"       , true);
    chain->SetBranchStatus("ele_sc_E"         , true);
    chain->SetBranchStatus("ele_sc_eta"       , true);
    chain->SetBranchStatus("ele_sc_phi"       , true);
    chain->SetBranchStatus("ele_ncl"          , true);
    chain->SetBranchStatus("ele_class"        , true);
    chain->SetBranchStatus("ele_isEB"         , true);
    chain->SetBranchStatus("ele_isEE"         , true);
    chain->SetBranchStatus("ele_isEBEEGap"    , true);
    chain->SetBranchStatus("ele_isEBGap"      , true);
    chain->SetBranchStatus("ele_isEEGap"      , true);
    chain->SetBranchStatus("ele_bdt"          , true);
    chain->SetBranchStatus("ele_pass"         , true);
    chain->SetBranchStatus("ele_seedtt_ieta"  , true);
    chain->SetBranchStatus("ele_seedtt_iphi"  , true);
    chain->SetBranchStatus("ele_tt_ieta"      , true);
    chain->SetBranchStatus("ele_tt_iphi"      , true);
    chain->SetBranchStatus("ele_tt_E"         , true);
    chain->SetBranchStatus("ele_rct_ieta"     , true);
    chain->SetBranchStatus("ele_rct_iphi"     , true);

    chain->SetBranchAddress("ele_n"            , &m_ele_n);
    chain->SetBranchAddress("ele_pt"           , &m_ele_pt);
    chain->SetBranchAddress("ele_et"           , &m_ele_et);
    chain->SetBranchAddress("ele_E"            , &m_ele_E);
    chain->SetBranchAddress("ele_eta"          , &m_ele_eta);
    chain->SetBranchAddress("ele_phi"          , &m_ele_phi);
    chain->SetBranchAddress("ele_charge"       , &m_ele_charge);
    chain->SetBranchAddress("ele_sc_E"         , &m_ele_sc_E);
    chain->SetBranchAddress("ele_sc_eta"       , &m_ele_sc_eta);
    chain->SetBranchAddress("ele_sc_phi"       , &m_ele_sc_phi);
    chain->SetBranchAddress("ele_ncl"          , &m_ele_ncl);
    chain->SetBranchAddress("ele_class"        , &m_ele_class);
    chain->SetBranchAddress("ele_isEB"         , &m_ele_isEB);
    chain->SetBranchAddress("ele_isEE"         , &m_ele_isEE);
    chain->SetBranchAddress("ele_isEBEEGap"    , &m_ele_isEBEEGap);
    chain->SetBranchAddress("ele_isEBGap"      , &m_ele_isEBGap);
    chain->SetBranchAddress("ele_isEEGap"      , &m_ele_isEEGap);
    chain->SetBranchAddress("ele_bdt"          , &m_ele_bdt);
    chain->SetBranchAddress("ele_pass"         , &m_ele_pass);
    chain->SetBranchAddress("ele_seedtt_ieta"  , &m_ele_seedtt_ieta);
    chain->SetBranchAddress("ele_seedtt_iphi"  , &m_ele_seedtt_iphi);
    chain->SetBranchAddress("ele_tt_ieta"      , &m_ele_tt_ieta);
    chain->SetBranchAddress("ele_tt_iphi"      , &m_ele_tt_iphi);
    chain->SetBranchAddress("ele_tt_E"         , &m_ele_tt_E);
    chain->SetBranchAddress("ele_rct_ieta"     , &m_ele_rct_ieta);
    chain->SetBranchAddress("ele_rct_iphi"     , &m_ele_rct_iphi);
}



/*****************************************************************/
void ElectronFactory::callback(void* object)
/*****************************************************************/
{
    ElectronFactory* myself = reinterpret_cast<ElectronFactory*>(object);
    myself->update();
}


/*****************************************************************/
void ElectronFactory::update()
/*****************************************************************/
{
    m_data.clear();
    for(int i=0;i<m_ele_n;i++)
    {
        Electron ele;
        ele.SetPtEtaPhiE((*m_ele_pt)[i], (*m_ele_eta)[i], (*m_ele_phi)[i], (*m_ele_E)[i]);
        ele.setCharge            ( (*m_ele_charge)[i]);
        ele.setCaloEt            ( (*m_ele_et)[i]);
        ele.setSuperClusterEnergy( (*m_ele_sc_E)[i]);
        ele.setSuperClusterEta   ( (*m_ele_sc_eta)[i]);
        ele.setSuperClusterPhi   ( (*m_ele_sc_phi)[i]);
        ele.setNClusters         ( (*m_ele_ncl)[i]);
        ele.setClassification    ( (*m_ele_class)[i]);
        ele.setIsEB              ( (*m_ele_isEB)[i]);
        ele.setIsEE              ( (*m_ele_isEE)[i]);
        ele.setIsEBEEGap         ( (*m_ele_isEBEEGap)[i]);
        ele.setIsEBGap           ( (*m_ele_isEBGap)[i]);
        ele.setIsEEGap           ( (*m_ele_isEEGap)[i]);
        ele.setBdt               ( (*m_ele_bdt)[i]);
        ele.setPass              ( (*m_ele_pass)[i]);
        ele.setSeedTTIeta        ( (*m_ele_seedtt_ieta)[i]);
        ele.setSeedTTIphi        ( (*m_ele_seedtt_iphi)[i]);
        ele.setListTTIeta        ( (*m_ele_tt_ieta)[i]);
        ele.setListTTIphi        ( (*m_ele_tt_iphi)[i]);
        ele.setListTTEnergy      ( (*m_ele_tt_E)[i]);
        ele.setListRCTIeta       ( (*m_ele_rct_ieta)[i]);
        ele.setListRCTIphi       ( (*m_ele_rct_iphi)[i]);

        // match reco electron to L1 e/g clusters
        vector< const L1EGamma* > matchedEGamma;
        for(auto egsItr=m_l1EGammas.cbegin(); egsItr!=m_l1EGammas.end(); egsItr++)
        {
            const vector<L1EGamma>* l1EGammas = *egsItr;
            bool rctLevel = ( (l1EGammas->size()>0 && l1EGammas->at(0).parameter("type")=="STAGE1_FROMTREE") ? true : false );
            matchedEGamma.push_back(&TriggerTools::match(ele, *l1EGammas, rctLevel));
        }

        m_data.push_back( RecoL1Electron(ele,matchedEGamma) );
    }
}
