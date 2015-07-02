#ifndef CONTROL_H
#define CONTROL_H

#include<fstream>
#include<iostream>
#include<iomanip>
#include<string>
#include<vector>
#include<sstream>
#include"lha.h"
#include"model.h"
#include<math.h>

using namespace std;

int TestSLHAFile(const char* filename, const char* stage, int ignoreConv = 0);

/******************************************************************************/

class Constraint {
 public:
  Constraint();

  ~Constraint();

  int GetActivity() {return active;}
  double GetPrecision() {return precision;}
  int GetScaling() {return scaling;}
  double GetLow() {return low;}
  double GetHigh() {return high;}
  int GetUpperBound() {return upperBound;}
  int GetLowerBound() {return lowerBound;}
  string GetBoundOptions() {return boundOptions;}

  void SetActivity(int a) {active = a;}
  void SetPrecision(double p) {precision = p;}
  void SetScaling(int s) {scaling = s;}
  void SetLow(double l) {low = l;}
  void SetHigh(double h) {high = h;}
  void SetUpperBound(int u);
  void SetLowerBound(int l);
  void SetBoundOptions(string s) {boundOptions = s;}

  LHAItem *condition;
  LHAItem *bisection;

 protected:
  int active, upperBound, lowerBound;
  double precision;
  int scaling;
  double low;
  double high;
  string boundOptions;
};

/******************************************************************************/

//Control class
//Manages LHA files, runs the spectrum generator, and runs micromegas
class Control {
 public:
  //Constructor takes as input an initialization stream, with format
  //detailed in function definition.
  Control();
  Control(istream &);
  Control(const char *);

  //Deconstructor
  ~Control();

  //Function to assign commands
  void Initialize(istream &);
  void Initialize(const char *);

  //Get default values for stored data
  virtual void LoadDefaults();

  //Load commands from a file (w/o reseting current commands)
  virtual void LoadCommands(istream &);
  virtual void LoadCommands(const char *);

  //Redefine variables in the class - returns 1 if variable was found
  //and 0 if now.  Handles definitions for constructor as well.
  virtual int Define(const char *buff);
  virtual int Define(const string buff);

  //Run one pass through the chain.  Returns 1 if successful
  virtual int RunChain(int runFlag = 1, LHAData *initialData = NULL);
  virtual int SolveChain();
  virtual int RunSoftsusy();
  //virtual int RunSoftsusyNew();
  virtual int RunSpectrum();
  virtual int RunMicromegas(int runFlag = 1);
  virtual int RunNMSSMTools(int runFlag = 1);

  //Delete all temporary files
  virtual void CleanFiles();

  //Save to specified file
  virtual void SaveLHA();

  //Access functions
  int GetCode() {return filecode;}

  //Verbosity variable
  static int controlVerbosity;
  static int controlDebug;

 protected:
  //int RunBisection(int, double, double);
  int RunBisection(int);
  int FindBoundaries(int, double &, double &);
  int FindEdge(int, double &, double, double, const char *s = "none");
  int Check(int, double &);
  void OutputIntermediate();

  //Common "true" define function with virtualization.
  virtual int Define(stringstream &buff);

  //Various save data
  vector<string> copyFiles;
  int spectrumFlag, micromegasFlag;
  string extension, micromegasAlternate;
  double inputPrecision;
  long int filecode;
  string outputFile;
  string tmp_dir;
  string bisectionIntermediates;
  int replace;
  int clean;
  int scatter;
  int ignoreConvergence;
  int intermediateNumber;
  int randomTries;

  //Model info
  Model *modelInfo;

  //Executions vectors
  vector<string> filenames;
  LHAData *data;

  //Various Command Vectors
  vector<LHAItem*> inputCommands, replaceCommands;
  vector<Constraint*> constraintCommands;
};

/******************************************************************************/

#endif
