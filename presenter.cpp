#include "header.h"

int main(int argc, char const *argv[])
{
  string message;
  ifstream namedPipeFD("./namedPipes/LB2PR");
  getline(namedPipeFD, message);
  namedPipeFD.close();
  vector<string> fields;
  string input(message), field;
  stringstream ss(input);
  while(getline(ss, field, '-')) {
    field = removeSpaces(field);
    fields.push_back(field);
  }
  vector<Filter> filters;
  filters = parseInput(fields);
  int prcCnt = stoi(filters[0].value);
  Filter sortingValue;
  for(int i = 0; i < filters.size(); i++)
  {
    if (filters[i].value == "ascend" || filters[i].value == "descend") {
      sortingValue.name = filters[i].name;
      sortingValue.value = filters[i].value;
    }
  }
  CompClass Comp(sortingValue);
  vector<Entry> workersDecodedData;
  vector<string> workersEncodedData;
  vector<string> pipeFiles;
  extractFiles(pipeFiles);
  for(int i = 0; i < prcCnt; i++)
  {
    string fifopath = "./namedPipes/" + pipeFiles[i];
    ifstream namedPipeFD2(fifopath);
    getline(namedPipeFD2, message);
    if(message != "DONE") {
      namedPipeFD2.close();
      replace(message.begin(), message.end(), '%', '\n');
      decodeAndMergeData(message, workersDecodedData, Comp, sortingValue);
    }
  }

  printData(workersDecodedData);
  return 0;
}