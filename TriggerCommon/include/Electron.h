/**
 *  @file  Electron.h
 *  @brief  
 *
 *
 *  @author  Jean-Baptiste Sauvan <sauvan@llr.in2p3.fr>
 *
 *  @date    07/29/2014
 *
 *  @internal
 *     Created :  07/29/2014
 * Last update :  07/29/2014 01:47:09 PM
 *          by :  JB Sauvan
 *
 * =====================================================================================
 */



#ifndef ELECTRON_H
#define ELECTRON_H

#include "TLorentzVector.h"

namespace AnHiMa
{
    class Electron: public TLorentzVector
    {
        public:
            Electron();
            ~Electron();

            int                       charge()             const {return m_charge;}
            double                    caloEt()             const {return m_et;}
            double                    superClusterEnergy() const {return m_sc_E;}
            double                    superClusterEta()    const {return m_sc_eta;}
            double                    superClusterPhi()    const {return m_sc_phi;}
            int                       nClusters()          const {return m_ncl;}
            int                       classification()     const {return m_class;}
            bool                      isEB()               const {return m_isEB;}
            bool                      isEE()               const {return m_isEE;}
            bool                      isEBEEGap()          const {return m_isEBEEGap;}
            bool                      isEBGap()            const {return m_isEBGap;}
            bool                      isEEGap()            const {return m_isEEGap;}
            double                    bdt()                const {return m_bdt;}
            bool                      pass()               const {return m_pass;}
            int                       seedTTIeta()         const {return m_seedtt_ieta;}
            int                       seedTTIphi()         const {return m_seedtt_iphi;}
            const std::vector<int>&   listTTIeta()         const {return m_tt_ieta;}
            const std::vector<int>&   listTTIphi()         const {return m_tt_iphi;}
            const std::vector<float>& listTTEnergy()       const {return m_tt_E;}
            const std::vector<int>&   listRCTIeta()        const {return m_rct_ieta;}
            const std::vector<int>&   listRCTIphi()        const {return m_rct_iphi;}

            void setCharge            (int charge)                       {m_charge      = charge;}
            void setCaloEt            (double et)                        {m_et          = et;}
            void setSuperClusterEnergy(double sc_E)                      {m_sc_E        = sc_E;}
            void setSuperClusterEta   (double sc_eta)                    {m_sc_eta      = sc_eta;}
            void setSuperClusterPhi   (double sc_phi)                    {m_sc_phi      = sc_phi;}
            void setNClusters         (int ncl)                          {m_ncl         = ncl;}
            void setClassification    (int cl)                           {m_class       = cl;}
            void setIsEB              (bool isEB)                        {m_isEB        = isEB;}
            void setIsEE              (bool isEE)                        {m_isEE        = isEE;}
            void setIsEBEEGap         (bool isEBEEGap)                   {m_isEBEEGap   = isEBEEGap;}
            void setIsEBGap           (bool isEBGap)                     {m_isEBGap     = isEBGap;}
            void setIsEEGap           (bool isEEGap)                     {m_isEEGap     = isEEGap;}
            void setBdt               (double bdt)                       {m_bdt         = bdt;}
            void setPass              (bool pass)                        {m_pass        = pass;}
            void setSeedTTIeta        (int seedtt_ieta)                  {m_seedtt_ieta = seedtt_ieta;}
            void setSeedTTIphi        (int seedtt_iphi)                  {m_seedtt_iphi = seedtt_iphi;}
            void setListTTIeta        (const std::vector<int>& tt_ieta)  {m_tt_ieta     = tt_ieta;}
            void setListTTIphi        (const std::vector<int>& tt_iphi)  {m_tt_iphi     = tt_iphi;}
            void setListTTEnergy      (const std::vector<float>& tt_E)   {m_tt_E        = tt_E;}
            void setListRCTIeta       (const std::vector<int>& rct_ieta) {m_rct_ieta    = rct_ieta;}
            void setListRCTIphi       (const std::vector<int>& rct_iphi) {m_rct_iphi    = rct_iphi;}


        private:
            double              m_pt;
            double              m_et;
            double              m_E;
            double              m_eta;
            double              m_phi;
            int                 m_charge;
            double              m_sc_E;
            double              m_sc_eta;
            double              m_sc_phi;
            int                 m_ncl;
            int                 m_class;
            bool                m_isEB;
            bool                m_isEE;
            bool                m_isEBEEGap;
            bool                m_isEBGap;
            bool                m_isEEGap;
            double              m_bdt;
            bool                m_pass;
            int                 m_seedtt_ieta;
            int                 m_seedtt_iphi;
            std::vector<int>    m_tt_ieta;
            std::vector<int>    m_tt_iphi;
            std::vector<float>  m_tt_E;
            std::vector<int>    m_rct_ieta;
            std::vector<int>    m_rct_iphi;
    };
}


#endif

