#ifndef HEADER_H
#define HEADER_H 

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

typedef struct Filter {
  string name;
  string value;

  Filter(): name(""), value(""){}
} Filter;

typedef struct Field
{
  string name;
  string value;
} Field;

typedef struct Entry
{
  vector<Field> fields;
} Entry;

struct CompClass {
  Filter _sortingValue;
  bool operator() (Entry A, Entry B) {
    for(int i = 0; i < A.fields.size(); i++)
    {
      if (A.fields[i].name == _sortingValue.name) {
        if(_sortingValue.value == "ascend") {
          return stoi(A.fields[i].value) < stoi(B.fields[i].value);
        }
        else {
          return stoi(A.fields[i].value) >= stoi(B.fields[i].value);
        }
      }
    }
    return 1;
  }
  CompClass(Filter sortingValue) : _sortingValue(sortingValue) {}
};

string removeSpaces(string str);
vector<string> getInput();
vector<Filter> parseInput(vector<string> input);
void extractFilters(vector<Filter>& filters, Filter& sortingValue, string& directory, int& prcCnt);
void extractFiles(string directory, vector<string>& files);
void extractFiles(vector<string>& files);
void readFile(string filename, vector<Entry>& data);
string serializeData(vector<Entry>& data);
void decodeData(string message, vector<Entry>& decoded);
void mergeData(vector<Entry>& totalData, vector<Entry>& decoded, Filter sort, CompClass Comp);
void decodeAndMergeData(string message, vector<Entry>& workersDecodedData, CompClass Comp, Filter sort);
void printData(vector<Entry>& data);

#endif