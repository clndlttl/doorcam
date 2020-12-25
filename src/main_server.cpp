/////////////////////////////////////////
//
// DOORCAM
// main_server.cpp
//

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <pyfunc.h>

#include <Config.h>
#include <Camera.h>
#include <Console.h>

int main() {

  // initialize via python call: filename, function
  callPythonFunc("doorcam_init", "configAndConnect");

  Config cfg("/media/doorcam_config/doorcam_config.json");

  // launch console
  std::thread console_thread( &ServerConsole::listen, ServerConsole(&cfg) );

  // pass config to camera and run
  Camera my_cam(&cfg);
  my_cam.run();

  std::cout << "Camera loop exited" << std::endl;
  console_thread.join();

  return 0;
}
