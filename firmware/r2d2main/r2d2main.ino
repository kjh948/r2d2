#import <Arduino.h>
//#include "printf.h"

#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int16MultiArray.h>
#include <std_msgs/Int16.h>
#include "geometry_msgs/Twist.h"

#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int16MultiArray.h>
#include <std_msgs/Int16.h>
#include "geometry_msgs/Twist.h"

#define USE_MPU6050_IMU
//header file for imu
#include "lino_msgs/Imu.h"
#include "Imu.h"

lino_msgs::Imu raw_imu_msg;
ros::Publisher raw_imu_pub("raw_imu", &raw_imu_msg);

#define DEBUG

#include "motor.h"
#include "piezo.h"
#include "dome.h"
#include "led.h"
#include "knock.h"
#include "tof.h"
#include "radar.h"
#include "sound_event.h"

#include "brain.h"

ros::NodeHandle nh;
float g_req_linear_vel_x = 0;

std_msgs::String str_msg;
std_msgs::Int16MultiArray sensor_msg;
std_msgs::Int16 led_msg;

//Command list
float g_req_linear_vel_y = 0;
float g_req_angular_vel_z = 0;
float g_pwm_scale = 120.;
int g_motor_left = 0;
int g_motor_right = 0;

unsigned long g_prev_command_time = 0;
int g_led0=0;
int g_led1=0;
float g_wheel_bias = 0.18*6.;
float g_pwm_comp = 0.85;//reduce the pwm of the left wheel
void commandCallback(const geometry_msgs::Twist& cmd_msg)
{
    //callback function every time linear and angular speed is received from 'cmd_vel' topic
    //this callback function receives cmd_msg object where linear and angular speed are stored
    // g_motor_left = (cmd_msg.linear.x - cmd_msg.angular.z)*pwm_scale;
    // g_motor_right = (cmd_msg.linear.x + cmd_msg.angular.z)*pwm_scale/;

    g_motor_left = ((cmd_msg.linear.x - (cmd_msg.angular.z * g_wheel_bias / 2.0)) * g_pwm_scale * g_pwm_comp);
    g_motor_right = ((cmd_msg.linear.x + (cmd_msg.angular.z * g_wheel_bias / 2.0))* g_pwm_scale);
    
    g_motor_left = constrain(g_motor_left, -200, 200);
    g_motor_right = constrain(g_motor_right, -200, 200);
    g_prev_command_time = millis();
}

void moveBase()
{
  moveMotor(g_motor_left,g_motor_right);

  // char ll[100];
  // sprintf(ll,"cmd_vel callback=\t%d",g_motor_left,g_motor_right);
  // nh.loginfo(ll);

}

void stopBase()
{
    g_motor_left = 0;
    g_motor_right = 0;
}

void ledCallback(const std_msgs::Int16& led_msg)
{
  g_led0 = led_msg.data&0x0f;
  g_led1 = led_msg.data>>4;

  char ll[100];
  sprintf(ll,"led callback=\t%d\t%d",g_led0,g_led1);
  nh.loginfo(ll);
  
}

int g_dome_pose = 0;
void domeCallback(const std_msgs::Int16& dome_msg)
{
  g_dome_pose = dome_msg.data;

  char ll[100];
  sprintf(ll,"dome callback=\t%d",g_dome_pose);
  nh.loginfo(ll);
  
}

ros::Subscriber<geometry_msgs::Twist> cmd_sub("cmd_vel", commandCallback);
ros::Subscriber<std_msgs::Int16> led_sub("led", ledCallback);
ros::Subscriber<std_msgs::Int16> dome_sub("dome", domeCallback);


ros::Publisher sensor_pub("/sensor", &sensor_msg);
int dist[3];
int sound_event=0;
int event_latch=0;
int touch_event=0;
int radar_event=0;
unsigned long g_prev_sensor_time = 0;

