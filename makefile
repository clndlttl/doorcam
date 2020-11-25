
CC=/usr/bin/g++

SRC_DIR=./src

SRVR_SRCS=$(SRC_DIR)/main.cpp \
          $(SRC_DIR)/socket.cpp \
          $(SRC_DIR)/server.cpp

CLNT_SRCS=$(SRC_DIR)/main_client.cpp \
          $(SRC_DIR)/socket.cpp \
          $(SRC_DIR)/client.cpp

INC_DIR=./inc

CV_INC_DIR=/usr/local/opencv/include/

CV_LINK_DIR=/usr/local/lib/opencv

CVLIBS= -lopencv_core \
	-lopencv_highgui \
	-lopencv_imgcodecs \
	-lopencv_calib3d \
	-lopencv_features2d \
	-lopencv_flann \
	-lopencv_imgproc \
	-lopencv_ml \
	-lopencv_objdetect \
	-lopencv_photo \
	-lopencv_stitching \
	-lopencv_videoio \
	-lopencv_video \

CFLAGS= -std=c++11 -I$(INC_DIR) -I$(CV_INC_DIR) -L$(CV_LINK_DIR) $(CVLIBS)

doorcam_server: $(SRVR_SRCS)
	$(CC) $(SRVR_SRCS) $(CFLAGS) -o doorcam_server

doorcam_client: $(CLNT_SRCS)
	$(CC) $(CLNT_SRCS) $(CFLAGS) -o doorcam_client
