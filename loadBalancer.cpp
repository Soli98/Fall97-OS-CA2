#include "header.h"

int main(int argc, char const *argv[])
{
  vector<string> input = getInput();
  vector<Filter> filters = parseInput(input);
  vector<string> files;
  char buf[100];
  Filter sortingValue;
  string directory;
  int prcCnt;
  extractFilters(filters, sortingValue, directory, prcCnt);
  extractFiles(directory, files);

  vector<vector<string> > workerFiles(prcCnt);
  vector<string> workerFilters(prcCnt);
  for(int i = 0; i < files.size(); i++)
  {
    workerFiles[i % prcCnt].push_back(directory + "/" + files[i]);
  }

  vector<pair<int, int> > pipes(prcCnt);
  for(int i = 0; i < prcCnt; i++)
  {
    vector<char*> _argv;
    for(int j = 0; j < workerFiles[i].size(); j++)
    {
      _argv.push_back((char*)workerFiles[i][j].c_str());
    }
    _argv.push_back(NULL);
    char** _argsarr = &_argv[0];
    int pfd[2];
    pipe(pfd);
    pipes[i] = make_pair(pfd[0], pfd[1]);
    int pid = fork();
    if(pid == 0) {
      dup2(pfd[0], STDIN_FILENO);
      close(pfd[0]);
      close(pfd[1]);
      int execstatus = execv("worker", _argsarr);
      break; // Unsuccessful exec
    }
    else if(pid > 0) {
      close(pfd[0]);
      string filtersToApply;
      for(int k = 0; k < filters.size(); k++)
      {
        string delim = (k == filters.size() - 1) ? "" : "-";
        filtersToApply += filters[k].name + "=" + filters[k].value + delim;
        workerFilters[i] = filtersToApply;
      }
    }
  }

  for(int i = 0; i < prcCnt; i++)
  {
      int written = write(pipes[i].second, workerFilters[i].c_str(), strlen(workerFilters[i].c_str()) + 1);
  }
  for(int i = 0; i < prcCnt; i++)
  {
    wait(NULL);
  }

  mkdir("namedPipes", 0777);
  // Create Named pipe
  string namedPipeName = "namedPipes/LB2PR";
  ofstream fifofile(namedPipeName.c_str());
  mkfifo(namedPipeName.c_str(), 0666);
  ofstream namedPipeFD(namedPipeName);

  int presenterPID = fork();
  
  if (presenterPID == 0) {
    execl("presenter", NULL);
  }
  string presenterMessage;
  string sortFilter;
  sortFilter = "-" + sortingValue.name + "=" + sortingValue.value + "\n";
  presenterMessage = "prc_cnt=" + to_string(prcCnt);
  if (sortingValue.name != "") {
    presenterMessage += sortFilter;
  }
  else {
    presenterMessage += "\n";
  }
  namedPipeFD.write(presenterMessage.c_str(), strlen(presenterMessage.c_str()) + 1);
  namedPipeFD.close();
  wait(NULL);
  
  execl("rm", "-rf", "namedPipes/");

  return 0;
}