from AnhimaBatchLauncher import AnhimaBatchLauncher
import glob
import os

baseDir = os.environ["ANHIMA_BASE"]
cmsswDir = os.environ["CMSSW_BASE"] 
wrapperDir = os.environ["EMUL_WRAPPER_BASE"]

inputBaseDir = "/home/llr/cms/sauvan/DATA/L1/Ntuples/Stage2Trees"
inputTemplate = "2015_07_09/tree_stage2_*.root"
datasets = []
datasets.append("Fall13dr-tsg_PU40bx25_POSTLS162_V2-v1/TT_Tune4C_13TeV-pythia8-tauola")


inputFiles =  []
for dataset in datasets:
    inputFiles.extend(glob.glob(inputBaseDir+"/"+dataset+"/"+inputTemplate))


batch = AnhimaBatchLauncher()
batch.exe = "{}/ZeroBias_Rate/analysis.exe".format(baseDir)
batch.name = "zeroBias_rate_TTbar_13TeV_PU40bx25"
batch.inputFiles = inputFiles
batch.tree = "l1TriggerNtuplizer/l1TriggerTree"
#batch.outputDirectory = "/data_CMS/cms/sauvan/L1/EmulatorEra/ZeroBias/rate/TTbar_13TeV_PU40bx25/Test9x3Filtering_Trimming5_No95Rescaling/"
#batch.outputFile = "zeroBias_rate_TTbar_13TeV_PU40bx25_Trimming5_No95Rescaling_Test9x3Filtering.root"
batch.outputDirectory = "/data_CMS/cms/sauvan/L1/EmulatorEra/ZeroBias/rate/TTbar_13TeV_PU40bx25/Trimming5_No95Rescaling/"
batch.outputFile = "zeroBias_rate_TTbar_13TeV_PU40bx25_Trimming5_No95Rescaling.root"
batch.histoParameters = "../histos.par"
batch.histoTag = "Histos"
batch.nFilesPerJob = 10
batch.queue = "llr"
batch.cmsswDir = cmsswDir
batch.additionalSetup = """## setup emulator wrapper
cd {0}
source wrapenv
cd -
ln -s {1}/TriggerCommon/ ../
""".format(wrapperDir, baseDir)


#batch.local = True

#batch.cuts = [""]

batch.additionalParameters = {"EmulatorParameters": "{}/data/caloStage2ParamsTrimming5.txt".format(wrapperDir),
                              "CutsHoEFile": "{}/TriggerCommon/data/cutsHoE_14.10.09.root".format(baseDir),
                              "CutsHoEHisto": "cutsEff05_eta_ET",
                              "ShapeDiscriminantFile": "{}/TriggerCommon/data/mergedDiscriminant_eta_et_shape.root".format(baseDir),
                              "ShapeDiscriminantHisto": "shapeDiscriminant2",
                              "ShapeWorkingPoint":"0.995",
                              "ApplyCorr95":"False",
                              "Corr95EtaStage2File":"{}/TriggerCommon/data/corr_vs_eta_Stage2_95pcEff.txt".format(baseDir),
                              "Corr95EtaStage1File":"{}/TriggerCommon/data/corr_vs_eta_Stage1_95pcEff_Emulator_1.5.txt".format(baseDir)
                             }



