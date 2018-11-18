#include "header.h"

int main(int argc, char const *argv[])
{
  vector<string> input = getInput();
  vector<Filter> filters = parseInput(input);
  vector<string> files;
  int prcCnt;
  Filter sortingValue;
  string directory;
  extractFilters(filters, sortingValue, directory, prcCnt);
  extractFiles(directory, files);
  vector<vector<string> > workerFiles(prcCnt);
  vector<string> workerFilters(prcCnt);
  runWorkers(files, workerFiles, directory, prcCnt, filters, workerFilters);
  // Wait for workers to finish
  for(int i = 0; i < prcCnt; i++)
  {
    wait(NULL);
  }

  runPresenter(sortingValue, prcCnt);
  wait(NULL);

  // execl("rm", "-rf", "namedPipes/");
  return 0;
}