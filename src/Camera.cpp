#include <thread>
#include <chrono>

#include <Camera.h>
#include <Smoother.h>
#include <pyfunc.h>

Camera::Camera(Config* _cfg) {
  cfg = _cfg;

  // setup camera
  do {
    std::cout << "attempt to open cam" << std::endl;
    m_ptrCam = new cv::VideoCapture(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  } while (!m_ptrCam->isOpened());

  // set image size
  m_imgWidth = cfg->getImageWidth();
  m_imgHeight = cfg->getImageHeight();

  m_ptrCam->set(cv::CAP_PROP_FRAME_WIDTH, m_imgWidth);
  m_ptrCam->set(cv::CAP_PROP_FRAME_HEIGHT, m_imgHeight);

  // frames per second
  m_fps = cfg->getFPS();

  // frame period in milliseconds
  m_fT_ms = static_cast<int>( 1e3f / m_fps );

  // circular buffer for last few frames
  for (int i = 0; i < M_CIRC_BUF_LEN; ++i) {
    m_frames.push_back( cv::Mat(m_imgHeight, m_imgWidth, CV_8UC3, cv::Scalar(0,0,0)) );
  }
  
  m_framePtr = 0;
}

Camera::~Camera() {
  m_ptrCam->release();
}

void Camera::run() {
  void (Camera::*job)();
  std::string mode = cfg->getMode();
  if ( !mode.compare("server") ) {
    job = &Camera::runAsServer;
  } else if ( !mode.compare("motion") ) {
    job = &Camera::runAsMotionDetector;
  }
 
  std::thread worker_thread(job, this);

  // camera acquistion loop: reads images at 50 FPS
  while ( !cfg->isTimeToQuit() ) {
    writeToCircBuf();
    std::this_thread::sleep_for(std::chrono::milliseconds( 20 ));
  }

  worker_thread.join();
}



/*
 *  Server mode
 *  
 *  Stream images to client
 */
void Camera::runAsServer() {
  std::cout << "Server Mode" << std::endl;

  // open a server socket that waits for a client
  try {
    // Create the socket
    ServerSocket server ( 30000 );

    while ( !cfg->isTimeToQuit() ) {

      ServerSocket new_sock;
      server.accept(new_sock);

      try {
        while ( !cfg->isTimeToQuit() ) {
          cv::Mat frame, gray;

          frame = getNewestImage();
          cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

          new_sock << gray;
          std::this_thread::sleep_for(std::chrono::milliseconds(m_fT_ms));
        }
      } catch ( SocketException& e ) {
        std::cout << "client lost: " << e.description() << std::endl;
      }
    }
  } catch ( SocketException& e ) {
    std::cout << e.description() << std::endl;
  }
}



/*
 *  Motion Detector mode
 *  
 *  Take an image once every 5 seconds, compare to previous image
 *  If the delta exceeds a threshold, begin video capture and save
 */
void Camera::runAsMotionDetector() {
  std::cout << "Motion Mode" << std::endl;
  
  while ( !cfg->isTimeToQuit() ) {
    // track the mean and variance of coords of pixel differences in time

    bool moving_obj = false;
    int motion_count = 0;

    cv::VideoWriter motionVid("motionCapture.avi", cv::VideoWriter::fourcc('M','J','P','G'),
                              m_fps, cv::Size(m_imgWidth, m_imgHeight), true);
    
    cv::Mat previous(m_imgHeight, m_imgWidth, CV_32FC1, cv::Scalar(0));
    
    std::cout << "looking for motion..." << std::endl;
    
    while ( !moving_obj && !cfg->isTimeToQuit() ) {

      cv::Mat frame, gray, grayf, diff, abs_of_diff;

      frame = getNewestImage();
      cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

      gray.convertTo(grayf, CV_32FC1);
      cv::subtract(grayf, previous, diff);
      previous = grayf;

      abs_of_diff = cv::abs(diff);

      float numchanged = 0.f;

      for (int r = 0; r < m_imgHeight; ++r) {
        for (int c = 0; c < m_imgWidth; ++c) {
          if (abs_of_diff.at<float>(r, c) > 10.f) {
            numchanged += 1.f;
          }
        }
      }

      // if at least so many pixels changed by at least so much
      if (numchanged > 20.f) {
        if (++motion_count == M_CIRC_BUF_LEN) moving_obj = true;
      } else {
        motion_count = --motion_count < 0 ? 0 : motion_count;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(m_fT_ms));
    }

    std::cout << "\trecording..." << std::endl;
    
    while ( moving_obj && !cfg->isTimeToQuit() ) {

      cv::Mat frame, gray, grayf, diff, abs_of_diff;

      // append these frames to video
      frame = getOldestImage();
      motionVid.write(frame);
      cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

      gray.convertTo(grayf, CV_32FC1);
      cv::subtract(grayf, previous, diff);
      previous = grayf;

      abs_of_diff = cv::abs(diff);

      float numchanged = 0.f;

      for (int r = 0; r < m_imgHeight; ++r) {
        for (int c = 0; c < m_imgWidth; ++c) {
          if (abs_of_diff.at<float>(r, c) > 10.f) {
            numchanged += 1.f;
          }
        }
      }

      // if at least so many pixels changed by at least so much
      if (numchanged > 20.f) {
        motion_count = (++motion_count > M_CIRC_BUF_LEN) ? M_CIRC_BUF_LEN : motion_count;
      } else {
        if (--motion_count == 0) moving_obj = false;
      }
      
      std::this_thread::sleep_for(std::chrono::milliseconds(m_fT_ms));
    }

    motionVid.release();
    std::cout << "\twriting video" << std::endl;
    callPythonFunc("fileTools", "shareVideo");
  }
}

void Camera::writeToCircBuf() {
  std::lock_guard<std::mutex> lock(mtx);
  *m_ptrCam >> m_frames[m_framePtr];
  m_framePtr = ++m_framePtr % M_CIRC_BUF_LEN;
}

const cv::Mat Camera::getOldestImage() const {
  std::lock_guard<std::mutex> lock(mtx);
  return m_frames[m_framePtr];
}

const cv::Mat Camera::getNewestImage() const {
  std::lock_guard<std::mutex> lock(mtx);
  int readIdx = ((m_framePtr - 1) < 0) ? M_CIRC_BUF_LEN - 1 : m_framePtr - 1;
  return m_frames[readIdx];
}
