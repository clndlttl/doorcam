//////////////////////////////////////////
//
// DOORCAM
// socket.h
//

#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

#include <iostream>

#include <opencv2/opencv.hpp>

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 50;

// pad zeros to string for expected length
std::string pad(const std::string& s);

class Socket {
  int m_sock;
  sockaddr_in m_addr;

 public:
  Socket();
  virtual ~Socket();  // making the destructor virtual guarantees that
                      // derived classes will be destroyed via a base
                      // class pointer

  // Server initialization
  bool create();
  bool bind(const int port);
  bool listen() const;
  bool accept(Socket&) const;

  // Client initialization
  bool connect(const std::string host, const int port);

  // Data Transimission
  bool send(const std::string) const;
  bool send(const std::string& header, const cv::Mat&) const;
  int recv(std::string&) const;
  int recv(cv::Mat&) const;

  void set_non_blocking(const bool);

  bool is_valid() const { return m_sock != -1; }
};
