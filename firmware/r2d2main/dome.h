
#include <AFMotor.h>

AF_DCMotor motorDome(3, MOTOR34_64KHZ);

int motorSpeedDome = 64;//changed to 5V for motor power

enum PinAssignments {
  encoderPinA = 19,
  encoderPinB = 18,
  clearButton = 17
};

volatile int encoderPos = 0;
unsigned int lastReportedPos = 1;

boolean A_set = false;
boolean B_set = false;


#include "protothreads.h"

pt ptDome;
int domeThread(struct pt* pt) {
  PT_BEGIN(pt);

  // Loop forever
  for(;;) {
    if (buttonState == HIGH) {
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      PT_SLEEP(pt, 200);
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      PT_SLEEP(pt, 100);
    } else {
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      PT_YIELD(pt);
    }
  }

  PT_END(pt);
}



// Interrupt on A changing state
void doEncoderA(){
  // Test transition
  A_set = digitalRead(encoderPinA) == HIGH;
  // and adjust counter + if A leads B
  encoderPos += (A_set != B_set) ? +1 : -1;
}

// Interrupt on B changing state
void doEncoderB(){
  // Test transition
  B_set = digitalRead(encoderPinB) == HIGH;
  // and adjust counter + if B follows A
  encoderPos += (A_set == B_set) ? +1 : -1;
}

void enableDomeEncoder()
{
// encoder pin on interrupt 0 (pin 19)
  attachInterrupt(digitalPinToInterrupt(encoderPinA), doEncoderA, CHANGE);
// encoder pin on interrupt 1 (pin 18)
  attachInterrupt(digitalPinToInterrupt(encoderPinB), doEncoderB, CHANGE);
}
void disableDomeEncoder()
{
// encoder pin on interrupt 0 (pin 19)
  detachInterrupt(4);
// encoder pin on interrupt 1 (pin 18)
  detachInterrupt(5);
}

void domeCW()
{
  motorDome.run(BACKWARD); 
}
void domeCCW()
{
  motorDome.run(FORWARD); 
}
void domeBreak()
{
  motorDome.run(RELEASE);
}

void domeSetPosHome(int pos)
{
  //enableDomeEncoder();
  /*if(encoderPos<0)
  {
    
    while(digitalRead(clearButton) != HIGH)
    {
      domeCW();
      Serial.println("HomingNeg");
    }
    encoderPos = 0;
  }
  else if(encoderPos>0)
  {
    
    while(digitalRead(clearButton) != HIGH)
    {
      domeCCW();
      Serial.println("HomingPos");
    }
    encoderPos = 0;
  }*/
  
  if(pos>=0 && pos>encoderPos)
  {
    domeCW();
    
    //Serial.println(encoderPos-pos, DEC);
    while(abs(encoderPos-pos)>0);
      //Serial.println(encoderPos);
    domeBreak();    
  }
  else if(pos<0 && pos<encoderPos)
  {
    domeCCW();
    //Serial.println(encoderPos-pos, DEC);
    while(abs(encoderPos-pos)>0);
      //Serial.println(encoderPos);
    domeBreak();  
  }
  /*Serial.print("  #### Reached target  delta ===>  ");
  Serial.println(encoderPos-pos, DEC);
  Serial.print("  #### Current Pos ===>  ");
  Serial.println(encoderPos, DEC);
  
  disableDomeEncoder();
*/
}


void domeEncoderTest(){ 
  if (lastReportedPos != encoderPos) {
    Serial.print("Index: ");
    Serial.print(encoderPos, DEC);
    Serial.println();
    lastReportedPos = encoderPos;
  }
  if (digitalRead(clearButton) == HIGH)  {
    Serial.println("Detected zero position");
    encoderPos = 0;
  }
}
void domeHoming()
{  
  domeCW();
  delay(1000);
  domeCCW();  
  while(digitalRead(clearButton) != HIGH)
      Serial.println(encoderPos);
  encoderPos = 0;
  domeBreak();
}

void domeMotorTest()
{
  domeSetPosHome(10);
  delay(1000);
  domeSetPosHome(-10);
  delay(1000);
  //domeSetPosHome(5);
  //delay(100);
  //domeHoming();
  /*if (Serial.available() > 0) {
    int pos = Serial.read();
    Serial.println("SetPose");
    Serial.println(pos,DEC);
    domeSetPos(pos);
  }*/

  /*domeCW();
  Serial.println("Motor Dome Left");
  Serial.println("   ");
  delay(500);
  domeBreak();
  Serial.println("Motor Dome Stop");
  Serial.println("   ");
  delay(500);
  domeCCW();
  Serial.println("Motor Dome Right");
  Serial.println("   ");
  delay(500);
  domeBreak();
  Serial.println("Motor Dome Stop");
  Serial.println("   ");*/
}


void initDome()
{
  motorDome.setSpeed(motorSpeedDome);
  
  pinMode(encoderPinA, INPUT); 
  pinMode(encoderPinB, INPUT); 
  pinMode(clearButton, INPUT);
  
  digitalWrite(encoderPinA, HIGH);  // turn on pullup resistor
  digitalWrite(encoderPinB, HIGH);  // turn on pullup resistor
  digitalWrite(clearButton, HIGH);
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  pinMode(clearButton, INPUT_PULLUP);

  enableDomeEncoder();
  domeHoming();
  //disableDomeEncoder();
}
