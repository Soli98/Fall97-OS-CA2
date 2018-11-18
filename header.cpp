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

void collectData(int prcCnt, vector<string>& pipeFiles, Filter& sortingValue, vector<Entry>& workersDecodedData) {
  string message;
  CompClass Comp(sortingValue);
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
}

Filter extractSortFilter(vector<Filter>& filters) {
  Filter sortingValue;
  for(int i = 0; i < filters.size(); i++)
  {
    if (filters[i].value == "ascend" || filters[i].value == "descend") {
      sortingValue.name = filters[i].name;
      sortingValue.value = filters[i].value;
    }
  }
  return sortingValue;
}

void parseFilters(vector<string>& fields) {
  string message;
  ifstream namedPipeFD(LB_FIFO_PATH);
  getline(namedPipeFD, message);
  namedPipeFD.close();
  string input(message), field;
  stringstream ss(input);
  while(getline(ss, field, '-')) {
    field = removeSpaces(field);
    fields.push_back(field);
  }
}

void runWorkers(vector<string>& files, vector<vector<string> >& workerFiles, string& directory, int prcCnt, vector<Filter>& filters, vector<string>& workerFilters) {
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
}

void runPresenter(Filter& sortingValue, int prcCnt) {
  mkdir("namedPipes", 0777);
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
}