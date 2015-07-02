
#include"control.h"

int TestSLHAFile(const char* filename, const char* stage, int ignoreConv) {
  if(CheckFileExistence(filename) == 0) return 0;

  string choice = stage;

  if(choice == "softsusy") {
    char buff[1000];
    string check = "# SOFTSUSY problem with point:";
    string special = "# SOFTSUSY problem with point: [ No convergence ]";

    ifstream fin(filename);
    stringstream line;
    while(!fin.eof()) {
      fin.getline(buff, 1000);
      line << buff;
      line.getline(buff, 31);
      if(buff == check) {
	if(ignoreConv == 0) return 0;
	else if(line.str() != special) return 0;
      }
      line.str("");
      line.seekp(0, ios_base::beg);
      line.clear();
    }
  }
  else if(choice == "micromegas") {
    string check;
    ifstream fin(filename);
    fin >> check;
    if(check == "micrOMEGAs_Failed!") return 0;
  }

  return 1;
}

/******************************************************************************/

Constraint::Constraint() : condition(NULL), bisection(NULL) {
  active = 0;
  precision = 1e-3;
  scaling = 1;
  low = 0;
  high = 0;
  upperBound = 0;
  lowerBound = 0;
  boundOptions = "none";
}

Constraint::~Constraint() {
  if(condition != NULL) delete condition;
  if(bisection != NULL) delete bisection;
}

void Constraint::SetUpperBound(int u) {
  upperBound = u;
  if(upperBound < lowerBound) {
    double temp = lowerBound;
    lowerBound = upperBound;
    upperBound = temp;
  }
}

void Constraint::SetLowerBound(int l) {
  lowerBound = l;
  if(upperBound < lowerBound) {
    double temp = lowerBound;
    lowerBound = upperBound;
    upperBound = temp;
  }
}

/******************************************************************************/

Control::Control() {
  LoadDefaults();
}

Control::Control(istream &in) : copyFiles(0), inputCommands(0),
				replaceCommands(0), constraintCommands(0) {
  Initialize(in);
}

Control::Control(const char *input) : copyFiles(0), inputCommands(0),
				      replaceCommands(0),
				      constraintCommands(0) {
  Initialize(input);
}

Control::~Control() {
  if(modelInfo != NULL) delete modelInfo;
  if(data != NULL) delete data;
  for(int i = 0; i < inputCommands.size(); i++) {
    delete inputCommands[i];
  }
  for(int j = 0; j < replaceCommands.size(); j++) {
    delete inputCommands[j];
  }
  for(int k = 0; k < constraintCommands.size(); k++) {
    delete constraintCommands[k];
  }
} 

void Control::Initialize(istream &in) {
  //Load Default Values
  this -> LoadDefaults();

  //Load Commands
  this -> LoadCommands(in);
}

void Control::Initialize(const char *input) {
  ifstream fin(input);
  if(fin.is_open()) Initialize(fin);
}


void Control::LoadCommands(istream &in) {
  char buff[1000];

  //Parse input stream for variable definitions.  Reads in input until
  //the end of the stream or a '!' character is encountered at the
  //start of a string.  Skips lines starting in '#'
  do {
    in.getline(buff, 1000);
    if(buff[0] == '#') continue;   //Skip lines starting in #
    else if(buff[0] == '!') break; // ! serves as a break character 
    else this -> Define(buff);     //Define function of inherited class
  }
  while(!in.eof());

  if(controlVerbosity > 0)
    cout << "Finished Loading Commands" << endl;

}

void Control::LoadCommands(const char *input) {
  ifstream fin(input);
  if(fin.is_open()) LoadCommands(fin);
}

void Control::LoadDefaults() {
  copyFiles.resize(0);
  inputCommands.resize(0);
  replaceCommands.resize(0);
  constraintCommands.resize(0);
  spectrumFlag = 0;
  micromegasFlag = 0;
  inputPrecision = 1e-8;
  scatter = 100;
  replace = 1;
  clean = 1;
  tmp_dir = "tmp_slha";
  extension = ".slha";
  outputFile = directory + "/" + tmp_dir + "/test_output";
  micromegasAlternate = "";
  bisectionIntermediates = "";
  intermediateNumber = 0;
  data = NULL;
  filenames.resize(0);
  ignoreConvergence = 0;
  randomTries = 100;
  modelInfo = NULL;
}

