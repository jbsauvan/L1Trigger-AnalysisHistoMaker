from AnhimaBatchLauncher import AnhimaBatchLauncher
import glob

inputBaseDir = "/home/llr/cms/sauvan/DATA/L1/Ntuples/EGammaStage2"
inputTemplate = "v_4_2014-10-10/tree_electron_stage2_*_0.root"
datasets = []
datasets.append("Neutrino_Pt-2to20_gun/Fall13dr-tsg_PU40bx25_POSTLS162_V2-v1")


inputFiles =  []
for dataset in datasets:
    inputFiles.extend(glob.glob(inputBaseDir+"/"+dataset+"/"+inputTemplate))


batch = AnhimaBatchLauncher()
batch.exe = "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/ZeroBias_Rate/analysis.exe"
batch.name = "zeroBias_rate_NuGun_13TeV_PU40bx25"
batch.inputFiles = inputFiles
batch.tree = "electronNtuplizer_electronTree"
batch.outputDirectory = "/data_CMS/cms/sauvan/L1/EmulatorEra/ZeroBias/rate/NuGun_13TeV_PU40bx25/Trimming5/"
batch.outputFile = "zeroBias_rate_NuGun_13TeV_PU40bx25_Trimming5.root"
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

#batch.local = True

#batch.cuts = [""]

batch.additionalParameters = {"EmulatorParameters": "/home/llr/cms/sauvan/L1_upgrade/L1TEmulatorWrapper/data/caloStage2ParamsTrimming5.txt",
                              "CutsHoEFile": "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Identification/data/cutsHoE_14.10.09.root",
                              "CutsHoEHisto": "cutsEff05_eta_ET",
                              "ShapeDiscriminantFile": "/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Identification/data/mergedDiscriminant_eta_et_shape.root",
                              "ShapeDiscriminantHisto": "shapeDiscriminant2",
                              "ShapeWorkingPoint":"0.9",
                              "ApplyCorr95":"True",
                              "Corr95EtaStage2File":"/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Resolution/data/corr_vs_eta_Stage2_95pcEff.txt",
                              "Corr95EtaStage1File":"/home/llr/cms/sauvan/L1_upgrade/AnHiMa2/TagAndProbe_Resolution/data/corr_vs_eta_Stage1_95pcEff_Emulator_1.5.txt"
                             }




