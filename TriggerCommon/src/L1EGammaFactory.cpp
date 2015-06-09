/**
 *  @file  L1EGammaFactory.cpp
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


#include "Math/GenVector/LorentzVector.h"
#include "L1EGammaFactory.h"

#include "L1Trigger/L1TCalorimeter/interface/CaloTools.h"

using namespace AnHiMa;
using namespace std;


//AnHiMa::g_nullCluster = std::auto_ptr<l1t::CaloCluster>(new l1t::CaloCluster());


/*****************************************************************/
bool AnHiMa::L1EGammaSort(const L1EGamma* eg1, const L1EGamma* eg2)
/*****************************************************************/
{
    return eg1->pt() < eg2->pt();
};


/*****************************************************************/
void L1EGammaFactory::initialize(IEvent* event, TChain* chain)
/*****************************************************************/
{
    event->registerCallback((void*)this, L1EGammaFactory::callback);
    connectVariables(chain);


    m_l1t_eg_n      = 0;
    m_l1t_eg_pt     = 0;
    m_l1t_eg_eta    = 0;
    m_l1t_eg_phi    = 0;
    m_l1t_eg_hwPt   = 0;
    m_l1t_eg_hwEta  = 0;
    m_l1t_eg_hwPhi  = 0;
    m_l1t_eg_hwQual = 0;
    m_l1t_eg_hwIso  = 0;

}

/*****************************************************************/
void L1EGammaFactory::initialize(IEvent* event, TChain* chain, EGType type, const string& emulatorParams, const vector<l1t::CaloTower>* towers)
/*****************************************************************/
{
    m_type = type;
    m_towers = towers;
    m_emulatorWrapper.initialize(emulatorParams);
    //m_emulatorWrapper.initialize("/home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/data/caloStage2Params.txt");
    initialize(event, chain);
}


/*****************************************************************/
void L1EGammaFactory::connectVariables(TChain* chain)
/*****************************************************************/
{
    switch(m_type)
    {
        case STAGE1_FROMTREE:
            chain->SetBranchStatus("l1t_stage1_eg_n"     , true);
            chain->SetBranchStatus("l1t_stage1_eg_pt"    , true);
            chain->SetBranchStatus("l1t_stage1_eg_eta"   , true);
            chain->SetBranchStatus("l1t_stage1_eg_phi"   , true);
            chain->SetBranchStatus("l1t_stage1_eg_hwPt"  , true);
            chain->SetBranchStatus("l1t_stage1_eg_hwEta" , true);
            chain->SetBranchStatus("l1t_stage1_eg_hwPhi" , true);
            chain->SetBranchStatus("l1t_stage1_eg_hwQual", true);
            chain->SetBranchStatus("l1t_stage1_eg_hwIso" , true);

            chain->SetBranchAddress("l1t_stage1_eg_n"     , &m_l1t_eg_n);
            chain->SetBranchAddress("l1t_stage1_eg_pt"    , &m_l1t_eg_pt);
            chain->SetBranchAddress("l1t_stage1_eg_eta"   , &m_l1t_eg_eta);
            chain->SetBranchAddress("l1t_stage1_eg_phi"   , &m_l1t_eg_phi);
            chain->SetBranchAddress("l1t_stage1_eg_hwPt"  , &m_l1t_eg_hwPt);
            chain->SetBranchAddress("l1t_stage1_eg_hwEta" , &m_l1t_eg_hwEta);
            chain->SetBranchAddress("l1t_stage1_eg_hwPhi" , &m_l1t_eg_hwPhi);
            chain->SetBranchAddress("l1t_stage1_eg_hwQual", &m_l1t_eg_hwQual);
            chain->SetBranchAddress("l1t_stage1_eg_hwIso" , &m_l1t_eg_hwIso);
            break;
        case STAGE2_FROMTREE:
            chain->SetBranchStatus("l1t_stage2_eg_n"     , true);
            chain->SetBranchStatus("l1t_stage2_eg_pt"    , true);
            chain->SetBranchStatus("l1t_stage2_eg_eta"   , true);
            chain->SetBranchStatus("l1t_stage2_eg_phi"   , true);
            chain->SetBranchStatus("l1t_stage2_eg_hwPt"  , true);
            chain->SetBranchStatus("l1t_stage2_eg_hwEta" , true);
            chain->SetBranchStatus("l1t_stage2_eg_hwPhi" , true);
            chain->SetBranchStatus("l1t_stage2_eg_hwQual", true);
            chain->SetBranchStatus("l1t_stage2_eg_hwIso" , true);

            chain->SetBranchAddress("l1t_stage2_eg_n"     , &m_l1t_eg_n);
            chain->SetBranchAddress("l1t_stage2_eg_pt"    , &m_l1t_eg_pt);
            chain->SetBranchAddress("l1t_stage2_eg_eta"   , &m_l1t_eg_eta);
            chain->SetBranchAddress("l1t_stage2_eg_phi"   , &m_l1t_eg_phi);
            chain->SetBranchAddress("l1t_stage2_eg_hwPt"  , &m_l1t_eg_hwPt);
            chain->SetBranchAddress("l1t_stage2_eg_hwEta" , &m_l1t_eg_hwEta);
            chain->SetBranchAddress("l1t_stage2_eg_hwPhi" , &m_l1t_eg_hwPhi);
            chain->SetBranchAddress("l1t_stage2_eg_hwQual", &m_l1t_eg_hwQual);
            chain->SetBranchAddress("l1t_stage2_eg_hwIso" , &m_l1t_eg_hwIso);
            break;
        default:
            break;
    }
}



