#include <thread>
#include <chrono>

#include <Camera.h>
#include <Smoother.h>
#include <timestamp.h>

void Camera::configure(std::shared_ptr<Config> _cfg) {
  cfg = _cfg;

  init();
  
  // frames per second
  m_fps = cfg->getFPS();

  // frame period in milliseconds
  m_fT_ms = static_cast<int>( 1e3f / m_fps );
}


Camera::~Camera() {
  m_ptrCam->release();
}


bool Camera::accessCamera() {
  
  if (m_ptrCam != nullptr) m_ptrCam->release();
  
  for (int i = 0; i < 10; ++i) {
    std::cout << "attempt to open cam" << std::endl;
    m_ptrCam = std::make_unique<cv::VideoCapture>(0);
    if (!m_ptrCam->isOpened())
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    else
      return true;
  }
  
  return false;
}

void Camera::updateAll(ServerSocket* sock) {
  uint32_t todo = cfg->updateNeeded();
  if (todo & RESOLUTION) {
    init();
    sock->updateHeader(imgHeader);
    // pause so circ buf can refill - maybe unnecessary 
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
}


void Camera::init() {
  std::lock_guard<std::mutex> lock(mtx);

  if (!accessCamera()) {
    std::cout << "Can't access camera, exiting..." << std::endl;
    exit(0);
  }

  int width = cfg->getImageWidth();
  int height = cfg->getImageHeight();

  std::cout << width << "x" << height << std::endl;

  m_ptrCam->set(cv::CAP_PROP_FRAME_WIDTH, width);
  m_ptrCam->set(cv::CAP_PROP_FRAME_HEIGHT, height);
  
  m_imgWidth = width;  // m_ptrCam->get(cv::CAP_PROP_FRAME_WIDTH);
  m_imgHeight = height;  // m_ptrCam->get(cv::CAP_PROP_FRAME_HEIGHT);
  
  // update Config's ptree
  cfg->setImageWidth( std::to_string(m_imgWidth) );
  cfg->setImageHeight( std::to_string(m_imgHeight) );

  // update socket header ptree
  imgHeader.put("imgWidth", m_imgWidth);
  imgHeader.put("imgHeight", m_imgHeight);

  // init frame buffer 
  m_frames.clear();
  m_framePtr = 0;
  for (int i = 0; i < M_CIRC_BUF_LEN; ++i) {
    m_frames.push_back( cv::Mat(m_imgHeight, m_imgWidth, CV_8UC3, cv::Scalar(0,0,0)) );
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


void Camera::run() {
  while ( !cfg->isTimeToQuit() ) {
    void (Camera::*job)();
    int mode = cfg->getMode();
   
    if ( mode == ::SERVER ) {
      job = &Camera::runAsServer;
    } else if ( mode == ::MOTION ) {
      job = &Camera::runAsMotionDetector;
    }
 
    std::thread worker_thread(job, this);

    // camera acquistion loop: reads images at 50 FPS
    // don't add processing here!
    while ( cfg->getMode() == mode ) {
      writeToCircBuf();
      std::this_thread::sleep_for(std::chrono::milliseconds( 20 ));
    }

    worker_thread.join();
  }
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

    while ( cfg->getMode() == ::SERVER ) {

      ServerSocket new_sock;
      server.accept(new_sock);

      new_sock.updateHeader(imgHeader);

      try {
        while ( cfg->getMode() == ::SERVER ) {
	  
	  updateAll(&new_sock);
          
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
 *  Take an image once every second, compare to previous image
 *  If the delta exceeds a threshold, begin video capture and save
 */
void Camera::runAsMotionDetector() {
  std::cout << "Motion Mode" << std::endl;
  
  while ( cfg->getMode() == ::MOTION ) {
    // track the mean and variance of coords of pixel differences in time

    bool moving_obj = false;
    int motion_count = 0;

    std::string vidname("/share/motion_");
    vidname += get_timestamp();
    vidname += ".avi";

    cv::VideoWriter motionVid;
    
    cv::Mat previous(m_imgHeight, m_imgWidth, CV_32FC1, cv::Scalar(0));
    
    std::cout << "looking for motion..." << std::endl;
    
    while ( !moving_obj && (cfg->getMode() == ::MOTION) ) {

      cv::Mat frame, gray, grayf, diff, abs_of_diff;

      frame = getNewestImage();
      cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

      gray.convertTo(grayf, CV_32FC1);
      cv::subtract(grayf, previous, diff);
      previous = grayf;

      abs_of_diff = cv::abs(diff);

      int numchanged = 0;

      for (int r = 0; r < m_imgHeight; ++r) {
        for (int c = 0; c < m_imgWidth; ++c) {
          if (abs_of_diff.at<float>(r, c) > 20.f) {
            numchanged += 1;
          }
        }
      }

      // if at least so many pixels changed by at least so much
      if (numchanged > 20) {
        if (++motion_count == M_CIRC_BUF_LEN) moving_obj = true;
      } else {
        motion_count = --motion_count < 0 ? 0 : motion_count;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    while ( moving_obj && (cfg->getMode() == ::MOTION) ) {
      
      if (!motionVid.isOpened()) {
        motionVid = cv::VideoWriter(vidname, cv::VideoWriter::fourcc('M','J','P','G'),
                                    m_fps, cv::Size(m_imgWidth, m_imgHeight), true);
      }

      cv::Mat frame, gray, grayf, diff, abs_of_diff;

      // append these frames to video
      frame = getOldestImage();
      motionVid.write(frame);
      cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

      gray.convertTo(grayf, CV_32FC1);
      cv::subtract(grayf, previous, diff);
      previous = grayf;

      abs_of_diff = cv::abs(diff);

      int numchanged = 0;

      for (int r = 0; r < m_imgHeight; ++r) {
        for (int c = 0; c < m_imgWidth; ++c) {
          if (abs_of_diff.at<float>(r, c) > 20.f) {
            numchanged += 1;
          }
        }
      }

      std::cout << "numchanged = " << numchanged << std::endl;

      // if at least so many pixels changed by at least so much
      if (numchanged > 20) {
        motion_count = (++motion_count > M_CIRC_BUF_LEN) ? M_CIRC_BUF_LEN : motion_count;
      } else {
        if (--motion_count == 0) moving_obj = false;
      }
      
      std::this_thread::sleep_for(std::chrono::milliseconds(m_fT_ms));
    }

    motionVid.release();
  }
}
