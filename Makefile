#challenge3: challenge3.cpp
#	g++ challenge3.cpp -ggdb `pkg-config --cflags opencv` `pkg-config --libs opencv` -lopencv_highgui -lopencv_core -lopencv_imgproc -o challenge3 -std=c++11
export OPENNI2_INCLUDE=/usr/include/openni2 
opencamera: opencamera.cpp
	g++ -I /home/ubuntu/OpenNI-Linux-Arm-2.2/Include -I /opt/ros/indigo/include -L /usr/local/cuda-6.5/lib -L /opt/ros/indigo/lib -L /usr/lib -L /usr/include/openni2 color_detect.cpp ros_wrapper.cpp opencamera.cpp -ggdb `pkg-config --cflags opencv` `pkg-config --libs opencv` -lopencv_highgui -lopencv_core -lOpenNI2 -lroscpp -lrosconsole -lrostime -lroscpp_serialization -o opencamera  -std=c++11
clean:
	rm -f challenge3
	rm -f opencamera
