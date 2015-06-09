/**
 *  @file  GenElectronFactory.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    08/04/2014
 *
 *  @internal
 *     Created :  08/04/2014
 * Last update :  08/04/2014 09:56:19 AM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */





#include "GenElectronFactory.h"
#include "TriggerTools.h"

using namespace AnHiMa;
using namespace std;



/*****************************************************************/
void GenElectronFactory::initialize(IEvent* event, TChain* chain)
/*****************************************************************/
{
    event->registerCallback((void*)this, GenElectronFactory::callback);
    connectVariables(chain);


    m_ele_n           = 0;
    m_ele_pt          = 0;
    m_ele_E           = 0;
    m_ele_eta         = 0;
    m_ele_phi         = 0;
    m_ele_charge      = 0;
    m_ele_recoMatch   = 0;

}

/*****************************************************************/
void GenElectronFactory::initialize(IEvent* event, TChain* chain, const vector<RecoL1Electron>* recoElectrons)
/*****************************************************************/
{
    m_recoElectrons = recoElectrons;
    initialize(event, chain);
}


/*****************************************************************/
void GenElectronFactory::connectVariables(TChain* chain)
/*****************************************************************/
{
    chain->SetBranchStatus("truthele_n"            , true);
    chain->SetBranchStatus("truthele_pt"           , true);
    chain->SetBranchStatus("truthele_E"            , true);
    chain->SetBranchStatus("truthele_eta"          , true);
    chain->SetBranchStatus("truthele_phi"          , true);
    chain->SetBranchStatus("truthele_charge"       , true);
    chain->SetBranchStatus("truthele_recoMatch"    , true);

    chain->SetBranchAddress("truthele_n"            , &m_ele_n);
    chain->SetBranchAddress("truthele_pt"           , &m_ele_pt);
    chain->SetBranchAddress("truthele_E"            , &m_ele_E);
    chain->SetBranchAddress("truthele_eta"          , &m_ele_eta);
    chain->SetBranchAddress("truthele_phi"          , &m_ele_phi);
    chain->SetBranchAddress("truthele_charge"       , &m_ele_charge);
    chain->SetBranchAddress("truthele_recoMatch"    , &m_ele_recoMatch);
}



/*****************************************************************/
void GenElectronFactory::callback(void* object)
/*****************************************************************/
{
    GenElectronFactory* myself = reinterpret_cast<GenElectronFactory*>(object);
    myself->update();
}


/*****************************************************************/
void GenElectronFactory::update()
/*****************************************************************/
{
    m_data.clear();
    for(int i=0;i<m_ele_n;i++)
    {
        GenParticle ele;
        ele.SetPtEtaPhiE((*m_ele_pt)[i], (*m_ele_eta)[i], (*m_ele_phi)[i], (*m_ele_E)[i]);
        ele.setCharge   ( (*m_ele_charge)[i]);
        ele.setPid      ( (ele.charge()<0 ? 11 : -11) );
        ele.setRecoMatch((*m_ele_recoMatch)[i]);

        // match gen electron to reco electron
        const RecoL1Electron& recoEle = TriggerTools::match(ele, *m_recoElectrons);

        m_data.push_back( GenElectron(ele,recoEle) );
    }
}
