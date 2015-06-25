#makefile 


CC   =   g++

#UCFLAGS = -O0 -g3 -Wall -gstabs+  
UCFLAGS = -O3 -Wall -gstabs+ -std=c++0x 


RUCFLAGS := $(shell root-config --cflags) -I./include/ -I../Core/include/ -I../TriggerCommon/include/ -I${EMUL_WRAPPER_BASE}/include/ -I${CMSSW_BASE}/src/ -I${CMSSW_RELEASE_BASE}/src/ 
LIBS :=  $(shell root-config --libs) -lTreePlayer -lGenVector -L${EMUL_WRAPPER_BASE}/lib -lL1TEmulatorWrapper -L${CMSSW_BASE}/lib/${SCRAM_ARCH} -L${CMSSW_RELEASE_BASE}/lib/${SCRAM_ARCH} -lTreePlayer -lDataFormatsL1TCalorimeter -lDataFormatsL1Trigger -lDataFormatsCandidate -lCondFormatsL1TObjects -lL1TriggerL1TCalorimeter  
GLIBS := $(shell root-config --glibs)

VPATH = ../Core/src:../TriggerCommon/src/:./src/

SRCPP = main.cpp\
	HistoManager.cpp\
	IAnalysis.cpp\
	IEvent.cpp\
	ParReader.cpp\
	Utilities.cpp\
	Electron.cpp\
	GenParticle.cpp\
	GenJet.cpp\
	ElectronFactory.cpp\
	GenElectronFactory.cpp\
	GenJetFactory.cpp\
	L1TowerFactory.cpp\
	L1EGammaFactory.cpp\
	TriggerTools.cpp\
	EventEGamma.cpp\
	EventTagAndProbe.cpp\
	EventTagAndProbeWithJets.cpp\
	Analysis.cpp\
	Event.cpp


	
         
#OBJCPP = $(SRCPP:.cpp=.o)
OBJCPP = $(patsubst %.cpp,obj/%.o,$(SRCPP))


all : analysis.exe ../TriggerCommon/lib/libDictionary_C.so

obj/%.o : %.cpp
	@mkdir -p obj/
	@echo compiling $*
	@$(CC) -c $< $(UCFLAGS) $(RUCFLAGS) -o $@

analysis.exe : $(OBJCPP) 
	@echo linking
	@$(CC) $^ $(ACLIBS) $(LIBS) $(GLIBS)  -o $@

clean:
	@rm  -f obj/*.o
	@rm -f analysis.exe

../TriggerCommon/lib/libDictionary_C.so: ../TriggerCommon/include/libDictionary.C
	@echo "> Generating dictionary"
	@cd ../TriggerCommon/include && root -b -q libDictionary.C++
	@mkdir -p ../TriggerCommon/lib/
	@mv ../TriggerCommon/include/libDictionary_C.so ../TriggerCommon/lib/
