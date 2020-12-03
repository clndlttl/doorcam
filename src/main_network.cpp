#include <pyfunc.h>
#include <string>
#include <fstream>

int main(int argc, char** argv) {
  // temp: test python
  std::string filename, funcname;

  // read in doorcam_config.json
  // need to check for drive on sda1,
  // if it's there, try reading .json file
  filename = "getconfig";
  funcname = "copyConfigFile";
  callPythonFunc(filename, funcname);
  
  // then check for network connectivity (python)
  // should python do the drive read too?
  // if no connection, make it with creds in .json
}


