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

  auto cfg = std::make_shared<Config>("/media/doorcam_config.json");

  // launch console
  std::thread console_thread( &ServerConsole::listen, ServerConsole(cfg) );

  // pass config to camera and run
  Camera& my_cam = Camera::getInstance();
  my_cam.configure(cfg);
  my_cam.run();

  std::cout << "Camera loop exited" << std::endl;
  console_thread.join();

  return 0;
}
