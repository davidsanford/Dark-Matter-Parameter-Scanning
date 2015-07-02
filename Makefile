DIR=/common/dsanford/SUSYSpectra/SpectraGeneration
INCLUDE=include
SOURCE=src
OBJ=obj
INC=-I$(DIR)/include

all :  slhagenerator slhareader

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
	rm *~ tmp_slha/* */*~ */*/*~ */*/*/*~ $(OBJ)/*.o;
