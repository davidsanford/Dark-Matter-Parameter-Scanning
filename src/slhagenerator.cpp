#include"lha.h"
#include"model.h"
#include"control.h"
#include<time.h>

using namespace std;

int Control::controlVerbosity = -1;
int Control::controlDebug = 0;
string directory = "/common/dsanford/SUSYSpectra/SpectraGeneration";

int main(int argc, char* argv[]) {
  if(argc < 2) {
    cout << "Error :  Need at least one control file" << endl;
  }

  srand(time(NULL));
  long int code = rand();
  stringstream convert;
  Control *SLHAManager = new Control();
  string outFile;
  int pass;

  /* Begin : Handle control files */
  convert << "file_code " << rand();
  SLHAManager -> Define(convert.str());
  for(int i = 1; i < argc; i++) SLHAManager -> LoadCommands(argv[i]);
  /* End : Handle control files */

  pass = SLHAManager -> SolveChain();

  if(pass == 0) {
    cout << "Failed!" << endl
	 << "Code: " << SLHAManager -> GetCode() << endl;
  }
  else {
    if(Control::controlDebug > 0) cout << "Succeeded" << endl;

    SLHAManager -> SaveLHA();
    SLHAManager -> CleanFiles();
  }

  return 1;
}
