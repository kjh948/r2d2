  

#include <AFMotor.h>

AF_DCMotor motorR(2, MOTOR12_64KHZ);
AF_DCMotor motorL(1, MOTOR12_64KHZ);


int motorSpeed = 105;//96;//changed to 5V for motor power



///////////////////////////////////////////////////////////////
void initMotors()
{
  //TCCR1B = TCCR1B  & 0b11111000 | 0x01;
  motorL.setSpeed(motorSpeed);
  motorR.setSpeed(motorSpeed);
}


void setVel(int v)
{
  motorSpeed = v;
  motorL.setSpeed(motorSpeed);
  motorR.setSpeed(motorSpeed);
}
void moveLeft()
{
  motorL.run(FORWARD); 
  motorR.run(BACKWARD); 
}
void moveRight()
{
  motorL.run(BACKWARD); 
  motorR.run(FORWARD); 
}
void moveBackward()
{
   
  motorL.run(BACKWARD); 
  motorR.run(BACKWARD);
}
void moveForward()
{
  motorL.run(FORWARD); 
  motorR.run(FORWARD); 
}
void moveStop()
{
  motorL.run(RELEASE); 
  motorR.run(RELEASE); 
}

void testMotors()
{

  if (Serial.available() > 0) {
    int inByte = Serial.read();
    int speed; // Local variable

    switch (inByte) {

      //______________Motor 1______________

    case '1': // Motor 1 Forward
      motorL.run(FORWARD); 
      Serial.println("Motor 1 Forward");
      Serial.println("   ");
      break;

    case '2': // Motor 1 Stop (Freespin)
      motorL.run(RELEASE); 
      Serial.println("Motor 1 Stop");
      Serial.println("   ");
      break;

    case '3': // Motor 1 Reverse
      motorL.run(BACKWARD); 
      Serial.println("Motor 1 Reverse");
      Serial.println("   ");
      break;

      //______________Motor 2______________

    case '4': // Motor 2 Forward
      motorR.run(FORWARD); 
      Serial.println("Motor 2 Forward");
      Serial.println("   ");
      break;

    case '5': // Motor 1 Stop (Freespin)
      motorR.run(RELEASE); 
      Serial.println("Motor 2 Stop");
      Serial.println("   ");
      break;

    case '6': // Motor 2 Reverse
      motorR.run(BACKWARD); 
      Serial.println("Motor 2 Reverse");
      Serial.println("   ");
      break;

    default:
      break;
    }
  }

}
void moveMotor(int left, int right)
{
  if(left==0 && right==0)
  {
    moveStop();
    return; 
  }
  if(left>0) motorL.run(FORWARD); 
  else motorL.run(BACKWARD);

  if(right>0) motorR.run(FORWARD); 
  else motorR.run(BACKWARD);

  motorL.setSpeed(abs(left));
  motorR.setSpeed(abs(right));
}
