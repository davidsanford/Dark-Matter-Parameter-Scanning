#ifndef LHA_h
#define LHA_h

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include<vector>
#include<sstream>
#include<iomanip>

using namespace std;

//Conversion functions between strings and numbers
inline string DoubleToString(double d);
inline double StringToDouble(string s);
inline double StringToInt(int i);

/******************************************************************************/

//LHAItem class
//Contains the block name, codes, value, and comments, stored as strings
class LHAItem {
 public:
  //Class constructor
  //All except default call the appropriate version of the AssignLHA
  LHAItem();
  LHAItem(string b, stringstream &in);
  LHAItem(string b, const char *l);
  LHAItem(string b, string c, double v = 0);
  LHAItem(string b, vector<string> &c, double v = 0);
  LHAItem(LHAItem &item) {*this = item;}

  //Deconstructor
  ~LHAItem() {}

  // << operator
  friend ostream & operator<<(ostream &out, LHAItem &item) {
    item.Print(out);
  }

  //AssignLHA functions
  //All redefine the contents of the LHAItem class, using various inputs
  virtual void AssignLHA(string b, stringstream &in);
  virtual void AssignLHA(string b, const char *l);
  virtual void AssignLHA(string b, string c, double v = 0,
			  string com = "");
  virtual void AssignLHA(string b, vector<string> &c, double v = 0,
			  string com = "");

  //Return class data
  string GetBlock() {return block;}
  string GetCodes() {return codes;}
  double GetValue();
  string GetComment() {return comment;}

  //Define class data
  void SetBlock(const char* b) {block = b;}
  void SetCodes(const char* c) {codes = c;}
  void SetCodes(vector<string> &c);
  void SetComment(const char* c) {comment = c;}
  void SetValue(string v) {value = v;}
  void SetValue(double v);

  LHAItem & operator=(LHAItem &);     //Assignment operator
  bool operator==(LHAItem &);         //Equivalence operator

  //Output a single line containing class data to output stream
  virtual void Print(ostream &out);

 protected:
  string block;     //Name of LHA block which to which item is associated
  string comment;   //Comment at the end of the line
  string codes;     //String containing integer codes, formatted with
                    //three spaces between each code
  string value;     //String containing input data
};

/******************************************************************************/

//LHABlock class
//Contains the name of the block and comment
//Future: will contain pointers to all associated LHAItems
class LHABlock {
 public:
  //Constructors
  LHABlock();
  LHABlock(stringstream &);
  LHABlock(string b, string c = "");

  //Destructor
  ~LHABlock() {}

  // << operator
  friend ostream & operator<<(ostream &out, LHABlock &block) {
    block.Print(out);
  }

  LHABlock & operator=(LHABlock &);     //Assignment operator
  bool operator==(LHABlock &);          //Equivalence operator

  //Return class data
  string GetBlock() {return block;}
  string GetComment() {return comment;}

  //Define class data
  void SetBlock(const char* b) {block = b;}
  void SetComment(const char* c) {comment = c;}

  //Output a single line containing block and comment to output stream
  void Print(ostream &out);

  //Add pointer to the LHAItems vector if it is not already present
  //Does not error check for identical data with different memory locations
  void AddItem(LHAItem *);

 protected:
  string block;      //Block name
  string comment;    //End of line comment
  vector<LHAItem *> blockItems;  //LHA items associated with the block
                                 //Points to identical memory
                                 //locations as the items in LHAData
};

/******************************************************************************/

//LHAData class
//Contains all information from an LHA file
//Items are stored in one array regardless of block, and blocks in another
class LHAData {
 public:
  //Constructors
  LHAData();
  LHAData(istream &);
  LHAData(const char *);
  LHAData(LHAData &data) {*this = data;}

  //Destructor
  ~LHAData();

  //Assignment operator.  Copies all items and blocks
  LHAData & operator=(LHAData &);

  //Read an lha file and store the data within
  //Appends read data to existing LHA data
  void Read(istream &);
  void Read(const char *);

  //Print out the stored data in LHA format
  //If a non-null vector containing block names is passed, only the
  //specified blocks (entries 1 through N) are printed in given order
  virtual void Print(ostream &out, vector<string> *ordering = NULL);
  virtual void Print(const char* file, vector<string> *ordering = NULL);

  //Pass a block/item to the class
  //The given block/item object is added to the class, replacing its
  //equivalent existing block/item if present.  The pointer location
  //of the passed object is not re-used.  The object is NOT deleted.
  LHABlock* AddLHABlock(LHABlock *, int deleteBlock = 0);
  LHAItem* AddLHAItem(LHAItem *, int deleteItem = 0);
  LHAItem* AddLHADecay(LHAItem *, int deleteItem = 0);

  //Modify a given item.  If the item does not yet exist, it is added
  //The memory location of the passed object is not re-used, but a new
  void Modify(const char *block, const char *codes, double value);
  void Modify(string block, string codes, double value);
  void Modify(LHAItem *item, int deleteItem = 0);

  //Get in stored value in an item.  If the item does not exist in the
  //class, the return value is 0.
  double GetValue(LHAItem *compare, int deleteItem = 0);
  double GetValue(const char *block, const char *codes);

 protected:
  vector<LHAItem *> lhaItems;     //Vector of stored items
  vector<LHAItem *> lhaDecays;    //Vector of decay widths
  vector<LHABlock *> lhaBlocks;   //Vector of stored blocks
  vector<vector<string> *> decayChannels; //Matrix of decay channels

  string subBlockType;
};

/******************************************************************************/

#endif
