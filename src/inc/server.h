//////////////////////////////////////////
//
// DOORCAM
// server.h
//

#pragma once

#include "socket.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// public and protected members in Socket
// are private in ServerSocket

class ServerSocket : private Socket {
  std::string m_header;
  
 public:
  ServerSocket(int port);
  ServerSocket() {};
  virtual ~ServerSocket();

  const ServerSocket& operator << ( const std::string& ) const;
  const ServerSocket& operator << ( const cv::Mat& ) const;
  const ServerSocket& operator >> ( std::string& ) const;

  void accept(ServerSocket&);

  void updateHeader(const boost::property_tree::ptree& header);
};
