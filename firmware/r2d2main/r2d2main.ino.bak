#import <Arduino.h>
#include "printf.h"

#define DEBUG

#include "motor.h"
#include "piezo.h"
#include "dome.h"
#include "led.h"
#include "knock.h"
#include "tof.h"
#include "radar.h"
#include "sound_event.h"

#include "packet.h"

#include "brain.h"

#define TEST


#ifdef TEST
int testModule = 0;
void testsuite()
{
  // Initialize the Serial interface:

  if (testModule == 0)
  {
    rover();
    return;
  }
  else 
  if (testModule == 1)
  {
    testMotors();
    return;
  }
  else if (testModule == 2)
  {
    //Serial.println("Dome test\n");
    domeMotorTest();
    //domeEncoderTest();
    //return;
  }
  else if (testModule == 3)
  {
    squeak();
    delay(500);
    catcall();
    delay(500);
    ohhh();
    delay(500);
    laugh();
    delay(500);
    closeEncounters();
    delay(500);
    laugh();
    delay(500);
    waka();
    delay(500);
    r2D2();
    delay(500);
    ariel();
    delay(3000);
  }
  else if (testModule == 4)
  {
    testLed();
  }
  else if (testModule == 6)
  {
    testKnock();
  }
  else if (testModule == 7)
  {
    testToF();
  }

  else if (testModule == 8)
  {
    testRadar();
  }
  else if (testModule == 9)
  {
    testSoundEvent();
  }  
}
#endif


#define COMMAND_RATE 10 //hz
#define SENSOR_RATE 10 //hz
#define LED_RATE 30 //hz
#define DEBUG_RATE 5


byte ledCmd = 0;
byte ledRate = 30;

int16_t tofdistance;
byte radar;
byte knock;
byte mic;

void setup(void)
{
  Serial.begin(115200);      //Set Baud Rate
  printf_begin();
  Serial.println("Init Started");
#ifdef TEST
  if(testModule==2) initDome();
#else
  initDome();
#endif  
  initMotors();
  initPiezo();
  
  initLed();
  initToF();
  initRadar();
  initSoundEvent();
  Serial.println("Init Completed");

}




void loop() {
#ifdef TEST
  testsuite();
#else
  static unsigned long prev_sensor_time = 0;
  static unsigned long prev_command_time = 0;
  static unsigned long prev_led_time = 0;


  if ((millis() - prev_sensor_time) >= (1000 / SENSOR_RATE))
  {
    tofdistance = measureToF();
    radar = checkRadar();
    knock = 0;mic = 0;
    prepareStatusBuf(tofdistance,radar,knock,mic);
    Serial.write(statusBuf,sizeof(statusBuf));
  }

  if ((millis() - prev_command_time) >= (1000 / COMMAND_RATE))
  {
    domeSetPosHome(3);
    prev_command_time = millis();
  }

  if ((millis() - prev_led_time) >= (1000 / ledRate))
  {
    if (ledCmd > 0)
    {
      showLed(prev_led_time % 256, (255 - prev_led_time % 256));
    }
    prev_led_time = millis();
  }



#endif
}
