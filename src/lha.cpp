#include"lha.h"

inline string DoubleToString(double d) {
  stringstream convert;
  convert << setprecision(8) << scientific << d;
  return convert.str();
}

inline double StringToDouble(string s) {
  stringstream convert(s);
  double d;
  convert >> d;
  return d;
}

inline int StringToInt(string s) {
  stringstream convert(s);
  int i;
  convert >> i;
  return i;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

LHAItem::LHAItem() : block(""), codes(""), value("0"), comment("") {}

LHAItem::LHAItem(string b, stringstream &input) : comment("") {
  this -> AssignLHA(b, input);
}

LHAItem::LHAItem(string b, const char *l) : comment("") {
  this -> AssignLHA(b, l);
}

LHAItem::LHAItem(string b, string c, double v) : comment("") {
  this -> AssignLHA(b, c, v);
}

LHAItem::LHAItem(string b, vector<string> &c, double v) : comment("") {
  this -> AssignLHA(b, c, v);
}

LHAItem & LHAItem::operator=(LHAItem &lha) {
  block = lha.block;
  value = lha.value;
  codes = lha.codes;
  if(lha.comment != "") comment = lha.comment;
}

bool LHAItem::operator==(LHAItem &lha) {
  if(block == lha.block && codes == lha.codes) return true;

  return false;
}

void LHAItem::AssignLHA(string b, stringstream &input) {
  block = b;

  string temp;
  vector<string> values(0);


  temp = input.str();
  int index;
  for(index = 0; index < temp.size(); index++) {
    if(temp[index] == '#') break;
  }

  if(index >= temp.size()) {
    // input >> temp;

    while(!input.eof()) {
      input >> temp;
      values.push_back(temp);
    }

    value = values.back();
    values.pop_back();
    SetCodes(values);
  }
  else {
    char buff[200];
    input.getline(buff, 200, '#');
    string line = buff;
    stringstream valueInput(line);

    valueInput >> temp;

    while(!valueInput.eof()) {
      values.push_back(temp);
      valueInput >> temp;
    }

    value = values.back();
    values.pop_back();
    SetCodes(values);

    input.getline(buff, 200);
    comment = buff;
  }
}

void LHAItem::AssignLHA(string b, const char *l) {
  stringstream convert;
  convert << l;
  AssignLHA(b, convert);
}

void LHAItem::AssignLHA(string b, string c, double v, string com) {
  block = b;
  codes = c;
  value = DoubleToString(v);
  if(com != "") comment = com;
}

void LHAItem::AssignLHA(string b, vector<string> &c, double v, string com) {
  block = b;
  SetCodes(c);
  value = DoubleToString(v);
  if(com != "") comment = com;
}

double LHAItem::GetValue() {
  return StringToDouble(value);
}

void LHAItem::SetValue(double v) {
  value = DoubleToString(v);
}

void LHAItem::SetCodes(vector<string> &c) {
  stringstream convert;
  for(int i = 0; i < c.size(); i++) {
    convert << c[i];
    if(i < c.size() - 1) convert << "   ";
  }

  codes = convert.str();
}

void LHAItem::Print(ostream &out) {
  out << setprecision(0) << fixed
      << setw(16) << codes
      << setprecision(8) << scientific << setw(20) << value
      << "   # " << comment << "\n";
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

LHABlock::LHABlock() : block(""), comment(""), blockItems(0) {}

LHABlock::LHABlock(stringstream &input) : blockItems(0) {
  input >> block;
  char buff[200];
  input.getline(buff, 200);
  comment = buff;
}

LHABlock::LHABlock(string b, string c) {
  block = b;
  comment = c;
}

LHABlock & LHABlock::operator=(LHABlock &lha) {
  block = lha.block;
  if(lha.comment != "") comment = lha.comment;
}

bool LHABlock::operator==(LHABlock &lha) {
  if(block == lha.block) return true;
  return false;
}

void LHABlock::Print(ostream &out) {
  out << "Block  " << block << "  " << comment << endl;
  for(int i = 0; i < blockItems.size(); i++) {
    out << *(blockItems[i]);
  }
}

void LHABlock::AddItem(LHAItem *item) {
  for(int i = 0; i < blockItems.size(); i++)
    if(blockItems[i] == item) return;

  blockItems.push_back(item);
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

LHAData::LHAData() : lhaBlocks(0), lhaItems(0), lhaDecays(0),
		     decayChannels(0) {
  subBlockType = "";
}

LHAData::LHAData(istream &in) : lhaBlocks(0), lhaItems(0), lhaDecays(0) {
  Read(in);
  subBlockType = "";
}

LHAData::LHAData(const char* file) : lhaBlocks(0), lhaItems(0), lhaDecays(0) {
  Read(file);
  subBlockType = "";
}

LHAData::~LHAData() {
  for(int i = 0; i < lhaBlocks.size(); i++)
    delete lhaBlocks[i];
  for(int j = 0; j < lhaItems.size(); j++)
    delete lhaItems[j];
  for(int k = 0; k < lhaItems.size(); k++)
    delete lhaDecays[k];
}

LHAData & LHAData::operator=(LHAData &lha) {
  for(int i = 0; i < lha.lhaBlocks.size(); i++)
    AddLHABlock(lha.lhaBlocks[i]);

  for(int j = 0; j < lha.lhaItems.size(); j++)
    AddLHAItem(lha.lhaItems[j]);

  for(int k = 0; k < lha.lhaDecays.size(); k++)
    AddLHADecay(lha.lhaDecays[k]);

  subBlockType = lha.subBlockType;
}

void LHAData::Read(istream &in) {
  stringstream convert;
  string test;
  char buff[200];
  LHABlock *currentBlock = NULL;

  while(!in.eof()) {
    convert.str("");
    convert.seekp(0, ios_base::beg);
    convert.clear();
    in.getline(buff, 200);
    convert << buff;
    convert >> test;

    if(in.eof()) break;
    if(convert.eof() || test == "#") continue;
    else if(test == "DECAY") {
      AddLHADecay(new LHAItem(string("DECAY"), convert), 1);
      subBlockType = "DECAY";
      decayChannels.push_back(new vector<string>(0));
    }
    else if(test == "Block") {
      currentBlock = AddLHABlock(new LHABlock(convert), 1);
      subBlockType = "Block";
    }
    else {
      if(subBlockType == "Block") {
	convert.str("");
	convert.seekp(0, ios_base::beg);
	convert.clear();
	convert << buff;

	if(currentBlock != NULL) {
	  AddLHAItem(new LHAItem(currentBlock -> GetBlock(), buff), 1);
	}
      }
      if(subBlockType == "DECAY") {
	decayChannels[decayChannels.size() - 1] -> push_back(buff);
      }
    }
  }
}

void LHAData::Read(const char* file) {
  ifstream fin(file);
  if(fin.is_open()) Read(fin);
}

void LHAData::Print(ostream &out, vector<string> *ordering) {
  if(ordering == NULL) {
    for(int i = 0; i < lhaBlocks.size(); i++) out << *(lhaBlocks[i]);
  }
  else {
    for(int i = 1; i < ordering -> size(); i++) {
      for(int j = 0; j < lhaBlocks.size(); j++) {
	if(lhaBlocks[j] -> GetBlock() == ordering -> at(i)) {
	  out << *(lhaBlocks[j]);
	  break;
	}
      }
    }
  }

  if(lhaDecays.size() > 0) out << "# Table of decay widths" << "\n";

  for(int i = 0; i < lhaDecays.size(); i++) {
    out << "DECAY  " << *(lhaDecays[i]);
    for(int j = 0; j < decayChannels[i] -> size(); j++) {
      out << (*decayChannels[i])[j] << endl;
    }
  }
  out << endl;
}

void LHAData::Print(const char* file, vector<string> *ordering) {
  ofstream fout(file);
  Print(fout, ordering);
}

LHAItem* LHAData::AddLHAItem(LHAItem *item, int deleteItem) {
  if(item == NULL) return NULL;
  for(int i = 0; i < lhaItems.size(); i++) {
    if(*item == *(lhaItems[i])) {
      if(lhaItems[i] -> GetComment() == "")
	lhaItems[i] -> SetComment(item -> GetComment().c_str());
      lhaItems[i] -> SetValue(item -> GetValue());
      if(deleteItem) delete item;
      return lhaItems[i];
    }
  }

  lhaItems.push_back(new LHAItem(*item));
  if(deleteItem) delete item;

  int index;
  for(index = 0; index < lhaBlocks.size(); index++) {
    if(lhaItems.back() -> GetBlock() == lhaBlocks[index] -> GetBlock()) {
      lhaBlocks[index] -> AddItem(lhaItems.back());
      break;
    }
  }
  if(index >= lhaBlocks.size()) {
    LHABlock *tempBlock = AddLHABlock(new LHABlock(lhaItems.back() 
						   -> GetBlock()), 1);
    tempBlock -> AddItem(lhaItems.back());
  }

  return lhaItems.back();
}

LHAItem* LHAData::AddLHADecay(LHAItem *item, int deleteItem) {
  if(item == NULL) return NULL;
  for(int i = 0; i < lhaDecays.size(); i++) {
    if(*item == *(lhaDecays[i])) {
      if(lhaDecays[i] -> GetComment() == "")
	lhaDecays[i] -> SetComment(item -> GetComment().c_str());
      lhaDecays[i] -> SetValue(item -> GetValue());
      if(deleteItem) delete item;
      return lhaDecays[i];
    }
  }

  lhaDecays.push_back(new LHAItem(*item));
  if(deleteItem) delete item;
  return lhaDecays.back();
}

LHABlock* LHAData::AddLHABlock(LHABlock *block, int deleteBlock) {
  if(block == NULL) return NULL;
  for(int i = 0; i < lhaBlocks.size(); i++) {
    if(*block == *(lhaBlocks[i])) {
      if(lhaBlocks[i] -> GetComment() == "")
	lhaBlocks[i] -> SetComment(block -> GetComment().c_str());
      if(deleteBlock) delete block;
      return lhaBlocks[i];
    }
  }

  lhaBlocks.push_back(new LHABlock(*block));
  if(deleteBlock) delete block;
  return lhaBlocks.back();
}

void LHAData::Modify(const char *block, const char *codes, double value) {
  Modify(string(block), string(codes), value);
}

void LHAData::Modify(string block, string codes, double value) {
  AddLHABlock(new LHABlock(block), 1);
  AddLHAItem(new LHAItem(block, codes, value), 1);
}

void LHAData::Modify(LHAItem *item, int deleteItem) {
  if(item == NULL) return;
  AddLHABlock(new LHABlock(item -> GetBlock()), 1);
  AddLHAItem(item, deleteItem);
}

double LHAData::GetValue(LHAItem *compare, int deleteItem) {
  if(compare == NULL) return 0;
  for(int i = 0; i < lhaItems.size(); i++) {
    if(*compare == *(lhaItems[i])) {
      if(deleteItem) delete compare;
      return lhaItems[i] -> GetValue();
    }
  }

  if(deleteItem) delete compare;
  return 0;
}

double LHAData::GetValue(const char *block, const char *codes) {
  string b = block;
  string c = codes;

  return GetValue(new LHAItem(block, codes), 1);
}