void pubishSensor()
{
  //tof: 0~2, sound_event: 3, piezo: 4, radar: 5
  sensor_msg.data[0] = dist[0];sensor_msg.data[1] = dist[1];sensor_msg.data[2] = dist[2];
  sensor_msg.data[3] = sound_event;
  sensor_msg.data[4] = touch_event;
  sensor_msg.data[5] = radar_event;
  
  sensor_pub.publish(&sensor_msg);

  event_latch = 0;
  g_prev_sensor_time = millis();
}

void publishIMU()
{
    //pass accelerometer data to imu object
    raw_imu_msg.linear_acceleration = readAccelerometer();    

    //pass gyroscope data to imu object
    raw_imu_msg.angular_velocity = readGyroscope();

    //pass accelerometer data to imu object
    raw_imu_msg.magnetic_field = readMagnetometer();

    //publish raw_imu_msg object to ROS
    raw_imu_pub.publish(&raw_imu_msg);
}


#define COMMAND_RATE 10 //hz
#define SENSOR_RATE 30 //hz
#define PUBLISH_RATE 10 //hz
#define LED_RATE 10 //hz
#define IMU_PUBLISH_RATE 10 //hz


void setup(void)
{
  //Serial.begin(115200);      //Set Baud Rate
  //printf_begin();
  //printf("Init Started\n");

  initDome();
  initMotors();
  initPiezo();
  
  initLed();
  initRadar();
  //Serial.println("Init Completed");
  //printf("Module Init End\n");

  
  nh.initNode();
  nh.getHardware()->setBaud(57600);
  nh.subscribe(cmd_sub);
  nh.subscribe(led_sub);
  nh.subscribe(dome_sub);


  nh.advertise(raw_imu_pub);

  //tof: 0~2, sound_event: 3, piezo: 4, dara: 5
  sensor_msg.data =  (int*)malloc(sizeof(int) * 6);
  sensor_msg.data_length =  6;
  nh.advertise(sensor_pub);
  
  while (!nh.connected())
  {
      nh.spinOnce();
  }
  nh.loginfo("R2D2 CONNECTED");
  delay(1);
    
}

void loop() {

  static unsigned long prev_sensor_time = 0;
  static unsigned long prev_command_time = 0;
  static unsigned long prev_pub_time = 0;
  static unsigned long prev_led_time = 0;


  static unsigned long prev_control_time = 0;
  static unsigned long publish_vel_time = 0;
  static unsigned long prev_imu_time = 0;
  static unsigned long prev_debug_time = 0;
  static bool imu_is_initialized;


  //this block drives the robot based on defined rate
  if ((millis() - prev_control_time) >= (1000 / COMMAND_RATE))
  {
      moveBase();
      domeSetPos(g_dome_pose);
      prev_control_time = millis();
  }

  //this block stops the motor when no command is received
  if ((millis() - g_prev_command_time) >= 3000)
  {
      stopBase();
  }

  //this block publishes the IMU data based on defined rate
  if ((millis() - prev_imu_time) >= (1000 / IMU_PUBLISH_RATE))
  {
      //sanity check if the IMU is connected
      if (!imu_is_initialized)
      {
          imu_is_initialized = initIMU();

          if(imu_is_initialized)
              nh.loginfo("IMU Initialized");
          else
              nh.logfatal("IMU failed to initialize. Check your IMU connection.");
      }
      else
      {
          publishIMU();
          //nh.loginfo("IMU Published");
      }
      prev_imu_time = millis();
  }

  if ((millis() - prev_pub_time) >= (1000 / SENSOR_RATE))
  {
    pubishSensor();

    //nh.loginfo("pub sensor");
    
    prev_pub_time = millis();
  }

  if ((millis() - prev_led_time) >= (1000 / LED_RATE))
  {
    showLed(g_led0,g_led1);

    //nh.loginfo("led processing");
    
    prev_led_time = millis();
  }

  nh.spinOnce();
}
