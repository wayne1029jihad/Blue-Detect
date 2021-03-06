/********************
This code for NTHU Nvision Team 
*********************/
// STL Header
#include <iostream>
#include <vector>

//Ros Header
#include "ros/ros.h"
#include "std_msgs/Float32MultiArray.h"
#include <sstream>
#include "stdio.h"   
// OpenCV Header
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
  
// OpenNI Header
#include <OpenNI.h>
  
// Color detection
#include "color_detect.hpp"

// ROS Wrapper
#include "ros_wrapper.hpp"

// namespace
using namespace std;
using namespace openni;
using namespace cv;
void onMouse(int event,int x,int y,int flags,void* param);
CvPoint VertexOne,VertexThree;
CvPoint mouseC;
//ros::Publisher * chatter_pub;
//ros::Rate loop_rate(10);  

RosWrapper * g_ros_wrapper;

int main( int argc, char **argv )
{
VertexOne=cvPoint(0,0);
VertexThree=cvPoint(0,0);
mouseC = cvPoint(-100,-100);

// 0. initial ros
    //ros::init(argc, argv, "Blue_Detect");      
    //ros::NodeHandle n;
    //chatter_pub = new ros::Publisher;
    //*chatter_pub = n.advertise<std_msgs::Float32MultiArray>("chatter",1000);
   g_ros_wrapper = new RosWrapper(argc, argv, "blue_detect", "chatter");
// 1. initial openni

  if(OpenNI::initialize() != STATUS_OK)
  {
    cerr << "OpenNI Initial Error: " 
         << OpenNI::getExtendedError() << endl;
    return -1;
  }
  
  // 2. Open Device
  Device mDevice;
  if( mDevice.open( ANY_DEVICE ) != STATUS_OK )
  {
    cerr << "Can't Open Device: " 
         << OpenNI::getExtendedError() << endl;
    return -1;
  }
  
  // 3. Create depth stream
    /*
  VideoStream mDepthStream;
  if( mDevice.hasSensor( SENSOR_DEPTH ) )
  {
    if( mDepthStream.create( mDevice, SENSOR_DEPTH ) == STATUS_OK )
    {
      // 3a. set video mode
      VideoMode mMode;
      mMode.setResolution( 640, 480 );
      mMode.setFps( 30 );
      mMode.setPixelFormat( PIXEL_FORMAT_DEPTH_1_MM );
  
      if( mDepthStream.setVideoMode( mMode) != STATUS_OK )
      {
        cout << "Can't apply VideoMode: "
             << OpenNI::getExtendedError() << endl;
      }
    }
    else
    {
      cerr << "Can't create depth stream on device: "
           << OpenNI::getExtendedError() << endl;
      return -1;
    }
  }
  else
  {
    cerr << "ERROR: This device does not have depth sensor" << endl;
    return -1;
  }
  */
  // 4. Create color stream
  VideoStream mColorStream;
  if( mDevice.hasSensor( SENSOR_COLOR ) )
  {
    if( mColorStream.create( mDevice, SENSOR_COLOR ) == STATUS_OK )
    {
      // 4a. set video mode
     VideoMode mMode;
     cout<< mMode.getPixelFormat()<<endl;
     cout<< mMode.getResolutionX()<<endl;

   mMode.setResolution( 1280,960  );
//      mMode.setResolution(640,480);
      mMode.setFps( 30 );
      mMode.setPixelFormat( PIXEL_FORMAT_JPEG );
      mMode.setPixelFormat(PIXEL_FORMAT_RGB888);
      cout<< mMode.getPixelFormat()<<endl;
      cout<< mMode.getResolutionX()<<endl;
      if( mColorStream.setVideoMode( mMode) != STATUS_OK )
      {
        cout << "Can't apply VideoMode: " 
             << OpenNI::getExtendedError() << endl;
      }
    /*
          // 4b. image registration
        if( mDevice.isImageRegistrationModeSupported(
               IMAGE_REGISTRATION_DEPTH_TO_COLOR ) )
        {
         mDevice.setImageRegistrationMode( IMAGE_REGISTRATION_DEPTH_TO_COLOR );
        }
    */                              
  
    }
    else
    {
      cerr << "Can't create color stream on device: "
           << OpenNI::getExtendedError() << endl;
      return -1;
    }
  }
  // 5. create OpenCV Window
 // cv::namedWindow( "Depth Image",  CV_WINDOW_NORMAL );
  cv::namedWindow( "Color Image",  CV_WINDOW_NORMAL );
cvSetMouseCallback("Color Image",onMouse,NULL);//????callback??  
  // 6. start
  VideoFrameRef  mColorFrame;
 // VideoFrameRef  mDepthFrame;
 // mDepthStream.start();
  mColorStream.start();
 // int iMaxDepth = mDepthStream.getMaxPixelValue();
  while( true )
  {
    // 7. check is color stream is available
    if( mColorStream.isValid() )
    {
      // 7a. get color frame
      if( mColorStream.readFrame( &mColorFrame ) == STATUS_OK )
      {
        // 7b. convert data to OpenCV format
        const cv::Mat mImageRGB(
                mColorFrame.getHeight(), mColorFrame.getWidth(),
                CV_8UC3, (void*)mColorFrame.getData() );
        // 7c. convert form RGB to BGR
        cv::Mat cImageBGR;
        cv::cvtColor( mImageRGB, cImageBGR, CV_RGB2BGR );
        
        // Detect blue
        std::vector<cv::Rect> rects;
        color_detect::find_blue(cImageBGR, rects, 
            800, // Minimum area threshold
            cv::Scalar(100, 150, 100), // Blue lower bound
            cv::Scalar(140, 255, 255) // Blue upper bound 
        );
        for(auto rect : rects) {
            cv::rectangle(cImageBGR, rect.tl(), rect.br(), cv::Scalar(0, 255, 0), 2);
        } 
        circle(cImageBGR,mouseC,10,Scalar(0,0,255), 4);    

        // 7d. show image
        cv::imshow( "Color Image", cImageBGR );
      }
    }
  /*
    // 8a. get depth frame
    if( mDepthStream.readFrame( &mDepthFrame ) == STATUS_OK )
    {
      // 8b. convert data to OpenCV format
      const cv::Mat mImageDepth(
                mDepthFrame.getHeight(), mDepthFrame.getWidth(),
                CV_16UC1, (void*)mDepthFrame.getData() );
      // 8c. re-map depth data [0,Max] to [0,255]
      cv::Mat mScaledDepth;
      mImageDepth.convertTo( mScaledDepth, CV_8U, 255.0 / iMaxDepth );
      // 8d. show image
      cv::imshow( "Depth Image", mScaledDepth );
    }
  */
    // 6a. check keyboard
    if( cv::waitKey( 1 ) == 'q' )
      break;
  }
  
  // 9. stop
 // mDepthStream.destroy();
  mColorStream.destroy();
  mDevice.close();
  OpenNI::shutdown();
  
  return 0;
}

void onMouse(int event,int x,int y,int flag,void* param){   
    std_msgs::Float32MultiArray arr;
    if(event==CV_EVENT_LBUTTONDOWN||event==CV_EVENT_RBUTTONDOWN){
        arr.data.clear();
        VertexOne=cvPoint(x,y);
        cout<<x<< " "<<y<<endl;   
        mouseC = cvPoint(x,y);
        arr.data.push_back(100);
        arr.data.push_back(x);
        arr.data.push_back(y);
        arr.data.push_back(0);
        //chatter_pub->publish(arr);
        //ros::spinOnce();
        //loop_rate.sleep();
        g_ros_wrapper->publish<std_msgs::Float32MultiArray>(arr);
    }
}
