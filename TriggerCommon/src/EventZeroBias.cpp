/**
 *  @file  EventZeroBias.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    15/09/2014
 *
 *  @internal
 *     Created :  15/09/2014
 * Last update :  15/09/2014 17:38:01
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */





#include <iostream>

//#include "DataFormats/L1TCalorimeter/CaloTower.h"
//#include "DataFormats/L1Trigger/EGamma.h"

#include <TChain.h>

#include "EventZeroBias.h"


using namespace AnHiMa;
using namespace std;

/*****************************************************************/
EventZeroBias::EventZeroBias():IEvent(),
    m_emulatorParameters("/home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/data/caloStage2Params.txt")
/*****************************************************************/
{

}



/*****************************************************************/
EventZeroBias::~EventZeroBias()
/*****************************************************************/
{
}


/*****************************************************************/
void EventZeroBias::connectVariables(TChain* inputChain)
/*****************************************************************/
{
    IEvent::connectVariables(inputChain);

    // Branches to read
    inputChain->SetBranchStatus("event" , true);
    inputChain->SetBranchStatus("run"   , true);
    inputChain->SetBranchStatus("npv"   , true);
    inputChain->SetBranchStatus("npu"   , true);
    inputChain->SetBranchStatus("rho"   , true);

    // Connect branches
    inputChain->SetBranchAddress("event"        , &m_event);
    inputChain->SetBranchAddress("run"          , &m_run);
    inputChain->SetBranchAddress("npv"          , &m_npv);
    inputChain->SetBranchAddress("npu"          , &m_npu);
    inputChain->SetBranchAddress("rho"          , &m_rho);

    // !!initialize towers before egammas!!
    m_towerFactory.initialize(this, inputChain);
    // !!initialize l1 objects before reco!!
    m_egammaStage1Factory.initialize(this, inputChain, L1EGammaFactory::EGType::STAGE1_FROMTREE, m_emulatorParameters, &towers());
    m_egammaRefFactory.initialize(this, inputChain, L1EGammaFactory::EGType::STAGE2_FROMTREE, m_emulatorParameters, &towers());
    m_egammaFactory.initialize(this, inputChain, L1EGammaFactory::EGType::STAGE2_REBUILD, m_emulatorParameters, &towers());
}


/*****************************************************************/
void EventZeroBias::setEmulatorParameters(const string& params)
/*****************************************************************/
{
    m_emulatorParameters = params;
}

/*****************************************************************/
bool EventZeroBias::passSelection(int selection)
/*****************************************************************/
{
    return true;
}


