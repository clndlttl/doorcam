
CC=/usr/bin/g++

SRC_DIR=./src

SRCS=$(SRC_DIR)/main.cpp \
     $(SRC_DIR)/socket.cpp \
     $(SRC_DIR)/server.cpp

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
	-lopencv_shape \
	-lopencv_stitching \
	-lopencv_superres \
	-lopencv_videoio \
	-lopencv_video \
	-lopencv_videostab

CFLAGS= -std=c++11 -I$(INC_DIR) -I$(CV_INC_DIR) -L$(CV_LINK_DIR) $(CVLIBS)

doorcam: $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) -o doorcam
