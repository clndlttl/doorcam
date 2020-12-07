// SocketException class

#pragma once

#include <string>

class SocketException
{
  std::string m_s;

 public:
  SocketException ( std::string s ) : m_s ( s ) {};
  ~SocketException (){};

  std::string description() { return m_s; }

};