//Given a converter functions for the real Define functions
int Control::Define(const char *buff) {
  stringstream inputLine;
  inputLine << buff;
  return this -> Define(inputLine);
}

int Control::Define(const string buff) {
  stringstream inputLine;
  inputLine << buff;
  return this -> Define(inputLine);
}

int Control::Define(stringstream &inputLine) {
  string block;

  inputLine >> block;

  if(controlVerbosity > 1) {
    cout << "Full Define stream: " << inputLine.str() << "\n";
  }

  if(controlVerbosity > 0) {
    cout << "Control: Assigning Block: " << block << "\n";
  }

  if(block == "copy_from_file") {
    string file;
    inputLine >> file;
    copyFiles.push_back(file);
  }
  else if(block == "model") {
    string modelName;
    inputLine >> modelName;
    if(modelInfo != NULL) delete modelInfo;
    modelInfo = new Model(modelName.c_str());
  }
  else if(block == "reset_copy_list") copyFiles.resize(0);
  else if(block == "output_file") inputLine >> outputFile;
  else if(block == "replace_file") inputLine >> replace;
  else if(block == "tmp_dir") inputLine >> tmp_dir;
  else if(block == "input") {
    string blockName;
    inputLine >> blockName;
    inputCommands.push_back(new LHAItem(blockName, inputLine));
  }
  else if(block == "replace") {
    string blockName;
    inputLine >> blockName;
    replaceCommands.push_back(new LHAItem(blockName, inputLine));
  }
  else if(block == "constraint") {
    constraintCommands.push_back(new Constraint);
    stringstream settings, conditionInput;
    char buff[200];
    int scaling;
    double precision, low, high;
    string block;
    string boundOptions;

    inputLine.getline(buff, 200, '|');
    settings << buff;
    settings >> precision >> scaling >> low >> high >> boundOptions;
    constraintCommands.back() -> SetPrecision(precision);
    constraintCommands.back() -> SetScaling(scaling);
    constraintCommands.back() -> SetLow(low);
    constraintCommands.back() -> SetHigh(high);
    constraintCommands.back() -> SetBoundOptions(boundOptions);

    inputLine.getline(buff, 200, '|');
    conditionInput << buff;
    conditionInput >> block;
    constraintCommands.back() -> condition = new LHAItem(block, conditionInput);
    inputLine >> block;
    constraintCommands.back() -> bisection = new LHAItem(block, inputLine);
  }
  else if(block == "generate_spectrum") inputLine >> spectrumFlag;
  else if(block == "use_micromegas") inputLine >> micromegasFlag;
  else if(block == "input_precision") inputLine >> inputPrecision;
  else if(block == "ignore_convergence") inputLine >> ignoreConvergence;
  else if(block == "file_code") inputLine >> filecode;
  else if(block == "scatter") inputLine >> scatter;
  else if(block == "output_intermediates")
    inputLine >> bisectionIntermediates;
  else if(block == "clean_files") inputLine >> clean;
  else if(block == "verbosity") inputLine >> controlVerbosity;
  else if(block == "random_tries") inputLine >> randomTries;
  else {
    if(controlVerbosity > 0)
      cout << "Control: Invalid option: " << block << "\n";
    return 0;
  }

  return 1;
}

