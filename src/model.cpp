#include"model.h"

int CheckFileExistence(const char* filename) {
  struct stat buffer;
  if(stat(filename, &buffer) == 0) return 1;
  return 0;
}

int DeleteFile(const char* filename) {
  if(CheckFileExistence(filename) == 0) return 0;

  stringstream command;
  command << "rm " << filename;
  system(command.str().c_str());

  return 1;
}


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/


Model::Model() : varNames(0), varItems(0), blockLists(0),
		 modelName("") {}

Model::Model(const char *model)
  : varNames(0), varItems(0), modelName(model), blockLists(0),  
    spectrumExec(""), micromegasExec(""), micromegasLoopExec("") {

  stringstream convert;
  convert << directory << "/models/" << modelName << "/index.in";
  string filetype, filename;

  ifstream fin(convert.str().c_str());

  if(!fin.is_open()) return;

  while(!fin.eof()) {
    fin >> filetype >> filename;

    if(filetype == "spectrum_executable") spectrumExec = filename;
    else if(filetype == "micromegas_executable") micromegasExec = filename;
    else if(filetype == "micromegas_loop_exec") micromegasLoopExec = filename;
    else if(filetype == "variables") AddVariables(filename.c_str());
    else if(filetype == "blocklist") AddBlockList(filename.c_str());
  }
}

Model::~Model() {
  for(int i = 0; i < varItems.size(); i++)
    delete varItems[i];
  for(int j = 0; j < varItems.size(); j++)
    delete blockLists[j];
}

void Model::AddVariables(const char *name) {
  stringstream convert;
  convert << directory << "/models/"
	  << modelName << "/" << name << ".in";
  ifstream fin(convert.str().c_str());
  string var, block;
  char buff[200];

  if(!fin.is_open()) return;

  while(!fin.eof()) {
    fin >> var;
    fin >> block;
    fin.getline(buff, 200);
    varNames.push_back(var);
    varItems.push_back(new LHAItem(block, buff));
  }
}

void Model::AddBlockList(const char *name) {
  stringstream convert;
  convert << directory << "/models/"
	  << modelName << "/" << name << ".in";
  ifstream fin(convert.str().c_str());
  string block;

  if(!fin.is_open()) return;

  int index;
  for(index = 0; index < blockLists.size(); index++) {
    if(name == blockLists[index] -> at(0)) {
      blockLists[index] -> resize(0);
      blockLists[index] -> push_back(name);
      break;
    }
  }
  if(index == blockLists.size()) {
    blockLists.push_back(new vector<string>(0));
    blockLists[index] -> push_back(name);
  }

  fin >> block;

  while(!fin.eof()) {
    blockLists[index] -> push_back(block);
    fin >> block;
  }
}

LHAItem* Model::GetItem(const char *name) {
  for(int i = 0; i < varNames.size(); i++) {
    if(varNames[i] == name) {
      LHAItem *item = new LHAItem(*(varItems[i]));
      return item;
    }
  }

  return NULL;
}

string Model::GetVarName(LHAItem *item) {
  for(int i = 0; i < varNames.size(); i++) {
    if(*item == *(varItems[i])) {
      return varNames[i];
    }
  }

  return "";
}

vector<string>* Model::GetBlockList(const char *label) {
  for(int i = 0; i < blockLists.size(); i++) {
    if(blockLists[i] != NULL && blockLists[i] -> size() > 0
       && blockLists[i] -> at(0) == label)
      return blockLists[i];
  }

  return NULL;
}

void Model::GetInputVars(vector<string> &inputVars) {
  inputVars.resize(0);

  for(int i = 0; i < varNames.size(); i++) {
    if(varItems[i] -> GetBlock() == "EXTPAR")
      inputVars.push_back(varNames[i]);
  }
}
