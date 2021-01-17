/////////////////////////////////////////
//
// DOORCAM
// main_server.cpp
//

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include <Config.h>
#include <Camera.h>
#include <Console.h>

int main(int argc, char** argv) {

  Config cfg("/media/doorcam_config.json");

  // launch console
  std::thread console_thread( &ServerConsole::listen, ServerConsole(&cfg) );

  // pass config to camera and run
  Camera my_cam(&cfg);
  my_cam.run();

  std::cout << "Camera loop exited" << std::endl;
  console_thread.join();

  return 0;
}
