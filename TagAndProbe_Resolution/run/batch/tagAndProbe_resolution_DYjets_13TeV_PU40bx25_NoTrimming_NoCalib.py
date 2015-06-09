from AnhimaBatchLauncher import AnhimaBatchLauncher
import glob

inputBaseDir = "/home/llr/cms/sauvan/DATA/sauvan/L1/Ntuples/EGammaStage1Stage2"
inputTemplate = "v_1_2014-07-29/tree_electron_stage1_stage2_*.root"
datasets = []
datasets.append("DYJetsToLL_M-50_13TeV-pythia6/Fall13dr-tsg_PU40bx25_POSTLS162_V2-v1")


inputFiles =  []
for dataset in datasets:
    inputFiles.extend(glob.glob(inputBaseDir+"/"+dataset+"/"+inputTemplate))


batch = AnhimaBatchLauncher()
batch.exe = "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Resolution/analysis.exe"
batch.name = "tagAndProbe_resolution_DYjets_13TeV_PU40bx25"
batch.inputFiles = inputFiles
batch.tree = "electronNtuplizer_electronTree"
batch.outputDirectory = "/data_CMS/cms/sauvan/L1/EmulatorEra/tagAndProbe/resolution/DYjets_13TeV_PU40bx25/NoTrimming/"
batch.outputFile = "tagAndProbe_resolution_DYjets_13TeV_PU40bx25_NoTrimming.root"
batch.histoParameters = "../histos.par"
batch.histoTag = "Histos"
batch.nFilesPerJob = 5
batch.queue = "cms"
batch.cmsswDir = "/home/llr/cms/sauvan/CMSSW/L1_Emulator/CMSSW_7_1_0_pre8/"
batch.additionalSetup = """## setup emulator wrapper
cd /home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/
source wrapenv
cd -
ln -s /home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TriggerCommon/ ../
"""

#batch.local = True

#batch.cuts = [""]

batch.additionalParameters = {"EmulatorParameters": "/home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/data/caloStage2ParamsNoTrimming_NoCalib.txt"}




