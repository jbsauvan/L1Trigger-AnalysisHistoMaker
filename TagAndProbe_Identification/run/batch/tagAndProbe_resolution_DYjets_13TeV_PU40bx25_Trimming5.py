from AnhimaBatchLauncher import AnhimaBatchLauncher
import glob

inputBaseDir = "/home/llr/cms/sauvan/DATA/L1/Ntuples/EGammaStage1Stage2"
inputTemplate = "v_1_2014-07-29/tree_electron_stage1_stage2_*.root"
datasets = []
datasets.append("DYJetsToLL_M-50_13TeV-pythia6/Fall13dr-tsg_PU40bx25_POSTLS162_V2-v1")


inputFiles =  []
for dataset in datasets:
    inputFiles.extend(glob.glob(inputBaseDir+"/"+dataset+"/"+inputTemplate))


batch = AnhimaBatchLauncher()
batch.exe = "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Identification/analysis.exe"
batch.name = "tagAndProbe_identification_DYjets_13TeV_PU40bx25_Trimming5"
batch.inputFiles = inputFiles
batch.tree = "electronNtuplizer_electronTree"
batch.outputDirectory = "/data_CMS/cms/sauvan/L1/EmulatorEra/tagAndProbe/identification/DYjets_13TeV_PU40bx25/Trimming5/"
batch.outputFile = "tagAndProbe_identification_DYjets_13TeV_PU40bx25_Trimming5.root"
batch.histoParameters = "../histos.par"
batch.histoTag = "Histos"
batch.nFilesPerJob = 1
batch.queue = "cms"
batch.cmsswDir = "/home/llr/cms/sauvan/CMSSW/L1_Emulator/CMSSW_7_3_0_pre1/"
batch.additionalSetup = """## setup emulator wrapper
cd /home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/
source wrapenv
cd -
ln -s /home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TriggerCommon/ ../
"""

#batch.cuts = [""]

batch.additionalParameters = {"EmulatorParameters": "/home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/data/caloStage2ParamsTrimming5.txt"}



