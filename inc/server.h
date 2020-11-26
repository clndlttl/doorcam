//////////////////////////////////////////
//
// DOORCAM
// server.h
//

#pragma once

#include "socket.h"

// public and protected members in Socket
// are private in ServerSocket

class ServerSocket : private Socket {
 public:
  ServerSocket(int port);
  ServerSocket() {};
  virtual ~ServerSocket();

  const ServerSocket& operator << ( const std::string& ) const;
  const ServerSocket& operator << ( const cv::Mat& ) const;
  const ServerSocket& operator >> ( std::string& ) const;

  void accept(ServerSocket&);
};
