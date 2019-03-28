#include "ros/ros.h"
#include "std_msgs/Bool.h"
#include "checkerboard/board_pose.h"
#include "checkerboard/danger_region.h"
#include "std_msgs/String.h"
#include <sstream>
#include <bits/stdc++.h>

void chatter1(const checkerboard::board_pose::ConstPtr& msg)
{
  ROS_INFO("I heard: [%f,%f]", msg->x, msg->y);
}

bool danger(checkerboard::danger_region::Request  &req, checkerboard::danger_region::Response &res)
{
	std_msgs::String msg;
	std::stringstream ss;
	if(req.warn)
		ss << "In danger!!";
	else
		ss << "Safe";
	res.out = ss.str();
	ROS_INFO("request: input=%d", (int)req.warn);
	ROS_INFO("response: %s", res.out);
	return true;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "cblistener");
  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("check_pose", 1000, chatter1);

  ros::ServiceServer service = n.advertiseService("danger_region", danger);

  ros::spin();

  return 0;
}


