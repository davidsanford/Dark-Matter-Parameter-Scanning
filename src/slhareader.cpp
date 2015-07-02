#include"include/lha.h"
#include<time.h>

using namespace std;

int main(int argc, char* argv[]) {
  //srand(time(NULL));

  stringstream inFile, outFile;
  inFile << "tmp_slha/input_" << rand() << ".slha";
  outFile << "tmp_slha/softsusy_" << rand() << ".slha";

  LHAData *inputLHA = new LHAData();
  inputLHA -> Modify("MODSEL", "1", 1);
  inputLHA -> Modify("SMINPUTS", "1", 1.279340000e+02);
  inputLHA -> Modify("SMINPUTS", "2", 1.166370000e-05);
  inputLHA -> Modify("SMINPUTS", "3", 1.172000000e-01);
  inputLHA -> Modify("SMINPUTS", "4", 9.118760000e+01);
  inputLHA -> Modify("SMINPUTS", "5", 4.250000000e+00);
  inputLHA -> Modify("SMINPUTS", "6", 1.743000000e+02);
  inputLHA -> Modify("SMINPUTS", "7", 1.777000000e+00);
  inputLHA -> Modify("MINPAR", "1", 1000);
  inputLHA -> Modify("MINPAR", "2", 500);
  inputLHA -> Modify("MINPAR", "3", 10);
  inputLHA -> Modify("MINPAR", "4", 1);
  inputLHA -> Modify("MINPAR", "5", 0);
  inputLHA -> Modify("SOFTSUSY", "1", 1e-4);
  inputLHA -> Modify("SOFTSUSY", "2", 0);
  inputLHA -> Modify("SOFTSUSY", "5", 1);

  inputLHA -> Print(inFile.str().c_str());

  stringstream command;
  command << "../softsusy-3.3.6/softpoint.x leshouches < "
  	  << inFile.str() << " > " << outFile.str();

  system(command.str().c_str());

  //LHAData *intermediateLHA = new LHAData(outFile.str().c_str());

  //intermediateLHA -> Print("tmp_slha/testout.slha");

  inputLHA -> Read(outFile.str().c_str());

  inputLHA -> Print("tmp_slha/testout.slha");

  vector<string> *testing = new vector<string>(4);

  testing -> at(0) = "EXTPAR";
  testing -> at(1) = "MINPAR";
  testing -> at(2) = "MASS";
  testing -> at(3) = "nmix";

  inputLHA -> Print("tmp_slha/testout_abbreviated.slha", testing);

  return 1;
}
