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
Analysis::Analysis():IAnalysis()
/*****************************************************************/
{

}



/*****************************************************************/
Analysis::~Analysis()
/*****************************************************************/
{

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

    event().setEmulatorParameters(emulatorParams);
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

    const L1EGamma* l1egMax_all    = (event().egammasSorted().size()>0  ?  event().egammasSorted().back()   : NULL);
    //const L1EGamma* l1egMax_FG     = (event().egammas_FG().size()>0  ?     event().egammas_FG().back()      : NULL);
    //const L1EGamma* l1egMax_FG_HoE = (event().egammas_FG_HoE().size()>0  ? event().egammas_FG_HoE().back()  : NULL);
    //const L1EGamma* l1egMax_ID     = (event().egammas_ID().size()>0  ?     event().egammas_ID().back()      : NULL);


    m_histos.FillHisto(100+hoffset, (l1egMax_all    ? l1egMax_all->pt()   : 0.) , weight, sysNum);
    m_histos.FillHisto(200+hoffset, (l1egMax_all    ? l1egMax_all->hwEta()   : 0) , weight, sysNum);


    if(l1egMax_all && l1egMax_all->hwPt()>=15)
    {
        // get seed tower
        const l1t::CaloTower& seedTower = l1t::CaloTools::getTower(event().towers(), l1egMax_all->hwEta(), l1egMax_all->hwPhi());
        int seedE = seedTower.hwEtEm();
        int seedH = seedTower.hwEtHad();
        double seedHoE = (double)seedH/(double)seedE;
        int seedFG = ( (seedTower.hwQual() & (0x1<<2)) ? 1 : 0);

        // get cluster shape
        unsigned int clusterShape_all = TriggerTools::convertClusterShapeTo8Bits(l1egMax_all->ref());
        unsigned int sortedShape_all = clusterShape_all;
        if(m_shapeLUT.find(clusterShape_all)!=m_shapeLUT.end())
        {
            sortedShape_all = m_shapeLUT[clusterShape_all];
        }

        m_histos.FillHisto(300+hoffset, seedE, weight, sysNum);
        m_histos.FillHisto(301+hoffset, seedH, weight, sysNum);
        m_histos.FillHisto(302+hoffset, l1egMax_all->hwPt(), weight, sysNum);
        m_histos.FillHisto(303+hoffset, l1egMax_all->hwEta(), weight, sysNum);
        m_histos.FillHisto(304+hoffset, seedHoE, weight, sysNum);
        m_histos.FillHisto(305+hoffset, seedFG, weight, sysNum);
        //m_histos.FillHisto(306+hoffset, sortedShape_all, weight, sysNum);
        m_histos.FillHisto(306+hoffset, clusterShape_all, weight, sysNum);

        //vector<unsigned> forbiddenShapes = {64,65,68,69,73,76,77,101};
        //vector< pair<int,int>> highlights;
        //highlights.push_back( make_pair(l1egMax_all->hwEta(), l1egMax_all->hwPhi()) );
        //for(const auto& shape : forbiddenShapes)
        //{
            //if(shape==clusterShape_all)
            //{
                //TriggerTools::printTowers(event().towers(), highlights);
                //break;
            //}
        //}

        m_histos.FillNtuple(1000+hoffset, (double)event().run(), (double)event().event(), weight, sysNum);

    }


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
