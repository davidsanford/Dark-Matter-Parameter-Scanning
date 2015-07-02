DIR=/common/dsanford/SUSYSpectra/SpectraGeneration
INCLUDE=include
SOURCE=src
OBJ=obj
INC=-I$(DIR)/include

all :  slhagenerator slhareader randominput pifraction yfraction \
xtcalc random_prefixed nmssm_block testcopy combine nmssm_constrained \
nmssm_combine nmssmtools_input nomixing_input nmssm_scan split \
binolist nmssm_scan_bino omegafraction_mupos omegafraction_muneg \
omegafraction_wino_mupos omegafraction_wino_muneg ythetafraction

pifraction : $(SOURCE)/pifraction.cpp
	g++ -o $@ $^

xtcalc : $(SOURCE)/xtcalc.cpp
	g++ -o $@ $^

yfraction : $(SOURCE)/yfraction.cpp
	g++ -o $@ $^

ythetafraction : $(SOURCE)/ythetafraction.cpp
	g++ -o $@ $^

randominput : $(SOURCE)/randominput.cpp
	g++ -o $@ $^

random_prefixed : $(SOURCE)/random_prefixed.cpp
	g++ -o $@ $^

nmssm_block : $(SOURCE)/nmssm_block.cpp
	g++ -o $@ $^

nmssm_constrained : $(SOURCE)/nmssm_constrained.cpp
	g++ -o $@ $^

nmssm_combine : $(SOURCE)/nmssm_combine.cpp
	g++ -o $@ $^

testcopy : $(SOURCE)/testcopy.cpp
	g++ -o $@ $^

combine : $(SOURCE)/combine.cpp
	g++ -o $@ $^

nmssmtools_input : $(SOURCE)/nmssmtools_input.cpp
	g++ -o $@ $^

nomixing_input : $(SOURCE)/nomixing_input.cpp
	g++ -o $@ $^

nmssm_scan : $(SOURCE)/nmssm_scan.cpp
	g++ -o $@ $^

nmssm_scan_bino : $(SOURCE)/nmssm_scan_bino.cpp
	g++ -o $@ $^

split : $(SOURCE)/split.cpp
	g++ -o $@ $^

binolist : $(SOURCE)/binolist.cpp
	g++ -o $@ $^

omegafraction_mupos : $(SOURCE)/omegafraction_mupos.cpp
	g++ -o $@ $^

omegafraction_muneg : $(SOURCE)/omegafraction_muneg.cpp
	g++ -o $@ $^

omegafraction_wino_mupos : $(SOURCE)/omegafraction_wino_mupos.cpp
	g++ -o $@ $^

omegafraction_wino_muneg : $(SOURCE)/omegafraction_wino_muneg.cpp
	g++ -o $@ $^

slhareader : $(SOURCE)/slhareader.cpp $(OBJ)/lha.o
	g++ -o $@ $^ $(INC)

slhagenerator: $(SOURCE)/slhagenerator.cpp $(OBJ)/lha.o \
$(OBJ)/control.o $(OBJ)/model.o
	g++ -o $@ $^ $(INC)

$(OBJ)/lha.o : $(SOURCE)/lha.cpp $(INCLUDE)/lha.h
	g++ -c -o $@ $< $(INC)

$(OBJ)/model.o : $(SOURCE)/model.cpp $(INCLUDE)/model.h $(INCLUDE)/lha.h
	g++ -c -o $@ $< $(INC)

$(OBJ)/control.o : $(SOURCE)/control.cpp $(INCLUDE)/control.h \
$(INCLUDE)/lha.h $(INCLUDE)/model.h
	g++ -c -o $@ $< $(INC)

clean:
	rm *~ tmp_slha/* generate_spectrum */*~ */*/*~ */*/*/*~ $(OBJ)/*.o;
