#include "header.h"

int main(int argc, char const *argv[])
{
  vector<Entry> workersDecodedData;
  vector<string> workersEncodedData;
  vector<string> pipeFiles;
  vector<string> fields;
  vector<Filter> filters;
  string message;
  parseFilters(fields);
  filters = parseInput(fields);
  Filter sortingValue;
  int prcCnt = stoi(filters[0].value);
  sortingValue = extractSortFilter(filters);
  extractFiles(pipeFiles);
  collectData(prcCnt, pipeFiles, sortingValue, workersDecodedData);
  printData(workersDecodedData);
  return 0;
}