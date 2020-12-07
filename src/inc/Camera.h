#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <ptree.hpp>
#include <opencv2/opencv.hpp>
#include <server.h>
#include <SocketException.h>

using boost::property_tree::ptree;

class Camera {
  cv::VideoCapture* m_ptrCam;
  int m_imgWidth;
  int m_imgHeight;
  std::string m_mode;

  void runAsServer();
  void runAsMotionDetector();

 public:
  Camera(const ptree& cfg);
  ~Camera();
  void run();
};
