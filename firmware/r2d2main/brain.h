#import <Arduino.h>
#include "printf.h"

#define COMMAND_RATE 10 //hz
#define SENSOR_RATE 15 //hz
#define LED_RATE 5 //hz


void rover()
{
  int dist[3];
  int sound_event;
  int ledCmd;

  int distThre = 500;
  enum State {MOTION_STOP=0, MOTION_FORWARD, MOTION_LEFT, MOTION_RIGHT, MOTION_BACKWARD} ;

  State currentState;
  
  static unsigned long prev_sensor_time = 0;
  static unsigned long prev_command_time = 0;
  static unsigned long prev_led_time = 0;

  initDome();
  
  setVel(64);
  moveForward();currentState = MOTION_FORWARD;
  
  while(1)
  {
    //printf(".");
    if ((millis() - prev_command_time) >= (1000 / COMMAND_RATE))
    {
      if(dist[1]<distThre)
      {
        if(dist[0]<dist[2])
        {
          moveLeft();currentState = MOTION_LEFT;
          //domeSetPosHome(-1);
#ifdef DEBUG
          printf("turn left\n");
#endif
        }
        else
        {
          moveRight();currentState = MOTION_RIGHT;
          //domeSetPosHome(1);
#ifdef DEBUG
          printf("turn right\n");
#endif

        }
      }
      else
      {
        moveForward();currentState = MOTION_FORWARD;
#ifdef DEBUG
        printf("go forward\n");
#endif
      }
#ifdef DEBUG
      printf("\t\t\tSTATE=\t%d\n",currentState);
#endif
      prev_command_time = millis();
    }
    if ((millis() - prev_sensor_time) >= (1000 / SENSOR_RATE))
    {
      getDist(dist);
      sound_event = checkSoundEvent();
#ifdef DEBUG
      //printf("TOF: \t %d,%d,%d\n",dist[0],dist[1],dist[2]);
#endif
        
      prev_sensor_time = millis();
    }

    if ((millis() - prev_led_time) >= (1000 / LED_RATE))
    {
      if(sound_event) ledCmd++;
      if (ledCmd > 0)
      {
        showLed(prev_led_time % 256, (255 - prev_led_time % 256));
      }
      prev_led_time = millis();
    }
    
  }
  
  

}
