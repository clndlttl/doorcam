/////////////////////////////////////////
//
// DOORCAM
// main_server.cpp
//

#include <iostream>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>

#include <server.h>
#include <SocketException.h>

using namespace std;

int main(int argc, char *argv[] ) {

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
	  server.accept(new_sock);

	  try {
	    while (true) {
		  // std::string data;
		  // new_sock >> data;
		  // new_sock << data;

          cv::Mat frame, gray;

          *ptrCam >> frame;
          cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

          new_sock << gray;
          this_thread::sleep_for(chrono::milliseconds(100));
  
          // imwrite("image.jpg", gray);
	    }
	  }
	  catch ( SocketException& ) {}
	}
  }
  catch ( SocketException& e ) {
      std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
  }

  ptrCam->release();
  return 0;
}
