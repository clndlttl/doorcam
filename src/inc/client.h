// Definition of the ClientSocket class

#pragma once

#include "socket.h"

class ClientSocket : private Socket
{
 public:

  ClientSocket ( std::string host, int port );
  virtual ~ClientSocket(){};

  const ClientSocket& operator << ( const std::string& ) const;
  const ClientSocket& operator >> ( std::string& ) const;
  const ClientSocket& operator >> ( cv::Mat& ) const;

};
