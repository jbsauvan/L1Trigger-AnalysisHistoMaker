from AnhimaBatchLauncher import AnhimaBatchLauncher
import glob

#inputBaseDir = "/home/llr/cms/sauvan/DATA/L1/Ntuples/EGammaStage1Stage2"
#inputTemplate = "v_3_2014-10-09/tree_electron_stage1_stage2_*.root"
#datasets = []
#datasets.append("DYJetsToLL_M-50_13TeV-pythia6/Fall13dr-tsg_PU40bx25_POSTLS162_V2-v1")

inputBaseDir = "/home/llr/cms/sauvan/DATA/L1/Ntuples/EGammaStage1Stage2"
inputTemplate = "v_1_2014-07-29/tree_electron_stage1_stage2_*.root"
datasets = []
datasets.append("DYJetsToLL_M-50_13TeV-pythia6/Fall13dr-tsg_PU40bx25_POSTLS162_V2-v1")


inputFiles =  []
for dataset in datasets:
    inputFiles.extend(glob.glob(inputBaseDir+"/"+dataset+"/"+inputTemplate))


batch = AnhimaBatchLauncher()
batch.exe = "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_TurnOn/analysis.exe"
batch.name = "tagAndProbe_turnOn_DYjets_13TeV_PU40bx25"
batch.inputFiles = inputFiles
batch.tree = "electronNtuplizer_electronTree"
batch.outputDirectory = "/data_CMS/cms/sauvan/L1/EmulatorEra/tagAndProbe/turnOn/DYjets_13TeV_PU40bx25/"
batch.outputFile = "tagAndProbe_turnOn_DYjets_13TeV_PU40bx25.root"
batch.histoParameters = "../histos.par"
batch.histoTag = "Histos"
batch.nFilesPerJob = 2
batch.queue = "cms"
batch.cmsswDir = "/home/llr/cms/sauvan/CMSSW/L1_Emulator/CMSSW_7_3_0_pre1/"
batch.additionalSetup = """## setup emulator wrapper
cd /home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/
source wrapenv
cd -
ln -s /home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TriggerCommon/ ../
"""

#batch.cuts = [""]

batch.additionalParameters = {"EmulatorParameters": "/home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/data/caloStage2ParamsTrimming5.txt",
                              "CutsHoEFile": "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Identification/data/cutsHoE_14.10.09.root",
                              "CutsHoEHisto": "cutsEff05_eta_ET",
                              "ShapeDiscriminantFile": "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Identification/data/mergedDiscriminant_eta_et_shape.root",
                              "ShapeDiscriminantHisto": "shapeDiscriminant2",
                              "ShapeWorkingPoint":"0.995",
                              "ApplyCorr95":"False",
                              "Corr95EtaStage2File":"/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Resolution/data/corr_vs_eta_Stage2_95pcEff.txt",
                              "Corr95EtaStage1File":"/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Resolution/data/corr_vs_eta_Stage1_95pcEff_Emulator_1.5.txt"
                             }



