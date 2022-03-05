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

std_msgs::String str_msg;
std_msgs::Int16MultiArray sensor_msg;
std_msgs::Int16 led_msg;

//Command list
float g_req_linear_vel_x = 0;
float g_req_linear_vel_y = 0;
float g_req_angular_vel_z = 0;
float pwm_scale = 70;
float base_size = 20.;
int g_motor_left = 0;
int g_motor_right = 0;

unsigned long g_prev_command_time = 0;
int g_led0=0;
int g_led1=0;
void motorCallback(const geometry_msgs::Twist& cmd_msg)
{
  g_motor_left = (cmd_msg.linear.x - cmd_msg.angular.z)*pwm_scale;
  g_motor_right = (cmd_msg.linear.x + cmd_msg.angular.z)*pwm_scale;
  
  g_motor_left = constrain(g_motor_left, -200, 200);
  g_motor_right = constrain(g_motor_right, -200, 200);

  moveMotor(g_motor_left,g_motor_right);

  char ll[100];
  sprintf(ll,"cmd_vel callback=\t%d",g_motor_left,g_motor_right);
  nh.loginfo(ll);
 
}
void ledCallback(const std_msgs::Int16& led_msg)
{
  g_led0 = led_msg.data&0x0f;
  g_led1 = led_msg.data>>4;

  char ll[100];
  sprintf(ll,"led callback=\t%d\t%d",g_led0,g_led1);
  nh.loginfo(ll);
  
}

ros::Subscriber<geometry_msgs::Twist> cmd_sub("cmd_vel", motorCallback);
ros::Subscriber<std_msgs::Int16> led_sub("led", ledCallback);


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


#define COMMAND_RATE 10 //hz
#define SENSOR_RATE 30 //hz
#define PUBLISH_RATE 10 //hz
#define LED_RATE 10 //hz

void setup(void)
{
  //Serial.begin(115200);      //Set Baud Rate
  //printf_begin();
  //printf("Init Started\n");

  initDome();
  initMotors();
  initPiezo();
  
  initLed();
  //printf("LED\n");
  initToF();
  //printf("TOF\n");
  initRadar();
  initSoundEvent();
  //Serial.println("Init Completed");
  //printf("Module Init End\n");

  
  nh.initNode();
  nh.getHardware()->setBaud(57600);
  nh.subscribe(cmd_sub);
  nh.subscribe(led_sub);

  //tof: 0~2, sound_event: 3, piezo: 4, dara: 5
  sensor_msg.data =  (int*)malloc(sizeof(int) * 6);
  sensor_msg.data_length =  6;
  nh.advertise(sensor_pub);
  
  while (!nh.connected())
  {
      nh.spinOnce();
  }
  nh.loginfo("LINOBASE CONNECTED");
  //printf("LINOBASE CONNECTED\n");
  delay(1);
    
}

void loop() {

  static unsigned long prev_sensor_time = 0;
  static unsigned long prev_command_time = 0;
  static unsigned long prev_pub_time = 0;
  static unsigned long prev_led_time = 0;


  if ((millis() - prev_sensor_time) >= (1000 / SENSOR_RATE))
  {
    getDist(dist);
    if(event_latch)
    {
      sound_event = checkSoundEvent()|sound_event;
      touch_event = checkKnock()|touch_event;
      radar_event = checkRadar()|radar_event;
    }
    else
    {
      sound_event = checkSoundEvent();
      touch_event = checkKnock();
      radar_event = checkRadar(); 
      event_latch = 1;     
    }

    //nh.loginfo("sensor check");

    prev_sensor_time = millis();
  }

  if ((millis() - prev_command_time) >= (1000 / COMMAND_RATE))
  {
    //domeSetPos(3);
    //nh.loginfo("command processing");
    
    prev_command_time = millis();
  }

  if ((millis() - prev_pub_time) >= (1000 / PUBLISH_RATE))
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
