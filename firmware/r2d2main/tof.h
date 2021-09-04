# if 0
#include "Adafruit_VL53L0X.h"

//#define XSHUT_pin6 not required for address change
#define XSHUT_pin3 32
#define XSHUT_pin2 30
#define XSHUT_pin1 28

//ADDRESS_DEFAULT 0b0101001 or 41
//#define Sensor1_newAddress 41 not required address change
#define Sensor2_newAddress 42
#define Sensor3_newAddress 43


Adafruit_VL53L0X TOF1 = Adafruit_VL53L0X();
Adafruit_VL53L0X TOF2 = Adafruit_VL53L0X();
Adafruit_VL53L0X TOF3 = Adafruit_VL53L0X();

VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;
VL53L0X_RangingMeasurementData_t measure3;


void setID() {
  // all reset
  digitalWrite(XSHUT_pin1, LOW);    
  digitalWrite(XSHUT_pin2, LOW);
  digitalWrite(XSHUT_pin3, LOW);
  delay(10);
  // all unreset
  //digitalWrite(XSHUT_pin1, HIGH);    
  //digitalWrite(XSHUT_pin2, HIGH);
  //digitalWrite(XSHUT_pin3, HIGH);
  //delay(10);

  

  // activating LOX3
  pinMode(XSHUT_pin3, INPUT);//digitalWrite(XSHUT_pin3, HIGH);
  delay(10);

  if(!TOF3.begin(Sensor3_newAddress)) {
    Serial.println(F("Failed to boot first VL53L0X 3"));
    //while(1);
  }
  TOF3.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE);

  // activating LOX2
  pinMode(XSHUT_pin2, INPUT);//digitalWrite(XSHUT_pin2, HIGH);
  delay(10);

  if(!TOF2.begin(Sensor2_newAddress)) {
    Serial.println(F("Failed to boot first VL53L0X 2"));
    //while(1);
  }
  TOF2.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE);
  
  // activating LOX1 and reseting LOX2
  pinMode(XSHUT_pin1, INPUT);//digitalWrite(XSHUT_pin1, HIGH);    
  delay(10);
  // initing LOX1
  if(!TOF1.begin()) {
    Serial.println(F("Failed to boot first VL53L0X 1"));
    //while(1);
  }
  TOF1.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE);
  
    
}

///////////////////////////////////////////////////////////////
void initToF()
{
  pinMode(XSHUT_pin1, OUTPUT);
  pinMode(XSHUT_pin2, OUTPUT);
  pinMode(XSHUT_pin3, OUTPUT);
  setID();
}

void testToF()
{
  while(1){
    TOF1.rangingTest(&measure1, false);
    TOF2.rangingTest(&measure2, false);
    TOF3.rangingTest(&measure3, false);
    if (measure1.RangeStatus == 4) printf("TOF1 out of range\t");
    if (measure2.RangeStatus == 4) printf("TOF2 out of range\t");
    if (measure3.RangeStatus == 4) printf("TOF3 out of range\n");
    printf("%d\t%d\t%d\n",measure1.RangeMilliMeter,measure2.RangeMilliMeter,measure3.RangeMilliMeter);

    delay(100);
  }
}
#else
#include <VL53L0X.h>
#include "Median.h"

//#define XSHUT_pin6 not required for address change
#define XSHUT_pin3 24
#define XSHUT_pin2 26
#define XSHUT_pin1 28

//ADDRESS_DEFAULT 0b0101001 or 41
//#define Sensor1_newAddress 41 not required address change
#define Sensor2_newAddress 42
#define Sensor3_newAddress 43

VL53L0X Sensor1;
VL53L0X Sensor2;
VL53L0X Sensor3;


MedianFilter s1(10, 0);
MedianFilter s2(10, 0);
MedianFilter s3(10, 0);
///////////////////////////////////////////////////////////////
void initToF()
{
 
  pinMode(XSHUT_pin1, OUTPUT);
  pinMode(XSHUT_pin2, OUTPUT);
  pinMode(XSHUT_pin3, OUTPUT);
  delay(10);
  Wire.begin();

  pinMode(XSHUT_pin3, INPUT);
  delay(10);
  Sensor3.setAddress(Sensor3_newAddress);
  pinMode(XSHUT_pin2, INPUT);
  delay(10);
  Sensor2.setAddress(Sensor2_newAddress);
  pinMode(XSHUT_pin1, INPUT);
  delay(10);
  
  Sensor1.init();
  Sensor2.init();
  Sensor3.init();

  // lower the return signal rate limit (default is 0.25 MCPS)
  Sensor1.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  Sensor1.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  Sensor1.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);

  // lower the return signal rate limit (default is 0.25 MCPS)
  Sensor2.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  Sensor2.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  Sensor2.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);

  // lower the return signal rate limit (default is 0.25 MCPS)
  Sensor3.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  Sensor3.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  Sensor3.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
  
  Sensor1.setTimeout(100);
  Sensor2.setTimeout(100);
  Sensor3.setTimeout(100);

  // Start continuous back-to-back mode (take readings as
  // fast as possible).  To use continuous timed mode
  // instead, provide a desired inter-measurement period in
  // ms (e.g. sensor.startContinuous(100)).
  Sensor1.startContinuous();
  Sensor2.startContinuous();
  Sensor3.startContinuous();
}

void getDist(int *d)
{
  s1.in(Sensor1.readRangeContinuousMillimeters());
  s2.in(Sensor2.readRangeContinuousMillimeters());
  s3.in(Sensor3.readRangeContinuousMillimeters());
  
  d[0] = s1.out();
  d[1] = s2.out();
  d[2] = s3.out();
}
/*int16_t measureToF()
{
  TOF.rangingTest(&measure, false);
  if(measure.RangeStatus != 4) {  
    return measure.RangeMilliMeter;
  } 
  else {
    return -1;
  } 
}*/
void testToF()
{
  Serial.print(Sensor1.readRangeContinuousMillimeters());
  Serial.print(',');
  Serial.print(Sensor2.readRangeContinuousMillimeters());
  Serial.print(',');
  Serial.print(Sensor3.readRangeContinuousMillimeters());
  Serial.println();
    delay(50);  
/*
  s1.in(Sensor1.readRangeSingleMillimeters());
  int res = s1.out();
    Serial.print(res);
  if (Sensor1.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  Serial.println();
  delay(10);
*/
}
#endif
