

int knockSensor = A15;               
int THRESHOLD = 64;
int sensorReading;

void initKnock()
{}

bool checkKnock(void)
{
  sensorReading = analogRead(knockSensor);

  // if the sensor reading is greater than the threshold:
  if (sensorReading >= THRESHOLD) return true;
  else return false;
  
}

void testKnock(void)
{
    if(checkKnock()) Serial.println("Knock");
    delay(10);  
}
