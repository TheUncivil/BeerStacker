#!/usr/bin/env python

import rospy
from std_msgs.msg import String
import serial

ser = serial.Serial('/dev/ttyACM0', 9600)

#The data provided by the BeerStacker should be read as follows:
#	a: slowly go backwards
#	b: stay still
#	c: slowly go forwards
#	d: drive normally

def talker():
 while not rospy.is_shutdown():
   data= ser.read(1) 
   rospy.loginfo(data)
   pub.publish(String(data))
   rospy.sleep(0.001)


if __name__ == '__main__':
  try:
    rospy.init_node('BeerStacker_serial')
    pub = rospy.Publisher('BeerStacker_data', String)
    talker()
  except rospy.ROSInterruptException:
    pass
