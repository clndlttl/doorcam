#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <ptree.hpp>
#include <opencv2/opencv.hpp>
#include <server.h>
#include <SocketException.h>

using boost::property_tree::ptree;

class Camera {

  static constexpr int M_CIRC_BUF_LEN = 5;

  cv::VideoCapture* m_ptrCam;
  int m_imgWidth;
  int m_imgHeight;
  std::string m_mode;
  float m_fps;
  int m_fT_ms;

  std::vector<cv::Mat> m_frames;
  int m_framePtr;

  void writeToCircBuf(const cv::Mat& img);
  const cv::Mat& readFromCircBuf();

  void runAsServer();
  void runAsMotionDetector();

 public:
  Camera(const ptree& cfg);
  ~Camera();
  void run();
};
