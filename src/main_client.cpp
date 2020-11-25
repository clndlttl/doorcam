// client
#include "client.h"
#include "SocketException.h"
#include <iostream>
#include <string>

int main ( int argc, char** argv ) {
  if (argc < 3) {
    std::cout << "usage: doorcam_client <ip_addr> <message>" << std::endl;
    return 0;
  }

  try {
    ClientSocket client_socket ( argv[1], 30000 );

    std::string reply;

    try {
      client_socket << argv[2];
      client_socket >> reply;
    } catch ( SocketException& ) {}

    std::cout << "We received this response from the server:\n\"" << reply << "\"\n";;

  } catch ( SocketException& e ) {
    std::cout << "Exception was caught:" << e.description() << "\n";
  }

  return 0;
}
