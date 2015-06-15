from AnhimaBatchLauncher import AnhimaBatchLauncher
import glob
import os

baseDir = os.environ["ANHIMA_BASE"]
cmsswDir = os.environ["CMSSW_BASE"] 
wrapperDir = os.environ["EMUL_WRAPPER_BASE"]

inputBaseDir = "/data_CMS/cms/sauvan/L1/Ntuples/EGammaStage1Stage2"
#inputTemplate = "v_2_2014-09-23/tree_electron_stage1_stage2_*.root"
inputTemplate = "v_3_2014-10-09/tree_electron_stage1_stage2_*.root"
datasets = []
datasets.append("DYJetsToLL_M-50_13TeV-pythia6/Fall13dr-tsg_PU40bx25_POSTLS162_V2-v1")


inputFiles =  []
for dataset in datasets:
    inputFiles.extend(glob.glob(inputBaseDir+"/"+dataset+"/"+inputTemplate))


batch = AnhimaBatchLauncher()
batch.exe = "{}/TagAndProbe_Resolution/analysis.exe".format(baseDir)
batch.name = "tagAndProbe_resolution_DYjets_13TeV_PU40bx25_Trimming5_CompressedCalib"
batch.inputFiles = inputFiles
batch.tree = "electronNtuplizer_electronTree"
batch.outputDirectory = "/data_CMS/cms/sauvan/L1/EmulatorEra/tagAndProbe/resolution/DYjets_13TeV_PU40bx25/CompressedCalib/Trimming5/"
batch.outputFile = "tagAndProbe_resolution_DYjets_13TeV_PU40bx25_Trimming5_CompressedCalib.root"
batch.histoParameters = "../histos.par"
batch.histoTag = "Histos"
batch.nFilesPerJob = 2
batch.queue = "cms"
batch.cmsswDir = cmsswDir
batch.additionalSetup = """## setup emulator wrapper
cd {0}
source wrapenv
cd -
ln -s {1}/TriggerCommon/ ../
""".format(wrapperDir, baseDir)

#batch.local = True

#batch.cuts = [""]

batch.additionalParameters = {"EmulatorParameters": "{}/data/caloStage2ParamsTrimming5_compressedCalib.txt".format(wrapperDir),
                              "CutsHoEFile": "{}/TriggerCommon/data/cutsHoE_14.10.09.root".format(baseDir),
                              "CutsHoEHisto": "cutsEff05_eta_ET",
                              "CutsShapeFile": "{}/TriggerCommon/data/cutsShape_14.10.09.root".format(baseDir),
                              "CutsShapeHisto": "cutsEff05_eta_ET",
                              "ApplyCorr95":"False",
                              "Corr95EtaStage2File":"{}/TriggerCommon/data/corr_vs_eta_Stage2_95pcEff.txt".format(baseDir),
                              "Corr95EtaStage1File":"{}/TriggerCommon/data/corr_vs_eta_Stage1_95pcEff.txt".format(baseDir)
                             }
