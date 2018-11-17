#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <unistd.h>
using namespace std;

typedef struct Field
{
  string name;
  string value;
} Field;

typedef struct Entry
{
  vector<Field> fields;
} Entry;

vector<Entry> readFile(string filename) {
  vector<Entry> data;
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
  return data;
}

int main(int argc, char const *argv[])
{
  cout << "Worker PID: " << getpid() << endl;
  fprintf(stdout, "Hello. I'm a Worker.\n");
  cout << "Files assigned to me:" << endl;
  for(int i = 0; i < argc; i++)
  {
    cout << argv[i] << endl;
  }
  char buf[100];
  int readbytes = read(STDIN_FILENO, buf, 100);
  buf[readbytes] = '\0';
  cout << "read " << readbytes << " bytes: " << buf << endl;
  

  
  vector<Entry> data;
  string in;
  data = readFile(argv[0]);
  
  return 0;
}
