
int sePin = 23;

void initSoundEvent()
{
  pinMode(sePin, INPUT);
}

bool checkSoundEvent(void)
{
  if (digitalRead(sePin)) return true;
  else return false;
  
}

void testSoundEvent(void)
{
    if(checkSoundEvent()) Serial.println("Sound Detected");
    else Serial.println("Not Detected");
    delay(10);  
}
