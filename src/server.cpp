//////////////////////////////////////////
//
// DOORCAM
// server.cpp
//

#include <server.h>
#include <SocketException.h>


ServerSocket::ServerSocket(int port) {
  if ( ! Socket::create() ) {
    throw SocketException ( "Could not create server socket." );
  }

  if ( ! Socket::bind(port) ) {
    throw SocketException ( "Could not bind to port." );
  }

  if ( ! Socket::listen() ) {
    throw SocketException ( "Could not listen to socket." );
  }
}


ServerSocket::~ServerSocket()
{
}


const ServerSocket& ServerSocket::operator << (const std::string& s) const {
  if ( ! Socket::send ( s ) ) {
    throw SocketException ("Could not write to socket.");
  }

  // temp
  std::cout << "server got " << s << std::endl;

  return *this;
}


const ServerSocket& ServerSocket::operator << (const cv::Mat& img) const {

  if ( ! Socket::send ( m_header, img ) ) {
    throw SocketException ("Could not write to socket.");
  }

  return *this;
}


const ServerSocket& ServerSocket::operator >> (std::string& s) const {
  if ( ! Socket::recv(s) ) {
    throw SocketException ("Could not read from socket.");
  }

  return *this;
}


void ServerSocket::accept(ServerSocket& sock) {
  if ( ! Socket::accept(sock) ) {
    throw SocketException ("Could not accept socket.");
  }
}


void ServerSocket::updateHeader(const boost::property_tree::ptree& header) {
  std::stringstream ss;
  write_json(ss, header, false);
  // client expects header to be a certain size
  std::string s = ss.str();
  if (s.size() < MAXRECV) m_header = pad(s);
  else std::cout << "header too big!" << std::endl;
}
