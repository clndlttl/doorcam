/////////////////////////////////////////
//
// DOORCAM
// main.cpp
//

#include <iostream>
#include <thread>
#include <chrono>
#include <opencv2/opencv.hpp>

#include <server.h>
#include <SocketException.h>

using namespace cv;
using namespace std;

int main(int argc, char *argv[] ) {

  VideoCapture* ptrCam;
  do {
    cout << "attempt to open cam" << endl;
    ptrCam = new VideoCapture(0);
    this_thread::sleep_for(chrono::milliseconds(1000));
  } while (!ptrCam->isOpened());

  int imgWidth = 320;
  int imgHeight = 240;

  ptrCam->set(CV_CAP_PROP_FRAME_WIDTH, imgWidth);
  ptrCam->set(CV_CAP_PROP_FRAME_HEIGHT, imgHeight);

  // open a server socket that waits for a client
  try {
    // Create the socket
    ServerSocket server ( 30000 );

    while ( true ) {

	  ServerSocket new_sock;
	  server.accept ( new_sock );

	  try {
	    while ( true ) {
		  std::string data;
		  new_sock >> data;
		  new_sock << data;
	    }
	  }
	  catch ( SocketException& ) {}

	}
  }
  catch ( SocketException& e ) {
      std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
  }



  Mat frame, gray;

  *ptrCam >> frame;
  cvtColor(frame, gray, CV_BGR2GRAY);

  imwrite("image.jpg", gray);
  cout << "wrote image.jpg" << endl;

  ptrCam->release();
  return 0;
}
