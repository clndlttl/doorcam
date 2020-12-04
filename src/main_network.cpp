#include <pyfunc.h>
#include <string>
#include <fstream>

int main(int argc, char** argv) {
  std::string filename, funcname;

  // looks for a thumbdrive, if it's there,
  // copy config file to /media/doorcam_config/doorcam_config.json
  filename = "doorcam_getconfig";
  funcname = "copyfile";
  callPythonFunc(filename, funcname);
  
  // check for network connectivity, if not connected,
  // use credentials in config file to make one
  filename = "doorcam_getwifi";
  funcname = "connect";
  callPythonFunc(filename, funcname);
}


