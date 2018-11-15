#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <dirent.h>

using namespace std;

typedef struct Filter {
  string name;
  string value;

  Filter(): name(""), value(""){}
} Filter;

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

int main(int argc, char const *argv[])
{
  vector<string> input = getInput();
  vector<Filter> filters = parseInput(input);
  vector<string> files;
  Filter sortingValue;
  string directory;
  int prcCnt;
  extractFilters(filters, sortingValue, directory, prcCnt);
  extractFiles(directory, files);
  
  return 0;
}