#!/usr/bin/env python


import rospy
from roslib import message
from geometry_msgs.msg import Twist
from std_msgs.msg import Int16MultiArray

from connectome import run_once 
import time

class Worm():
    def __init__(self):
        rospy.init_node("worm")
        
        # Set the shutdown function (stop the robot)
        rospy.on_shutdown(self.shutdown)
        self.move_cmd = Twist()
        self.cmd_vel_pub = rospy.Publisher('cmd_vel', Twist, queue_size=5)

        self.move_cmd = Twist()
        self.speed = 1.0

        self.left_rot()
        time.sleep(0.8)
        self.right_rot()
        time.sleep(0.8)
        
        # Subscribe to the sensor data
        self.sensor_sub = rospy.Subscriber('sensor', Int16MultiArray, self.do_move, queue_size=1)

    def setspeed(self, v):
        self.speed = v

    def stop(self):
        rospy.loginfo("motor stop")
        self.move_cmd.angular.z = 0
        self.move_cmd.linear.x = 0
        self.cmd_vel_pub.publish(self.move_cmd)

    def left_rot(self):
        rospy.loginfo("motor left")
        self.move_cmd.angular.z = 1.0
        self.move_cmd.linear.x = 0
        self.cmd_vel_pub.publish(self.move_cmd)
    def right_rot(self):
        rospy.loginfo("motor right")
        self.move_cmd.angular.z = -1.0
        self.move_cmd.linear.x = 0
        self.cmd_vel_pub.publish(self.move_cmd)
    def fwd(self):
        rospy.loginfo("motor fwd")
        self.move_cmd.angular.z = 0
        self.move_cmd.linear.x = self.speed
        self.cmd_vel_pub.publish(self.move_cmd)
    def bwd(self):
        rospy.loginfo("motor bwd")
        self.move_cmd.angular.z = 0
        self.move_cmd.linear.x = -self.speed
        self.cmd_vel_pub.publish(self.move_cmd)



    def do_move(self, msg):
        rospy.loginfo("Sensor data:\t"+str(msg.data))
        accumleft, accumright = run_once(msg.data[1])

        # We turn the wheels according to the motor weight accumulation
        new_speed = abs(accumleft) + abs(accumright)
        if new_speed > 150:
            new_speed = 150
        elif new_speed < 75:
            new_speed = 75
        #rospy.loginfo("Left: ", accumleft, "Right:", accumright, "Speed: ", new_speed)
        
        self.setspeed(new_speed)
        if accumleft == 0 and accumright == 0:
            self.stop()
        elif accumright <= 0 and accumleft < 0:
            self.setspeed(150)
            turnratio = float(accumright) / float(accumleft)
            # print "Turn Ratio: ", turnratio
            if turnratio <= 0.6:
                self.left_rot()
                time.sleep(0.8)
            elif turnratio >= 2:
                self.right_rot()
                time.sleep(0.8)
            self.bwd()
            time.sleep(0.5)
        elif accumright <= 0 and accumleft >= 0:
            self.right_rot()
            time.sleep(.8)
        elif accumright >= 0 and accumleft <= 0:
            self.left_rot()
            time.sleep(.8)
        elif accumright >= 0 and accumleft > 0:
            turnratio = float(accumright) / float(accumleft)
            # print "Turn Ratio: ", turnratio
            if turnratio <= 0.6:
                self.left_rot()
                time.sleep(0.8)
            elif turnratio >= 2:
                self.right_rot()
                time.sleep(0.8)
            self.fwd()
            time.sleep(0.5)
        else:
            self.stop()
    def shutdown(self):
        rospy.loginfo("Stopping the robot...")
        
        # Send an emtpy Twist message to stop the robot
        self.cmd_vel_pub.publish(Twist())
        rospy.sleep(1)

if __name__ == '__main__':
    try:
        worm = Worm()
        rospy.spin()
    except rospy.ROSInterruptException:
        rospy.loginfo("Follower node terminated.")
            
    