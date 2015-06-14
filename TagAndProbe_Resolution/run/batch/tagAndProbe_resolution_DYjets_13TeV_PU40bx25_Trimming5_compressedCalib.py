from AnhimaBatchLauncher import AnhimaBatchLauncher
import glob

inputBaseDir = "/data_CMS/cms/sauvan/L1/Ntuples/EGammaStage1Stage2"
#inputTemplate = "v_2_2014-09-23/tree_electron_stage1_stage2_*.root"
inputTemplate = "v_3_2014-10-09/tree_electron_stage1_stage2_*.root"
datasets = []
datasets.append("DYJetsToLL_M-50_13TeV-pythia6/Fall13dr-tsg_PU40bx25_POSTLS162_V2-v1")


inputFiles =  []
for dataset in datasets:
    inputFiles.extend(glob.glob(inputBaseDir+"/"+dataset+"/"+inputTemplate))


batch = AnhimaBatchLauncher()
batch.exe = "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Resolution/analysis.exe"
batch.name = "tagAndProbe_resolution_DYjets_13TeV_PU40bx25_Trimming5_CompressedCalib"
batch.inputFiles = inputFiles
batch.tree = "electronNtuplizer_electronTree"
batch.outputDirectory = "/data_CMS/cms/sauvan/L1/EmulatorEra/tagAndProbe/resolution/DYjets_13TeV_PU40bx25/CompressedCalib/Trimming5/"
batch.outputFile = "tagAndProbe_resolution_DYjets_13TeV_PU40bx25_Trimming5_CompressedCalib.root"
batch.histoParameters = "../histos.par"
batch.histoTag = "Histos"
batch.nFilesPerJob = 2
batch.queue = "cms"
batch.cmsswDir = "/home/llr/cms/sauvan/CMSSW/L1_Emulator/CMSSW_7_5_0_pre4/"
batch.additionalSetup = """## setup emulator wrapper
cd /home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/
source wrapenv
cd -
ln -s /home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TriggerCommon/ ../
"""

#batch.local = True

#batch.cuts = [""]

batch.additionalParameters = {"EmulatorParameters": "/home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/data/caloStage2ParamsTrimming5_compressedCalib.txt",
                              "CutsHoEFile": "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Identification/data/cutsHoE_14.10.09.root",
                              "CutsHoEHisto": "cutsEff05_eta_ET",
                              "CutsShapeFile": "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Identification/data/cutsShape_14.10.09.root",
                              "CutsShapeHisto": "cutsEff05_eta_ET",
                              "ApplyCorr95":"False",
                              "Corr95EtaStage2File":"/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Resolution/data/corr_vs_eta_Stage2_95pcEff.txt",
                              "Corr95EtaStage1File":"/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Resolution/data/corr_vs_eta_Stage1_95pcEff.txt"
                             }