/*****************************************************************/
void L1EGammaFactory::callback(void* object)
/*****************************************************************/
{
    L1EGammaFactory* myself = reinterpret_cast<L1EGammaFactory*>(object);
    myself->update();
}


/*****************************************************************/
void L1EGammaFactory::update()
/*****************************************************************/
{
    m_data.clear();

    if(m_type==STAGE1_FROMTREE || m_type==STAGE2_FROMTREE)
    {
        for(int i=0;i<m_l1t_eg_n;i++)
        {
            ROOT::Math::LorentzVector<ROOT::Math::PtEtaPhiM4D<double> > p4((*m_l1t_eg_pt)[i], (*m_l1t_eg_eta)[i], (*m_l1t_eg_phi)[i], 0.);
            vector<l1t::CaloCluster> egClusters;
            const l1t::CaloCluster& cluster = l1t::CaloTools::getCluster(egClusters, 1, 1); // return nullCluster_
            L1EGamma eg(cluster);
            eg.setP4(p4);
            eg.setHwPt((*m_l1t_eg_hwPt)[i]);
            eg.setHwEta((*m_l1t_eg_hwEta)[i]);
            eg.setHwPhi((*m_l1t_eg_hwPhi)[i]);
            eg.setHwQual((*m_l1t_eg_hwQual)[i]);
            eg.setHwIso((*m_l1t_eg_hwIso)[i]);
            if(m_type==STAGE1_FROMTREE) eg.setParameter("type", "STAGE1_FROMTREE");
            else if(m_type==STAGE2_FROMTREE) eg.setParameter("type", "STAGE2_FROMTREE");
            m_data.push_back(eg);

        }
    }
    else if(m_type==STAGE2_REBUILD)
    {
        m_emulatorWrapper.process(*m_towers);
        const vector<l1t::EGamma>& egammas = m_emulatorWrapper.egammas();
        const vector<l1t::CaloCluster>& egClusters = m_emulatorWrapper.egClusters();
        for(auto egItr=egammas.cbegin();egItr!=egammas.end();egItr++)
        {
            const l1t::CaloCluster& cluster = l1t::CaloTools::getCluster(egClusters, egItr->hwEta(), egItr->hwPhi());
            if(!cluster.isValid()) cout<<"WARNING: Matching a non valid l1t::CaloCluster to l1t::EGamma\n";
            L1EGamma eg(*egItr, cluster);
            eg.setParameter("type", "STAGE2_REBUILD");
            m_data.push_back(eg);
        }
    }

}
