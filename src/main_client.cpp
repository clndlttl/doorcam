/////////////////////////////////////////
//
// DOORCAM
// main_client.cpp
//

#include "client.h"
#include "SocketException.h"
#include <iostream>
#include <string>

int main ( int argc, char** argv ) {
  if (argc < 2) {
    std::cout << "usage: doorcam_client <ip_addr>" << std::endl;
    return 0;
  }

  try {
    ClientSocket client_socket ( argv[1], 30000 );

    cv::namedWindow("door cam", 1);
    cv::Mat gray(240, 320, CV_8UC1, cv::Scalar(0)); 

    try {
      while (true) {
        client_socket >> gray;
	std::cout << "attempt to imshow" << std::endl;
	cv::imshow("door cam", gray);
	cv::waitKey(10);
        // cv::imwrite("image.jpg", gray);
      }
    } catch ( SocketException& ) {}

    std::cout << "saved image.jpg!" << std::endl;

  } catch ( SocketException& e ) {
    std::cout << "Exception was caught:" << e.description() << "\n";
  }

  return 0;
}
