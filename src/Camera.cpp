#include <thread>
#include <chrono>

#include <Camera.h>
#include <Smoother.h>
#include <pyfunc.h>

Camera::Camera(const Config& cfg) {
  // setup camera
  do {
    std::cout << "attempt to open cam" << std::endl;
    m_ptrCam = new cv::VideoCapture(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  } while (!m_ptrCam->isOpened());

  // set image size
  m_imgWidth = cfg.getImageWidth();
  m_imgHeight = cfg.getImageHeight();

  m_ptrCam->set(cv::CAP_PROP_FRAME_WIDTH, m_imgWidth);
  m_ptrCam->set(cv::CAP_PROP_FRAME_HEIGHT, m_imgHeight);

  // choose an operating mode, default is server mode
  m_mode = cfg.getMode();

  // frames per second
  m_fps = cfg.getFPS();

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
  if ( !m_mode.compare("server") ) {
    runAsServer();
  } else if ( !m_mode.compare("motion") ) {
    runAsMotionDetector();
  }
}

void Camera::runAsServer() {
  std::cout << "Server Mode" << std::endl;

  // open a server socket that waits for a client
  try {
    // Create the socket
    ServerSocket server ( 30000 );

    while ( true ) {

	  ServerSocket new_sock;
	  server.accept(new_sock);

	  try {
	    while (true) {

          cv::Mat frame, gray;

          *m_ptrCam >> frame;
          cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

          new_sock << gray;
          std::this_thread::sleep_for(std::chrono::milliseconds(m_fT_ms));
  
	    }
	  } catch ( SocketException& e ) {
        std::cout << "client lost: " << e.description() << std::endl;
      }
	}
  } catch ( SocketException& e ) {
    std::cout << "Exception was caught: " << e.description() << "\nExiting.\n";
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
  
  try {

    // track the mean and variance of coords of pixel differences in time

    bool moving_obj = false;
    int motion_count = 0;

    cv::VideoWriter motionVid("motionCapture.avi", cv::VideoWriter::fourcc('M','J','P','G'),
                              m_fps, cv::Size(m_imgWidth, m_imgHeight), true);
    
    cv::Mat previous(m_imgHeight, m_imgWidth, CV_32FC1, cv::Scalar(0));
    
    while ( !moving_obj ) {
      std::cout << "\tlooking for motion..." << std::endl;

      cv::Mat frame, gray, grayf, diff, abs_of_diff;

      *m_ptrCam >> frame;
      writeToCircBuf(frame);
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

    // unwrap the circular buffer here, prepend to video
    for (int i = 0; i < M_CIRC_BUF_LEN; ++i) {
      motionVid.write( readFromCircBuf() );
    }

    while ( moving_obj ) {
      std::cout << "\trecording motion!" << std::endl;

      cv::Mat frame, gray, grayf, diff, abs_of_diff;

      // append these frames to video
      *m_ptrCam >> frame;
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

  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  } 
}

void Camera::writeToCircBuf(const cv::Mat& img) {
  m_frames[m_framePtr] = img;
  m_framePtr = ++m_framePtr % M_CIRC_BUF_LEN;
}

const cv::Mat& Camera::readFromCircBuf() {
  cv::Mat* oldest = &(m_frames[m_framePtr]);
  m_framePtr = ++m_framePtr % M_CIRC_BUF_LEN;
  return *oldest;
}
