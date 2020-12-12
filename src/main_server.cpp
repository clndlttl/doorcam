/////////////////////////////////////////
//
// DOORCAM
// main_server.cpp
//

#include <iostream>
#include <string>
#include <ptree.hpp>
#include <json_parser.hpp>
#include <pyfunc.h>
#include <Camera.h>

int main() {

  // initialize via python call: filename, function
  callPythonFunc("doorcam_init", "configAndConnect");

  // read config file
  boost::property_tree::ptree cfg;
  boost::property_tree::json_parser::read_json(
    "/media/doorcam_config/doorcam_config.json",
    cfg
  ); 

  // pass config to camera and run
  Camera my_cam(cfg);
  my_cam.run(); 

  return 0;
}
