#include <Camera.h>
#include <Smoother.h>

Camera::Camera(const ptree& cfg) {
  // setup camera
  do {
    std::cout << "attempt to open cam" << std::endl;
    m_ptrCam = new cv::VideoCapture(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  } while (!m_ptrCam->isOpened());

  // set image size
  m_imgWidth = std::stoi( cfg.get("imgWidth", "320") );
  m_imgHeight = std::stoi( cfg.get("imgHeight", "240") );

  m_ptrCam->set(cv::CAP_PROP_FRAME_WIDTH, m_imgWidth);
  m_ptrCam->set(cv::CAP_PROP_FRAME_HEIGHT, m_imgHeight);

  // choose an operating mode, default is server mode
  m_mode = cfg.get("mode", "server");

  // circular buffer for last few frames
  for (int i = 0; i < M_CIRC_BUF_LEN; ++i) {
    m_frames.push_back( cv::Mat(m_imgHeight, m_imgWidth, CV_8UC1, cv::Scalar(0)) );
  }
  
  m_writePtr = 0;
  m_readPtr = 1;
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
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
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
  try {

    // track the mean and variance of coords of pixel differences in time

    bool moving_obj = false;
    int motion_count = 0;

    // Smoother<float> smooth_x( m_imgWidth / 2.f  , 0.8f );
    // Smoother<float> smooth_y( m_imgHeight / 2.f , 0.8f );

    cv::VideoWriter motionVid("motionCapture.avi", cv::VideoWriter::fourcc('M','J','P','G'),
                              10, cv::Size(m_imgWidth, m_imgHeight), false);
    
    cv::Mat previous(m_imgHeight, m_imgWidth, CV_32FC1, cv::Scalar(0));
    
    while ( !moving_obj ) {

      cv::Mat frame, gray, grayf, diff, abs_of_diff;
      // cv::Mat diff(m_imgHeight, m_imgWidth, CV_32FC1, cv::Scalar(0));

      *m_ptrCam >> frame;
      cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
      writeToCircBuf(gray);

      gray.convertTo(grayf, CV_32FC1);
      cv::subtract(grayf, previous, diff);
      previous = grayf;

      abs_of_diff = cv::abs(diff);

      float numchanged = 0.f;
      // int sum_y = 0;
      // int sum_x = 0;

      for (int r = 0; r < m_imgHeight; ++r) {
        for (int c = 0; c < m_imgWidth; ++c) {
          if (abs_of_diff.at<float>(r, c) > 10.f) {
            numchanged += 1.f;
            // sum_y += r; sum_x += c;
          }
        }
      }

      // if at least so many pixels changed by at least so much
      if (numchanged > 20.f) {
        // smooth_x.update( sum_x / numchanged );
        // smooth_y.update( sum_y / numchanged );
        // std::cout << smooth_x.mean() << "  " <<
        //              smooth_y.mean() << "  " <<
        //              smooth_x.var()  << "  " <<
        //              smooth_y.var()  << std::endl;

        if (++motion_count == M_CIRC_BUF_LEN) moving_obj = true;
      } else {
        motion_count = --motion_count < 0 ? 0 : motion_count;
      }

      std::cout << "motion count = " << motion_count << std::endl;

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // unwrap the circular buffer here, prepend to video
    for (int i = 0; i < M_CIRC_BUF_LEN; ++i) {
      motionVid.write( readFromCircBuf() );
    }

    while ( moving_obj ) {

      cv::Mat frame, gray, grayf, diff, abs_of_diff;
      // cv::Mat diff(m_imgHeight, m_imgWidth, CV_32FC1, cv::Scalar(0));

      // append these frames to video
      *m_ptrCam >> frame;
      cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
      motionVid.write(gray);

      gray.convertTo(grayf, CV_32FC1);
      cv::subtract(grayf, previous, diff);
      previous = grayf;

      abs_of_diff = cv::abs(diff);

      float numchanged = 0.f;
      // int sum_y = 0;
      // int sum_x = 0;

      for (int r = 0; r < m_imgHeight; ++r) {
        for (int c = 0; c < m_imgWidth; ++c) {
          if (abs_of_diff.at<float>(r, c) > 10.f) {
            numchanged += 1.f;
            // sum_y += r; sum_x += c;
          }
        }
      }

      // if at least so many pixels changed by at least so much
      if (numchanged > 20.f) {
        // smooth_x.update( sum_x / numchanged );
        // smooth_y.update( sum_y / numchanged );
        // std::cout << smooth_x.mean() << "  " <<
        //              smooth_y.mean() << "  " <<
        //              smooth_x.var()  << "  " <<
        //              smooth_y.var()  << std::endl;

        motion_count = (++motion_count > M_CIRC_BUF_LEN) ? M_CIRC_BUF_LEN : motion_count;
      } else {
        if (--motion_count == 0) moving_obj = false;
      }

      std::cout << "motion count = " << motion_count << std::endl;
      
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // motionVid.release();

  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  } 
}

void Camera::writeToCircBuf(const cv::Mat& img) {
  m_frames[m_writePtr] = img;
  m_writePtr = ++m_writePtr % M_CIRC_BUF_LEN;
  m_readPtr = ++m_readPtr % M_CIRC_BUF_LEN;
}

const cv::Mat& Camera::readFromCircBuf() {
  cv::Mat* oldest = &(m_frames[m_readPtr]);
  m_readPtr = ++m_readPtr % M_CIRC_BUF_LEN;
  m_writePtr = ++m_writePtr % M_CIRC_BUF_LEN;
  return *oldest;
}
