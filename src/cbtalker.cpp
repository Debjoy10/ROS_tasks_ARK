#include "ros/ros.h"
#include "std_msgs/Bool.h"
#include <sstream>

#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include "checkerboard/board_pose.h"
#include "checkerboard/danger_region.h"

#include<iostream>
#include <stdio.h>
#include <stdlib.h>  

using namespace std;
using namespace cv;

bool InDanger(int x, int y)
{
	if(x>300 && x<500 && y>300 && y<500)
		return true;
	else
		return false;
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "cbtalker");
  ros::NodeHandle n;

  ros::Publisher Pos = n.advertise<checkerboard::board_pose>("check_pose", 1000);
  ros::Rate loop_rate(10);

  bool warn = true;
  int count = 0;
  namedWindow("win",0);
  VideoCapture Video("new.avi"); 
  while (ros::ok())
  {  
	checkerboard::board_pose msg;
	Mat img, gray;
	
	Size patternsize(8,5);
	Video>>img;
	vector<Point2f> corners;
	cvtColor(img, gray, cv::COLOR_RGB2GRAY);
	bool patternfound = findChessboardCorners(gray, patternsize, corners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE+CALIB_CB_FAST_CHECK);

	if(patternfound)
		cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

	drawChessboardCorners(img, patternsize, Mat(corners), patternfound);

	Point2f center; 
	msg.x=(corners[19].x+corners[20].x)/2.0;
	msg.y=(corners[19].y+corners[20].y)/2.0;
	ROS_INFO("%f %f", msg.x,msg.y);
	Pos.publish(msg);
	
	ros::ServiceClient client = n.serviceClient<checkerboard::danger_region>("danger_region");

	checkerboard::danger_region srv;

	srv.request.warn = InDanger(int(msg.x),int(msg.y));
		

	if (client.call(srv))
  	{
    		ROS_INFO("Output: %s", srv.response.out);
 	}
  	else
  	{
    		ROS_ERROR("Failed to call service danger_region");
    		return 0;
	}
	imshow("win",img);
	waitKey(1);

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
  }


  return 0;
}
