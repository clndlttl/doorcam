#include <iostream>
#include <string>
#include <vector>
#include <mutex>

#include <opencv2/opencv.hpp>
#include <server.h>
#include <SocketException.h>
#include <Config.h>

using boost::property_tree::ptree;

class Camera {
  std::shared_ptr<Config> cfg;
  ptree imgHeader;
  static constexpr int M_CIRC_BUF_LEN = 5;

  std::unique_ptr<cv::VideoCapture> m_ptrCam;
  int m_imgWidth;
  int m_imgHeight;
  std::string m_mode;
  float m_fps;
  int m_fT_ms;

  mutable std::mutex mtx;

  std::vector<cv::Mat> m_frames;
  int m_framePtr;

  void writeToCircBuf();
  const cv::Mat getOldestImage() const;
  const cv::Mat getNewestImage() const;

  void runAsServer();
  void runAsMotionDetector();

  void updateAll(ServerSocket*);
  void init();

  bool accessCamera();

  // private cstr (singleton)
  Camera() = default;

 public:
  static Camera& getInstance() {
    static Camera cam;
    return cam;
  }
  void configure(std::shared_ptr<Config>);
  void run();
  ~Camera();
};
