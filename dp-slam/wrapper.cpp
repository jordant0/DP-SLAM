#include <cmath>
#include "ros/node.h"
#include "laser_scan/LaserScan.h"
#include "robot_msgs/PoseStamped.h"
#include <vector>
#include <string>
#include "rosrecord/Player.h"
using std::vector;
using std::string;

FILE *clog = NULL;
double odom_x = 0, odom_y = 0, odom_th = 0;

void odom_callback(string name, robot_msgs::PoseStamped * odom, ros::Time t, ros::Time t_no_use, void* n)
{
	printf("odom cb\n");
	odom_th = 2 * asin(odom->pose.orientation.z);
	odom_x = odom->pose.position.x;
	odom_y = odom->pose.position.y;
	/*
	fprintf(clog, "Odometry %f %f %f\n",
		odom->pose.position.x, odom->pose.position.y, th);
	*/
}

void scan_callback(string name, laser_scan::LaserScan* scan, ros::Time t, ros::Time t_no_use, void* n)
{
	printf("scan cb\n");
	// if we want to use this with a non-SICK laser, we need this code to
	// be smarter and subsample / upsample
	// also, the STAIR1A laser is upside down, so I'm inverting it here...
	fprintf(clog, "Odometry %f %f %f\n", odom_x, odom_y, odom_th);
	fprintf(clog, "Laser 181 ");
	for (uint32_t i = 0; i < scan->get_ranges_size(); i++)
		fprintf(clog, "%.3f ", scan->ranges[scan->get_ranges_size() - i - 1]);//scan->ranges[scan->get_ranges_size() - i - 1]);
		fprintf(clog, "\n");
	}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("usage: genlog_dpslam FILE1\n"
			"where FILE1 is a bag file containing at least /odom and /scan messages.\n");
		return 1;
	}
	ros::record::MultiPlayer player;
	vector<string> files;
	for (int i = 1; i < argc; i++)
	files.push_back(argv[i]);
	player.open(files, ros::Time());
	player.addHandler<robot_msgs::PoseStamped>(string("odom"), &odom_callback, NULL);
	player.addHandler<laser_scan::LaserScan>(string("scan"), &scan_callback, NULL);
	clog = fopen("dpslam.log", "w");
	while(player.nextMsg()) { }
	fclose(clog);
	return 0;
}