int Control::RunChain(int runFlag, LHAData *initialData) {
  if(CheckFileExistence(outputFile.c_str()) && replace == 0) return 0;

  filenames.resize(0);
  stringstream convert;
  int pass = 1;

  if(initialData != NULL) {
    if(initialData != data) {
      if(data != NULL) delete data;
      data = initialData;
    }
    for(int i = 0; i < constraintCommands.size(); i++) {
      data -> Modify(constraintCommands[i] -> bisection);
    }
  }
  else {
    if(data != NULL) delete data;
    data = new LHAData();
    for(int i = 0; i < copyFiles.size(); i++)
      data -> Read(copyFiles[i].c_str());

    for(int i = 0; i < inputCommands.size(); i++)
      data -> Modify(inputCommands[i]);
    for(int i = 0; i < constraintCommands.size(); i++)
      data -> Modify(constraintCommands[i] -> bisection);
  }

  if(controlDebug > 0 || pass == 0) {
    convert << directory << "/" << tmp_dir << "/" << filecode
	    << "_stage_"  << 0 << "_initial" << extension;
    filenames.push_back(convert.str());
    convert.str("");
 
    data -> Print(filenames[0].c_str(), modelInfo -> GetBlockList("input"));
  }

  if(spectrumFlag >= runFlag && pass) {
    if(modelInfo -> GetModelName() == "mssm" ||
       modelInfo -> GetModelName() == "mssm_new") pass = RunSoftsusy();
    else pass = RunSpectrum();
  }

  if(micromegasFlag >= runFlag && pass) {
    if(modelInfo -> GetModelName() == "nmssm") pass = RunNMSSMTools(runFlag);
    else pass = RunMicromegas(runFlag);
  }

  if(pass) return 1;
  else return 0;
}

int Control::SolveChain() {
  if(CheckFileExistence(outputFile.c_str()) && replace == 0) return 0;

  if(constraintCommands.size() == 0)
    return RunChain();

  int pass;
  double high, low, temp;
  RunChain(2);

  if(constraintCommands.size() > 0) {
    pass = RunBisection(constraintCommands.size() - 1);
    if(pass == 0) return 0;
  }

  return RunChain(1, data);
}

int Control::RunBisection(int index) {
  if(index < 0 || index > constraintCommands.size()) return 0;

  int scaling = constraintCommands[index] -> GetScaling(), pass;
  double diff, minPass, maxPass, low, high;

  pass = FindBoundaries(index, minPass, maxPass);

  if(pass == -1) return 1;
  else if(pass == 0) return 0;

  if(controlVerbosity >= 0) cout << "Level " << index + 1
				 << " -- Running Bisection" << endl;

  if(scaling != 0) {
    if(controlVerbosity > 0) cout << "Level " << index + 1 << " -- bottom:  ";
    constraintCommands[index] -> bisection -> SetValue(minPass);
    pass = Check(index, diff);
    if(pass > 0) {
      if(fabs(diff) < constraintCommands[index]-> GetPrecision())
	return 1;
      else if((scaling > 0 && diff < 0) || (scaling < 0 && diff > 0))
	low = minPass;
      else pass = 0;
    }

    if(pass == 0) {
      pass = FindEdge(index, low, minPass, maxPass, "lower");
      if(pass < 0) return 1;
      if(pass == 0) return 0;
    }

    if(controlVerbosity > 0) cout << "Level " << index + 1 << " -- top:  ";
    constraintCommands[index] -> bisection -> SetValue(maxPass);
    pass = Check(index, diff);
    if(pass > 0) {
      if(fabs(diff) < constraintCommands[index]-> GetPrecision())
	return 1;
      else if((scaling > 0 && diff < 0) || (scaling < 0 && diff > 0))
	high = maxPass;
      else pass = 0;
    }
    if(pass == 0) {
      pass = FindEdge(index, high, minPass, maxPass, "upper");
      if(pass < 0) return 1;
      if(pass == 0) return 0;
    }
  }
  else {
    if(controlVerbosity > 0) cout << "Level " << index + 1
				  << " -- Randomizing setup routine" << "\n";
    double above, below;
    double min = minPass;
    double max = maxPass;
    int aboveFlag = 0, belowFlag = 0;
    double current;
    //vector<int> filled(50, 0);
    //int fillIndex, filledMax;

    for(int i = 0; i < randomTries; i++) {
      current = double(rand()) / RAND_MAX;
      //fillIndex = current * 50;
      current = current * (max - min) + min;
      if(controlVerbosity > 0) cout << "Level " << index + 1 << " -- random: ";
      constraintCommands[index] -> bisection -> SetValue(current);
      pass = Check(index, diff);

      if(pass > 0) {
	//filled[fillIndex]++;
	if(fabs(diff) < constraintCommands[index]-> GetPrecision())
	  return 1;
	else if(diff < 0 && belowFlag == 0) {
	  low = current;
	  belowFlag = 1;
	}
	else if(diff > 0 && aboveFlag == 0) {
	  high = current;
	  aboveFlag = 1;
	}
      }

      if(aboveFlag == 1 && belowFlag == 1) {
	if(high < low) {
	  constraintCommands[index] -> SetScaling(-1);
	  current = low;
	  low = high;
	  high = current;
	}
	else constraintCommands[index] -> SetScaling(1);
	break;
      }
    }

    if(aboveFlag == 0 || belowFlag == 0) return 0;
  }

  double temp;
  pass = FindEdge(index, temp, low, high);
  if(scaling == 0) constraintCommands[index] -> SetScaling(0);
  if(controlVerbosity > 0) cout << "Level " << index + 1 << " -- Pass = "
				<< pass << "\n";
  if(pass == 0) return 0;
  else return 1;
}

