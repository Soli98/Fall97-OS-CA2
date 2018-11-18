#include "header.h"


int main(int argc, char const *argv[])
{
  vector<Entry> data, filteredData;
  char buf[100];
  int readbytes = read(STDIN_FILENO, buf, 100);
  buf[readbytes] = '\0';
  vector<string> fields;
  string input(buf), field;
  stringstream ss(input);
  while(getline(ss, field, '-')) {
    field = removeSpaces(field);
    fields.push_back(field);
  }
  vector<Filter> filters;
  filters = parseInput(fields);
  for(int i = 0; i < argc; i++)
  {
    readFile(argv[i], data);
  }
  for(int i = 0; i < data.size(); i++)
  {
    bool isValid = true;
    for(int j = 0; j < data[i].fields.size(); j++)
    {
      bool isValidFilter = true;
      for(int k = 0; k < filters.size(); k++)
      {
        if (filters[k].name == data[i].fields[j].name) {
          if(filters[k].value == data[i].fields[j].value) {
            isValid = true;
            isValidFilter = true;
          }
          else {
            isValid = false;
            isValidFilter = false;
            break;
          }
        }
      }
      if(!isValidFilter) {
        break;
      }
    }
    if(isValid) {
      filteredData.push_back(data[i]);
    }
  }
  string serializedData = serializeData(filteredData);
  
  if (serializedData == "") {
    serializedData = "DONE\n";
  }
  
  string fifoname = "./namedPipes/namedPipe" + to_string(getpid());
  ofstream fifofile(fifoname);
  mkfifo(fifoname.c_str(), 0666);
  ofstream namedPipeFD2(fifoname);
  namedPipeFD2.write(serializedData.c_str(), strlen(serializedData.c_str()) + 1);
  namedPipeFD2.close();
  
  return 0;
}

