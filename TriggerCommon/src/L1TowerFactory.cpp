/**
 *  @file  L1TowerFactory.cpp
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
#include "L1TowerFactory.h"

using namespace AnHiMa;



/*****************************************************************/
void L1TowerFactory::initialize(IEvent* event, TChain* chain)
/*****************************************************************/
{
    event->registerCallback((void*)this, L1TowerFactory::callback);
    connectVariables(chain);

    m_l1t_tower_n         = 0;
    m_l1t_tower_hwEta     = 0;
    m_l1t_tower_hwPhi     = 0;
    m_l1t_tower_hwEtEm    = 0;
    m_l1t_tower_hwEtHad   = 0;
    m_l1t_tower_hwEtRatio = 0;
    m_l1t_tower_hwQual    = 0;

}



/*****************************************************************/
void L1TowerFactory::connectVariables(TChain* chain)
/*****************************************************************/
{
    chain->SetBranchStatus("l1t_stage2_tower_n"         , true);
    chain->SetBranchStatus("l1t_stage2_tower_hwEta"     , true);
    chain->SetBranchStatus("l1t_stage2_tower_hwPhi"     , true);
    chain->SetBranchStatus("l1t_stage2_tower_hwEtEm"    , true);
    chain->SetBranchStatus("l1t_stage2_tower_hwEtHad"   , true);
    chain->SetBranchStatus("l1t_stage2_tower_hwEtRatio" , true);
    chain->SetBranchStatus("l1t_stage2_tower_hwQual"    , true);


    chain->SetBranchAddress("l1t_stage2_tower_n"         , &m_l1t_tower_n);
    chain->SetBranchAddress("l1t_stage2_tower_hwEta"     , &m_l1t_tower_hwEta);
    chain->SetBranchAddress("l1t_stage2_tower_hwPhi"     , &m_l1t_tower_hwPhi);
    chain->SetBranchAddress("l1t_stage2_tower_hwEtEm"    , &m_l1t_tower_hwEtEm);
    chain->SetBranchAddress("l1t_stage2_tower_hwEtHad"   , &m_l1t_tower_hwEtHad);
    chain->SetBranchAddress("l1t_stage2_tower_hwEtRatio" , &m_l1t_tower_hwEtRatio);
    chain->SetBranchAddress("l1t_stage2_tower_hwQual"    , &m_l1t_tower_hwQual);

}



/*****************************************************************/
void L1TowerFactory::callback(void* object)
/*****************************************************************/
{
    L1TowerFactory* myself = reinterpret_cast<L1TowerFactory*>(object);
    myself->update();
}


/*****************************************************************/
void L1TowerFactory::update()
/*****************************************************************/
{
    m_data.clear();
    for(int i=0;i<m_l1t_tower_n;i++)
    {
        l1t::CaloTower tower;
        tower.setHwEta    ( (*m_l1t_tower_hwEta)[i] );
        tower.setHwPhi    ( (*m_l1t_tower_hwPhi)[i] );
        tower.setHwEtEm   ( (*m_l1t_tower_hwEtEm)[i] );
        tower.setHwEtHad  ( (*m_l1t_tower_hwEtHad)[i] );
        tower.setHwPt     ( (*m_l1t_tower_hwEtEm)[i] + (*m_l1t_tower_hwEtHad)[i] );
        tower.setHwEtRatio( (*m_l1t_tower_hwEtRatio)[i] );
        tower.setHwQual   ( (*m_l1t_tower_hwQual)[i] );
        m_data.push_back(tower);

    }
}
