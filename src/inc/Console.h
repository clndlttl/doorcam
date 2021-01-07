#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <atomic>

#include <Config.h>
#include <server.h>
#include <client.h>
#include <SocketException.h>


class ServerConsole {
  Config* cfg = nullptr;
  using Args = const std::vector<std::string>&;
  using fptr = void(ServerConsole::*)(Args);
  std::unordered_map<std::string, fptr> funcMap;

  void quit(Args a);
  void setServerMode(Args a);
  void setMotionMode(Args a);
  void setResolution(Args a);

  void handle(const std::string& raw);

 public:

  ServerConsole(Config* _cfg);
  ~ServerConsole() {}
  
  void listen() {
    // open a server socket that waits for a client
    try {
      while ( !cfg->isTimeToQuit() ) {
        // Create the socket
        ServerSocket server ( 30001 );

        try {
          ServerSocket new_sock;
          server.accept(new_sock);

          while ( !cfg->isTimeToQuit() ) {
            std::string input;
            new_sock >> input;
            handle(input); 
          }
        } catch ( SocketException& e ) {
          std::cout << e.description() << std::endl;
        }
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
        while (*pMode != ::CLOSE) {
          std::string input;
	  std::cin >> input;

          if (::modeCode.find(input) != ::modeCode.end() ) {
            *pMode = ::modeCode[input];
	  }

	  // in src/inc/socket.h, see MAXRECV
	  // server socket is MSG_WAITALL, expects MAXRECV bytes
	  if (input.size() < MAXRECV)
	    client_socket << pad(input);
	  else
            std::cout << "too many characters!" << std::endl;
        }
      } catch ( SocketException& e ) {
        std::cout << "console loop: " << e.description() << std::endl;
      }
    } catch ( SocketException& e ) {
      std::cout << e.description() << std::endl;
    }
  }
};