int Control::Check(int index, double &diff) {
  if(index < 0 || index > constraintCommands.size()) return 0;

  if(controlVerbosity > 0)
    cout << constraintCommands[index] -> bisection -> GetValue() << endl;


  if(index > 0) {
    if(controlVerbosity > 0)
      cout << "Stepping down to level " << index << " bisection" << endl;

    int pass = RunBisection(index - 1);
    if(pass == 0) return 0;
  }

  diff = 1e30;
  int pass;
  double current, target;

  pass = RunChain(2, data);

  if(pass == 0) return 0;

  target = constraintCommands[index] -> condition -> GetValue();
  current = data -> GetValue(constraintCommands[index] -> condition);

  //Warning -- fails if target value is zero
  if(target == 0) {
    cout << "Target value in Check should never be zero" << "\n";
    return 0;
  }

  OutputIntermediate();

  diff = (current - target) / target;
  if(controlVerbosity > 0) cout << "Level " << index + 1
				<< " -- Diff: " << diff << endl;
  return 1;
}

int Control::FindBoundaries(int index, double &lower, double &upper) {
  if(controlVerbosity >= 0)
    cout << "Finding boundaries for bisection" << "\n";
  double min = constraintCommands[index] -> GetLow();
  double max = constraintCommands[index] -> GetHigh();
  double middleBreak;
  int aboveFlag = 0, belowFlag = 0, midFlag = 0;
  double current;
  int pass;
  double diff;

  if(controlVerbosity > 0) cout << "Level " << index + 1
				<< " -- minimum:  ";
  constraintCommands[index] -> bisection ->  SetValue(min);
  pass = Check(index, diff);
  if(pass > 0 && fabs(diff) < constraintCommands[index] -> GetPrecision())
    return -1;
  else if(pass > 0) {
    lower = min;
    belowFlag = 1;
  }

  if(controlVerbosity > 0) cout << "Level " << index + 1
				<< " -- maximum:  ";
  constraintCommands[index] -> bisection -> SetValue(max);
  pass = Check(index, diff);
  if(pass > 0 && fabs(diff) < constraintCommands[index] -> GetPrecision())
    return -1;
  else if(pass > 0) {
    upper = max;
    aboveFlag = 1;
  }

  if(aboveFlag == 1 && belowFlag == 1) return 1;

  for(int i = 0; i < 100; i++) {
    current = double(rand()) / RAND_MAX;
    current = current * (max - min) + min;
    if(controlVerbosity > 0) cout << "Level " << index + 1
				  << " -- check validity: ";
    constraintCommands[index] -> bisection -> SetValue(current);
    pass = Check(index, diff);

    if(pass > 0) {
      if(fabs(diff) < constraintCommands[index] -> GetPrecision()) return -1;
      else {
	middleBreak = current;
	midFlag = 1;
	break;
      }
    }
  }

  if(midFlag == 0) return 0;

  if(belowFlag == 0) {
    lower = middleBreak;
    while(fabs(lower - min) > inputPrecision) {
      current = (min + lower) / 2.;
      constraintCommands[index] -> bisection -> SetValue(current);
      if(controlVerbosity > 0) cout << "Level " << index + 1
				    << " -- finding minimum: ";
      pass = Check(index, diff);

      if(fabs(diff) < constraintCommands[index] -> GetPrecision()) return -1;
      else if(pass > 0) lower = current;
      else if(pass == 0) min = current;
    }
  }

  if(aboveFlag == 0) {
    upper = middleBreak;
    while(fabs(upper - max) > inputPrecision) {
      current = (max + upper) / 2.;
      constraintCommands[index] -> bisection -> SetValue(current);
      if(controlVerbosity > 0) cout << "Level " << index + 1
				    << " -- finding maximum: ";
      pass = Check(index, diff);

      if(fabs(diff) < constraintCommands[index] -> GetPrecision()) return -1;
      else if(pass > 0) upper = current;
      else if(pass == 0) max = current;
    }
  }

  return 1;
}

