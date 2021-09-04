
int radarPin = 22;

void initRadar()
{
  pinMode(radarPin, INPUT);
}

bool checkRadar(void)
{
  if (digitalRead(radarPin)) return true;
  else return false;
  
}

void testRadar(void)
{
    if(checkRadar()) Serial.println("Radar Detected");
    else Serial.println("Not Detected");
    delay(10);  
}
