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

    // std::string reply;
    cv::Mat gray(240, 320, CV_8UC1, 0); 

    try {
      // client_socket << argv[2];
      // client_socket >> reply;
      client_socket >> gray;
      cv::imwrite("image.jpg", gray);

    } catch ( SocketException& ) {}

    std::cout << "saved image.jpg!" << std::endl;

  } catch ( SocketException& e ) {
    std::cout << "Exception was caught:" << e.description() << "\n";
  }

  return 0;
}
