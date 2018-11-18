#include "header.h"

string removeSpaces(string str)  
{ 
    str = regex_replace(str, regex("^ +| +$|( ) +"), "$1");
    return str; 
}

vector<string> getInput() {
  string input, field;
  vector<string> fields;
  cout << "Enter your search query:" << endl;
  getline(cin, input);
  stringstream ss(input);
  while(getline(ss, field, '-')) {
    field = removeSpaces(field);
    fields.push_back(field);
  }
  return fields;
}

vector<Filter> parseInput(vector<string> input) {
  vector<Filter> filters;
  for(int i = 0; i < input.size(); i++)
  {
    string in, name, value;
    in = input[i];
    stringstream ss(in);
    getline(ss, name, '=');
    name = removeSpaces(name);
    getline(ss, value, '=');
    value = removeSpaces(value);
    Filter filter;
    filter.name = name;
    filter.value = value;
    filters.push_back(filter);
  }
  return filters;
}

void extractFilters(vector<Filter>& filters, Filter& sortingValue, string& directory, int& prcCnt) {
  int svIndex = -1, pcIndex, dIndex;
  for(int i = 0; i < filters.size(); i++)
  {
    if (filters[i].value == "ascend" || filters[i].value == "descend") {
      sortingValue.name = filters[i].name;
      sortingValue.value = filters[i].value;
      svIndex = i;
    }
    if (filters[i].name == "prc_cnt") {
      prcCnt = stoi(filters[i].value);
      pcIndex = i;
    }
    if (filters[i].name == "dir") {
      directory = filters[i].value;
      dIndex = i;
    }
  }
  if (svIndex >= 0) {
    filters.resize(svIndex);
  } else {
    filters.resize(pcIndex);
  }
}

void extractFiles(string directory, vector<string>& files) {
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(directory.c_str())) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      if (ent->d_name[0] != '.') {
        files.push_back(string(ent->d_name));
      }
    }
    closedir (dir);
  } else {
    perror ("");
  }
}

void extractFiles(vector<string>& files) {
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir("namedPipes")) != NULL) {
    while ((ent = readdir(dir)) != NULL) {
      if (ent->d_name[0] == 'n' && ent->d_name[1] == 'a' && ent->d_name[2] == 'm') {
        files.push_back(string(ent->d_name));
      }
    }
    closedir (dir);
  } else {
    perror ("");
  }
}

void readFile(string filename, vector<Entry>& data) {
  ifstream fd(filename);
  string fieldsLine, fieldName, line, fieldValue;
  vector<string> fieldNames;
  getline(fd, fieldsLine);
  stringstream ss(fieldsLine);
  while(ss >> fieldName){
    fieldNames.push_back(fieldName);
  }
  while (getline(fd, line)) {
    Entry e;
    stringstream ss(line);
      for(int i = 0; i < fieldNames.size(); i++)
      {
        Field f;
        f.name = fieldNames[i];
        ss >> fieldValue;
        f.value = fieldValue;
        e.fields.push_back(f);
      }
    data.push_back(e);
  }
}

string serializeData(vector<Entry>& data) {
  string serializedData = "";
  if(data.size() > 0) {
    string fields;
      for(int j = 0; j < data[0].fields.size(); j++)
      {
        string delim = (j == data[0].fields.size() - 1) ? "" : " ";
        fields += data[0].fields[j].name + delim;
      }
      serializedData += fields + "%";
  }
  
  for(int i = 0; i < data.size(); i++)
  {
    string entry;
    for(int j = 0; j < data[i].fields.size(); j++)
    {
      string delim = (j == data[i].fields.size() - 1) ? "" : " ";
      entry +=  data[i].fields[j].value + delim;
    }
    serializedData += entry + "%";
  }
  if(data.size() > 0)
    serializedData += "\n";
  return serializedData;
}

void decodeData(string message, vector<Entry>& decoded) {
  string fieldsLine, fieldName, line, fieldValue;
  vector<string> fieldNames;
  stringstream messagestream(message);
  getline(messagestream, fieldsLine);
  stringstream ss(fieldsLine);
  while(ss >> fieldName){
    fieldNames.push_back(fieldName);
  }
  while (getline(messagestream, line)) {
    Entry e;
    stringstream ss(line);
      for(int i = 0; i < fieldNames.size(); i++)
      { 
        Field f;
        f.name = fieldNames[i];
        ss >> fieldValue;
        f.value = fieldValue;
        e.fields.push_back(f);
      }
    decoded.push_back(e);
  }
}

void mergeData(vector<Entry>& totalData, vector<Entry>& decoded, Filter sort, CompClass Comp) {
  
  if (sort.name == "") {
    for(int i = 0; i < decoded.size(); i++)
    {
      totalData.push_back(decoded[i]);
    }
  }
  else {
    for(int i = 0; i < decoded.size(); i++)
    {
      if (sort.value == "ascend") {
        totalData.insert(upper_bound(totalData.begin(), totalData.end(), decoded[i], Comp), decoded[i]);
      }
      else {
        totalData.insert(lower_bound(totalData.begin(), totalData.end(), decoded[i], Comp), decoded[i]);
      }
    }

  }
}

void decodeAndMergeData(string message, vector<Entry>& workersDecodedData, CompClass Comp, Filter sort) {
  vector<Entry> decoded;
  decodeData(message, decoded);
  mergeData(workersDecodedData, decoded, sort, Comp);
}

void printData(vector<Entry>& data) {
  cout << "-------------DATA-------------" << endl;
  for(int i = 0; i < data.size(); i++)
  {
    for(int j = 0; j < data[i].fields.size(); j++)
    {
      cout << data[i].fields[j].value << " ";
    }
    cout << endl;
  }
}