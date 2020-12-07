/////////////////////////////////////////
//
// DOORCAM
// main_server.cpp
//

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>

#include <ptree.hpp>
#include <json_parser.hpp>

#include <server.h>
#include <SocketException.h>
#include <pyfunc.h>

using namespace std;

int main(int argc, char *argv[] ) {
  std::string filename, funcname;

  // initialize via python call
  filename = "doorcam_init";
  funcname = "configAndConnect";
  callPythonFunc(filename, funcname);

  // read config file
  boost::property_tree::ptree cfg;
  boost::property_tree::json_parser::read_json(
    "/media/doorcam_config/doorcam_config.json",
    cfg
  ); 

  cv::VideoCapture* ptrCam;
  do {
    cout << "attempt to open cam" << endl;
    ptrCam = new cv::VideoCapture(0);
    this_thread::sleep_for(chrono::milliseconds(1000));
  } while (!ptrCam->isOpened());

  int imgWidth = 320;
  int imgHeight = 240;

  ptrCam->set(cv::CAP_PROP_FRAME_WIDTH, imgWidth);
  ptrCam->set(cv::CAP_PROP_FRAME_HEIGHT, imgHeight);

  // open a server socket that waits for a client
  try {
    // Create the socket
    ServerSocket server ( 30000 );

    while ( true ) {

	  ServerSocket new_sock;
      std::cout << "wait for client" << std::endl;
	  server.accept(new_sock);
      std::cout << "found client" << std::endl;

	  try {
	    while (true) {

          cv::Mat frame, gray;

          *ptrCam >> frame;
          cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

          new_sock << gray;
          this_thread::sleep_for(chrono::milliseconds(100));
  
	    }
	  }
	  catch ( SocketException& e ) {
        std::cout << "client lost: " << e.description() << std::endl;
      }
	}
  }
  catch ( SocketException& e ) {
      std::cout << "Exception was caught: " << e.description() << "\nExiting.\n";
  }

  ptrCam->release();
  return 0;
}