int Control::FindEdge(int index, double &edge, double low, double high,
		      const char *s) {
  if(index < 0 || index > constraintCommands.size()) return 0;
  if( fabs( (low - high) / high) < inputPrecision) return 0;

  string side = s;
  int scaling = constraintCommands[index] -> GetScaling(), pass;
  double average = (low + high) / 2., diff;
  constraintCommands[index] -> bisection -> SetValue(average);

  if(controlVerbosity > 0) cout << "Level " << index + 1 << " -- "
				<< s << ":  ";

  pass = Check(index, diff);

  if(pass == 0) {
    if(controlVerbosity > 0) cout << "Invalid" << "\n";
    if(side == "lower")
      return FindEdge(index, edge, average, high, s);
    else if(side == "upper")
      return FindEdge(index, edge, low, average, s);
    else {
      double random = rand();
      random /= RAND_MAX;
      if(random > 0.5) return FindEdge(index, edge, average, high, s);
      else return FindEdge(index, edge, low, average, s);
    }
  }
  else if(side == "lower" || side == "upper") {
    if(fabs(diff) < constraintCommands[index]-> GetPrecision()) {
      edge = average;
      return -1;
    }
    if(diff < 0) {
      if(scaling > 0 && side == "lower" || scaling < 0 && side == "upper") {
	edge = average;
	return 1;
      }
      else if(scaling > 0 && side == "upper") {
	return FindEdge(index, edge, average, high, s);
      }
      else if(scaling < 0 && side == "lower") {
	return FindEdge(index, edge, low, average, s);
      }
    }
    if(diff > 0) {
      if(scaling < 0 && side == "lower" || scaling > 0 && side == "upper") {
	edge = average;
	return 1;
      }
      else if(scaling < 0 && side == "upper") {
	return FindEdge(index, edge, average, high, s);
      }
      else if(scaling > 0 && side == "lower") {
	return FindEdge(index, edge, low, average, s);
      }
    }
  }
  else {
    if(fabs(diff) < constraintCommands[index]-> GetPrecision()) {
      edge = average;
      return -1;
    }
    if(diff < 0) {
      if(scaling > 0)
	return FindEdge(index, edge, average, high);
      else 
	return FindEdge(index, edge, low, average);
    }
    if(diff > 0) {
      if(scaling > 0)
	return FindEdge(index, edge, low, average);
      else 
	return FindEdge(index, edge, average, high);
    }
  }

  cout << "Something's wrong with logic ordering in FindEdge" << "\n";

  return 0;
}

