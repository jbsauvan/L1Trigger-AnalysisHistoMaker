/**
 *  @file  GenJetFactory.cpp
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





#include "GenJetFactory.h"
#include "TriggerTools.h"

using namespace AnHiMa;
using namespace std;



/*****************************************************************/
void GenJetFactory::initialize(IEvent* event, TChain* chain)
/*****************************************************************/
{
    event->registerCallback((void*)this, GenJetFactory::callback);
    connectVariables(chain);


    m_jet_n           = 0;
    m_jet_pt          = 0;
    m_jet_E           = 0;
    m_jet_eta         = 0;
    m_jet_phi         = 0;
    m_jet_emEnergy    = 0;
    m_jet_hadEnergy   = 0;
    m_jet_invEnergy   = 0;

}


/*****************************************************************/
void GenJetFactory::connectVariables(TChain* chain)
/*****************************************************************/
{
    chain->SetBranchStatus("truthjet_n"            , true);
    chain->SetBranchStatus("truthjet_pt"           , true);
    chain->SetBranchStatus("truthjet_energy"       , true);
    chain->SetBranchStatus("truthjet_eta"          , true);
    chain->SetBranchStatus("truthjet_phi"          , true);
    chain->SetBranchStatus("truthjet_emenergy"     , true);
    chain->SetBranchStatus("truthjet_hadenergy"    , true);
    chain->SetBranchStatus("truthjet_invenergy"    , true);

    chain->SetBranchAddress("truthjet_n"            , &m_jet_n);
    chain->SetBranchAddress("truthjet_pt"           , &m_jet_pt);
    chain->SetBranchAddress("truthjet_energy"       , &m_jet_E);
    chain->SetBranchAddress("truthjet_eta"          , &m_jet_eta);
    chain->SetBranchAddress("truthjet_phi"          , &m_jet_phi);
    chain->SetBranchAddress("truthjet_emenergy"     , &m_jet_emEnergy);
    chain->SetBranchAddress("truthjet_hadenergy"    , &m_jet_hadEnergy);
    chain->SetBranchAddress("truthjet_invenergy"    , &m_jet_invEnergy);
}



/*****************************************************************/
void GenJetFactory::callback(void* object)
/*****************************************************************/
{
    GenJetFactory* myself = reinterpret_cast<GenJetFactory*>(object);
    myself->update();
}


/*****************************************************************/
void GenJetFactory::update()
/*****************************************************************/
{
    m_data.clear();
    for(int i=0;i<m_jet_n;i++)
    {
        GenJet jet;
        jet.SetPtEtaPhiE((*m_jet_pt)[i], (*m_jet_eta)[i], (*m_jet_phi)[i], (*m_jet_E)[i]);
        jet.setEmEnergy((*m_jet_emEnergy)[i]);
        jet.setHadEnergy((*m_jet_hadEnergy)[i]);
        jet.setInvEnergy((*m_jet_invEnergy)[i]);

        m_data.push_back( jet );
    }
}
