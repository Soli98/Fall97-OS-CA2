#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

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
  cout << "Parent PID: " << getpid() << endl;
  vector<string> input = getInput();
  vector<Filter> filters = parseInput(input);
  vector<string> files;
  char buf[100];
  Filter sortingValue;
  string directory;
  int prcCnt;
  extractFilters(filters, sortingValue, directory, prcCnt);
  extractFiles(directory, files);
  cout << "#Process Count: " << prcCnt << endl;

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
      printf("Will exec worker...\n");
      int execstatus = execv("worker", _argsarr);
      cout << "EXEC STATUS: " << execstatus << endl;
      break; // Unsuccessful exec
    }
    else if(pid > 0) {
      close(pfd[0]);
      printf("Hello. I'm LoadBalancer.\n");
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
      cout << "Writing to pipe fd: " << pipes[i].second << endl;
      int written = write(pipes[i].second, workerFilters[i].c_str(), strlen(workerFilters[i].c_str()) + 1);
      printf("wrote: %d\n", written);
  }
  

  return 0;
}