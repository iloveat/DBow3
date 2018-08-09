INCLUDES += -I/usr/local/include
INCLUDES += -I/usr/local/include/opencv
INCLUDES += -I/usr/local/include/opencv2
INCLUDES += -I../src

SOURCES += ../utils/orb/ORBextractor.cc
SOURCES += ../src/*.cpp
SOURCES += ../src/*.c
SOURCES += ../utils/demo_general.cpp

LIBS += -L/usr/local/lib -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_photo -lopencv_imgcodecs -lopencv_video -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_core

XXFLAGS += -std=c++11

CXX = g++


demo_general : 
	$(CXX) -o demo_general $(INCLUDES) $(SOURCES) $(LIBS) $(XXFLAGS)


clean :
	rm -f demo_general small_db.yml.gz


.PHONY : clean






