int Control::RunSoftsusy() {
  stringstream convert, command;
  if(modelInfo == NULL || data == NULL) return 0;
  if(modelInfo -> GetModelName() != "mssm" &&
     modelInfo -> GetModelName() != "mssm_new") return 0;

  if(controlDebug <= 0) {
    convert << directory << "/" << tmp_dir << "/" << filecode
	    << "_stage_"  << 0 << "_initial" << extension;
    filenames.push_back(convert.str());
    convert.str("");
 
    data -> Print(filenames[0].c_str(), modelInfo -> GetBlockList("input"));
  }

  convert << directory << "/" << tmp_dir << "/" << filecode
	  << "_stage_" << filenames.size() << "_softsusy_base"
	  << extension;
  filenames.push_back(convert.str());
  convert.str("");

  if(controlDebug > 0) DeleteFile(filenames.back().c_str());

  command << modelInfo -> GetSpectrumExec() << " leshouches < "
	  << filenames[filenames.size() - 2] << " > "
	  << filenames.back();

  system(command.str().c_str());
  command.str("");


  if(TestSLHAFile(filenames.back().c_str(), "softsusy", ignoreConvergence)) {
    data -> Read(filenames.back().c_str());

    ifstream fin(filenames.back().c_str());
    string temp;

    do{
      fin >> temp;
    }
    while(!fin.eof() && temp !="Desired");

    if(!fin.eof()) {
      stringstream convert;
      char buff[200];
      for(int i = 0; i < 2; i++) {
	fin.getline(buff, 200, '=');
	fin >> temp;
	convert << i << "  " << temp;
	data -> Modify(new LHAItem(string("ACCURACY"),
				   convert.str().c_str()), 1);
	convert.str("");
      }
    }

    if(replaceCommands.size() > 0) {
      convert << directory << "/" << tmp_dir << "/" << filecode
	      << "_stage_" << filenames.size() << "_softsusy_modified"
	      << extension;
      filenames.push_back(convert.str());
      convert.str("");

      if(controlDebug > 0) DeleteFile(filenames.back().c_str());

      for(int i = 0; i < replaceCommands.size(); i++)
	data -> Modify(replaceCommands[i]);

      data -> Print(filenames.back().c_str(), 
		    modelInfo -> GetBlockList("spectrum"));
    }

    return 1;
  }
  else {
    return 0;
  }
}
 
int Control::RunSpectrum() {
  stringstream convert, command;
  if(modelInfo == NULL || data == NULL) return 0;
  if(modelInfo -> GetModelName() == "mssm" ||
     modelInfo -> GetModelName() == "mssm_new") return 0;

  stringstream codeConvert;

  LHAItem* temp;

  vector<string> itemNames(0);
  modelInfo -> GetInputVars(itemNames);
  vector<double> itemValues(0);

  for(int i = 0; i < itemNames.size(); i++) {
    temp = modelInfo -> GetItem(itemNames[i].c_str());
    if(temp != NULL) {
      itemValues.push_back(data -> GetValue(temp));
    }
    else return 0;
  }

  convert << directory << "/" << tmp_dir << "/" << filecode
	  << "_stage_" << filenames.size() << ".5_to_micromegas.in";
  micromegasAlternate = convert.str();
  convert.str("");

  if(controlDebug > 0) DeleteFile(micromegasAlternate.c_str());

  command << setprecision(10);

  command << modelInfo -> GetSpectrumExec() << " " << micromegasAlternate;
  for(int i = 0; i < itemValues.size(); i++) {
    command << " " << itemValues[i];
  }

  system(command.str().c_str());
  command.str("");

  if(CheckFileExistence(micromegasAlternate.c_str())) {
    ifstream fin(micromegasAlternate.c_str());
    string var;
    double val;

    fin >> var >> val;

    if(var == "Error:") return 0;

    while(!fin.eof()) {
      temp = modelInfo -> GetItem(var.c_str());
      if(temp != NULL) {
	temp -> SetValue(val);
	data -> Modify(temp);
      }
      fin >> var >> val;
    }

    fin.close();
    fin.clear();

    convert << directory << "/" << tmp_dir << "/" << filecode
	    << "_stage_" << filenames.size() << "_singletdoublet_params"
	    << extension;
    filenames.push_back(convert.str());
    convert.str("");

    if(controlDebug > 0) DeleteFile(filenames.back().c_str());

    ofstream fout(micromegasAlternate.c_str(), ios::app);

    for(int i = 0; i < replaceCommands.size(); i++) {
      data -> Modify(replaceCommands[i]);
      var = modelInfo -> GetVarName(replaceCommands[i]);
      val = replaceCommands[i] -> GetValue();
      if(var != "") fout << var << "     " << val << "\n";
    }

    if(controlDebug > 0) data -> Print(filenames.back().c_str());

    return 1;
  }
  else {
    return 0;
  }
}

