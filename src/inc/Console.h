#pragma once

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <Config.h>
#include <server.h>
#include <client.h>
#include <SocketException.h>

class Console {
  Config* cfg = nullptr;
 public:
  Console() {}
  Console(Config* _cfg) :
          cfg(_cfg) {}
  void quit() {
    cfg->quit();
  }
  virtual ~Console() = default;
};


class ServerConsole : public Console {
 
 public:
  ServerConsole(Config* _cfg) : Console(_cfg) {}
  ~ServerConsole() {}

  void listen() {
    // open a server socket that waits for a client
    try {
      // Create the socket
      ServerSocket server ( 30001 );

      try {
        std::cout << "waiting for client" << std::endl;
        ServerSocket new_sock;
        server.accept(new_sock);

        std::string command("");
        while (command.compare("quit")) {
          new_sock >> command;
          std::cout << "ServerSocket recv: " << command << std::endl;
          std::this_thread::sleep_for( std::chrono::milliseconds(10) );
        }
        
        quit();

      }
      catch ( SocketException& e ) {
        std::cout << "Exception was caught: " << e.description() << "\nExiting.\n";
      }
    } catch ( SocketException& e ) {
      std::cout << "Exception was caught: " << e.description() << "\nExiting.\n";
    }
  }
};


class ClientConsole : public Console {
  std::string ipaddr;

 public:
  ClientConsole(const char* ip) : Console(), ipaddr(ip) {}
  ~ClientConsole() {}
  void connect() {
    try {
      ClientSocket client_socket ( ipaddr.c_str(), 30001 );

      try {
        while (true) {
          std::string input;
	  std::cin >> input;
	  client_socket << input;
        }
      } catch ( SocketException& e ) {
        std::cout << "Socket lost:" << e.description() << "\n";
      }
    } catch ( SocketException& e ) {
      std::cout << "Socket lost:" << e.description() << "\n";
    }
  }
};
