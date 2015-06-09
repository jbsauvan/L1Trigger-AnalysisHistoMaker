/**
 *  @file  EventEGammaEGamma.cpp
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/29/2014
 *
 *  @internal
 *     Created :  07/29/2014
 * Last update :  07/29/2014 06:13:15 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */



#include <iostream>

//#include "DataFormats/L1TCalorimeter/CaloTower.h"
//#include "DataFormats/L1Trigger/EGamma.h"

#include <TChain.h>

#include "EventEGamma.h"


using namespace AnHiMa;
using namespace std;

/*****************************************************************/
EventEGamma::EventEGamma():IEvent(),
    m_emulatorParameters("/home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/data/caloStage2Params.txt")
/*****************************************************************/
{

}



/*****************************************************************/
EventEGamma::~EventEGamma()
/*****************************************************************/
{
}


/*****************************************************************/
void EventEGamma::connectVariables(TChain* inputChain)
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
    // !!initialize reco objects before gen!!
    vector< const vector<L1EGamma>*> l1egammas(3);
    l1egammas[0] = &egammas();
    l1egammas[1] = &egammasRef();
    l1egammas[2] = &egammasStage1();
    m_eleFactory.initialize(this, inputChain, l1egammas);
    m_genEleFactory.initialize(this, inputChain, &electrons());
}


/*****************************************************************/
void EventEGamma::setEmulatorParameters(const string& params)
/*****************************************************************/
{
    m_emulatorParameters = params;
}

/*****************************************************************/
bool EventEGamma::passSelection(int selection)
/*****************************************************************/
{
    return true;
}


