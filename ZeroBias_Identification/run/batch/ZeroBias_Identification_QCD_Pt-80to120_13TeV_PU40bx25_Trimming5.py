from AnhimaBatchLauncher import AnhimaBatchLauncher
import glob

inputBaseDir = "/home/llr/cms/sauvan/DATA/L1/Ntuples/Stage2Trees"
inputTemplate = "2015_02_20/tree_stage2_*.root"
datasets = []
datasets.append("QCD_Pt-80to120_Tune4C_13TeV_pythia8/Fall13dr-castor_tsg_PU40bx25_POSTLS162_V2-v1/GEN-SIM-RAW")


inputFiles =  []
for dataset in datasets:
    inputFiles.extend(glob.glob(inputBaseDir+"/"+dataset+"/"+inputTemplate))


batch = AnhimaBatchLauncher()
batch.exe = "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/ZeroBias_Identification/analysis.exe"
batch.name = "qcd_pt-80to120_identification_NuGun_13TeV_PU40bx25"
batch.inputFiles = inputFiles
batch.tree = "l1TriggerNtuplizer/l1TriggerTree"
batch.outputDirectory = "/data_CMS/cms/sauvan/L1/EmulatorEra/ZeroBias/identification/QCD_Pt-80to120_13TeV_PU40bx25/Trimming5/"
batch.outputFile = "zeroBias_identification_QCD_Pt-80to120_13TeV_PU40bx25_Trimming5.root"
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

#batch.local = True

#batch.cuts = [""]

batch.additionalParameters = {"EmulatorParameters": "/home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/data/caloStage2ParamsTrimming5.txt"}




