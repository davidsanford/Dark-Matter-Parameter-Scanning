#ifndef MODEL_H
#define MODEL_H

#include<sys/stat.h>
#include"lha.h"

using namespace std;

extern string directory;
extern string tmp_directory;
int CheckFileExistence(const char* filename);
int DeleteFile(const char* filename);

/******************************************************************************/

class Model {
 public:
  Model();
  Model(const char *);

  ~Model();

  void SetModelName(const char *m) {modelName = m;}
  void AddVariables(const char *name);
  void AddBlockList(const char *name);

  string GetModelName() {return modelName;}
  string GetSpectrumExec() {return spectrumExec;}
  string GetMicromegasExec() {return micromegasExec;}
  string GetMicromegasLoopExec() {return micromegasLoopExec;}
  LHAItem* GetItem(const char *name);
  string GetVarName(LHAItem *item);
  vector<string>* GetBlockList(const char *label);
  void GetInputVars(vector<string> &);

 protected:
  string modelName, spectrumExec, micromegasExec, micromegasLoopExec;
  vector<string> varNames;
  vector<LHAItem *> varItems;
  vector<vector<string> *> blockLists;
};

/******************************************************************************/

#endif