int Control::RunMicromegas(int runFlag) {
  stringstream convert, command;
  if(filenames.size() < 1 && micromegasAlternate == "") return 0;
  if(modelInfo == NULL) return 0;

  string micromegasExec;

  if(runFlag >= 2) {
    micromegasExec = modelInfo -> GetMicromegasLoopExec();
    if(micromegasExec == "")
      micromegasExec = modelInfo -> GetMicromegasExec();
  }
  else micromegasExec = modelInfo -> GetMicromegasExec();


  command << micromegasExec << " ";

  if(micromegasAlternate == "")
    command << filenames.back();
  else
    command << micromegasAlternate;

  convert << directory << "/" << tmp_dir << "/" << filecode
	  << "_stage_" << filenames.size() << "_micromegas_addendum"
	  << extension;
  filenames.push_back(convert.str());
  convert.str("");

  if(controlDebug > 0) DeleteFile(filenames.back().c_str());

  command << " " << filenames.back();
  system(command.str().c_str());
  command.str("");

  if(TestSLHAFile(filenames.back().c_str(), "micromegas")) {
    data -> Read(filenames.back().c_str());

    if(controlDebug > 0) { 
      convert << directory << "/" << tmp_dir << "/" << filecode
	      << "_stage_" << filenames.size() << "_micromegas_full"
	      << extension;
      filenames.push_back(convert.str());
      convert.str("");

      DeleteFile(filenames.back().c_str());

      data -> Print(filenames.back().c_str(),
		    modelInfo -> GetBlockList("final"));
    }

    return 1;
  }
  else {
    return 0;
  }
}

int Control::RunNMSSMTools(int runFlag) {
  stringstream convert, command;
  if(filenames.size() < 1 && micromegasAlternate == "") return 0;
  if(modelInfo == NULL) return 0;

  string micromegasExec;

  if(runFlag >= 2) {
    micromegasExec = modelInfo -> GetMicromegasLoopExec();
    if(micromegasExec == "")
      micromegasExec = modelInfo -> GetMicromegasExec();
  }
  else micromegasExec = modelInfo -> GetMicromegasExec();


  command << micromegasExec << " ";

  if(micromegasAlternate == "")
    command << filenames.back();
  else
    command << micromegasAlternate;

  convert << directory << "/" << tmp_dir << "/" << filecode
	  << "_stage_" << filenames.size() << "_micromegas_addendum"
	  << extension;
  filenames.push_back(convert.str());
  convert.str("");

  if(controlDebug > 0) DeleteFile(filenames.back().c_str());

  command << " " << filenames.back();
  system(command.str().c_str());
  command.str("");

  if(TestSLHAFile(filenames.back().c_str(), "micromegas")) {
    data -> Read(filenames.back().c_str());

    if(controlDebug > 0) { 
      convert << directory << "/" << tmp_dir << "/" << filecode
	      << "_stage_" << filenames.size() << "_micromegas_full"
	      << extension;
      filenames.push_back(convert.str());
      convert.str("");

      DeleteFile(filenames.back().c_str());

      data -> Print(filenames.back().c_str(),
		    modelInfo -> GetBlockList("final"));
    }

    return 1;
  }
  else {
    return 0;
  }
}

//If the clean flag is set, delete all intermediate files 
void Control::CleanFiles() {
  stringstream command;

  if(controlVerbosity > 0)
    cout << "Clean flag: " << clean << endl;

  if(clean) {
    for(int i = 0; i < filenames.size(); i++) {
      DeleteFile(filenames[i].c_str());
    }
    DeleteFile(micromegasAlternate.c_str());
  }

  filenames.resize(0);
  micromegasAlternate = "";
}

//Save the finalized LHA file, defined as the last file in the
//filenames vector
void Control::SaveLHA() {

  //Basic sanity checks
  if(outputFile != "" && data != NULL) {

    //Don't overwrite an existing file if the replace flag is not set
    if(!(CheckFileExistence(outputFile.c_str()) && replace == 0))
      data -> Print(outputFile.c_str(), modelInfo -> GetBlockList("final"));
  }
}

void Control::OutputIntermediate() {

  if(bisectionIntermediates != "" && data != NULL) {

    stringstream filename;
    filename << bisectionIntermediates << "_" << intermediateNumber << ".lha";
    intermediateNumber++;

    //Don't overwrite an existing file if the replace flag is not set
    data -> Print(filename.str().c_str(), modelInfo -> GetBlockList("final"));
  }
}
