/////////////////////////////////////////
//
// DOORCAM
// main_client.cpp
//

#include <iostream>
#include <string>
#include <thread>
#include <client.h>
#include <SocketException.h>
#include <Console.h>

int main ( int argc, char** argv ) {
  if (argc < 2) {
    std::cout << "usage: doorcam_client <ip_addr>" << std::endl;
    return 0;
  }

  // launch console thread
  std::thread console_thread( &ClientConsole::connect, ClientConsole(argv[1]) );

  try {
    ClientSocket client_socket ( argv[1], 30000 );

    cv::namedWindow("door cam", 1);
    cv::Mat gray(240, 320, CV_8UC1, cv::Scalar(0)); 

    try {
      while (true) {
        client_socket >> gray;
        cv::imshow("door cam", gray);
        cv::waitKey(10);
      }
    } catch ( SocketException& ) {}

  } catch ( SocketException& e ) {
    std::cout << "Socket lost:" << e.description() << "\n";
  }
  
  console_thread.join();

  return 0;
}
