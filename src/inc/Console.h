#pragma once

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <unordered_map>

#include <Config.h>
#include <server.h>
#include <client.h>
#include <SocketException.h>


class ServerConsole {

  Config* cfg = nullptr;
  typedef void(ServerConsole::*fptr)(void);
  std::unordered_map<std::string, fptr> funcMap;

  void quit() { cfg->quit(); }
  void setServerMode() { cfg->setMode(::SERVER); }
  void setMotionMode() { cfg->setMode(::MOTION); }

 public:

  ServerConsole(Config* _cfg) : cfg(_cfg) {
    funcMap["quit"]   = &ServerConsole::quit;
    funcMap["server"] = &ServerConsole::setServerMode;
    funcMap["motion"] = &ServerConsole::setMotionMode;
  }
  
  ~ServerConsole() {}
  
  void listen() {
    // open a server socket that waits for a client
    try {
      // Create the socket
      ServerSocket server ( 30001 );

      try {
        ServerSocket new_sock;
        server.accept(new_sock);

        while ( !cfg->isTimeToQuit() ) {
          std::string command;
          new_sock >> command;

          if (funcMap.find(command) != funcMap.end()) {
            (this->*funcMap[command])();
	  } else {
            std::cout << "unknown command: " << command << std::endl;
	  }
        }

      }
      catch ( SocketException& e ) {
        std::cout << e.description() << std::endl;
      }
    } catch ( SocketException& e ) {
      std::cout << e.description() << std::endl;
    }
  }
};


class ClientConsole {
  std::string ipaddr;
  std::atomic<int>* pMode = nullptr;

 public:
  ClientConsole(const char* ip, std::atomic<int>* mode) : ipaddr(ip), pMode(mode) {}
  ~ClientConsole() {}

  void connect() {
    try {
      ClientSocket client_socket ( ipaddr.c_str(), 30001 );

      try {
        while (*pMode != ::QUIT) {
          std::string input;
	  std::cin >> input;

          if (::modeCode.find(input) != ::modeCode.end() ) {
            *pMode = ::modeCode[input];
	  }

	  client_socket << input;
        }
      } catch ( SocketException& e ) {
        std::cout << e.description() << std::endl;
      }
    } catch ( SocketException& e ) {
      std::cout << e.description() << std::endl;
    }
  }
};
