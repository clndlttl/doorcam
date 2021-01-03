/////////////////////////////////////////
//
// DOORCAM
// main_client.cpp
//

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>

#include <client.h>
#include <SocketException.h>
#include <Console.h>

int main ( int argc, char** argv ) {
  if (argc < 2) {
    std::cout << "usage: doorcam_client <ip_addr>" << std::endl;
    return 0;
  }

  std::atomic<int> mode(::SERVER); 

  // launch console thread
  std::thread console_thread( &ClientConsole::connect, ClientConsole(argv[1], &mode ));

  try {
    while ((mode != ::CLOSE) && (mode != ::QUIT)) {
      if (mode == ::SERVER) {
        ClientSocket client_socket ( argv[1], 30000 );

        cv::namedWindow("door cam", 1);
        cv::Mat gray(240, 320, CV_8UC1, cv::Scalar(0)); 

        try {
          while (mode == ::SERVER) {
            client_socket >> gray;
            cv::imshow("door cam", gray);
            cv::waitKey(1);
          }
        } catch ( SocketException& e ) {
          std::cout << e.description() << std::endl;
        }
      } else if (mode == ::MOTION) {
        std::cout << "Camera in motion detection mode" << std::endl;
        while (mode == ::MOTION) {
          std::this_thread::sleep_for( std::chrono::seconds(1) );
        }
      }
    } 
  } catch ( SocketException& e ) {
    std::cout << e.description() << std::endl;
  }

  console_thread.join();

  return 0;
}
