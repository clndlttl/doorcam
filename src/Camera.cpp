#include <Camera.h>

Camera::Camera(const ptree& cfg) {
  // setup camera
  do {
    std::cout << "attempt to open cam" << std::endl;
    m_ptrCam = new cv::VideoCapture(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  } while (!m_ptrCam->isOpened());

  // set image size
  int imgWidth = std::stoi( cfg.get("imgWidth", "320") );
  int imgHeight = std::stoi( cfg.get("imgHeight", "240") );

  m_ptrCam->set(cv::CAP_PROP_FRAME_WIDTH, imgWidth);
  m_ptrCam->set(cv::CAP_PROP_FRAME_HEIGHT, imgHeight);

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

    bool firsttime = true;
    cv::Mat previous;
    int diff_power = 0;

    while (diff_power < M_DIFF_THRESH) {
      cv::Mat frame, gray;
      cv::Mat diff(m_imgHeight, m_imgWidth, CV_32FC1, cv::Scalar(0)); 
      cv::Mat square(m_imgHeight, m_imgWidth, CV_32FC1);      

      *m_ptrCam >> frame;
      cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

      if (!firsttime) {
        diff = gray - previous;
      } else {
        firsttime = false;
      }

      previous = gray;

      cv::multiply(diff, diff, square);
      diff_power = cv::sum( square )[0];
      std::cout << "diff_power = " << diff_power << std::endl;

      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl;
  }    
}
