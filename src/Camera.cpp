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

    cv::Mat previous(m_imgHeight, m_imgWidth, CV_32FC1, cv::Scalar(0));
    bool moving_obj = false;
    bool firstpass = true;

    Smoother<float> smooth_x( m_imgWidth / 2.f  , 0.8f );
    Smoother<float> smooth_y( m_imgHeight / 2.f , 0.8f );

    while ( !moving_obj ) {
      cv::Mat frame, gray, grayf;
      cv::Mat diff(m_imgHeight, m_imgWidth, CV_32FC1, cv::Scalar(0)); 
      cv::Mat abs_of_diff;       

      *m_ptrCam >> frame;
      cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

      gray.convertTo(grayf, CV_32FC1);
      cv::subtract(grayf, previous, diff);
      previous = grayf;

      abs_of_diff = cv::abs(diff);

      float numchanged = 0.f;
      int sum_y = 0;
      int sum_x = 0;

      for (int r = 0; r < m_imgHeight; ++r) {
        for (int c = 0; c < m_imgWidth; ++c) {
          if (abs_of_diff.at<float>(r, c) > 10.f) {
            numchanged += 1.f;
            sum_y += r; sum_x += c;
          }
        }
      }

      // if at least so many pixels changed by at least so much
      if (numchanged > 20.f) {
        smooth_x.update( sum_x / numchanged );
        smooth_y.update( sum_y / numchanged );
        std::cout << smooth_x.mean() << "  " <<
                     smooth_y.mean() << "  " <<
                     smooth_x.var()  << "  " <<
                     smooth_y.var()  << std::endl;
      }

      // decision time: is something moving?
      

      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  } 
}
