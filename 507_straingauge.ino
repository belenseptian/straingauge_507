#include "ADS1232.h"
#include "RunningAverage.h"

#define SCALE_PDWN   25
#define SCALE_DOUT   26
#define SCALE_SCLK   27
#define SCALE_SPEED  33

ADS1232 scale;
RunningAverage myRA(10);
int samples = 0;
int initial_adc = 0;

double adctoVoltage(int adc)
{
  double voltage = (float(adc)/16777216)*5;
  return voltage;
}

float voltagetoKpa(double voltage)
{
  float kpa = voltage*1000000*0.261955;
  return kpa;
}

void setup() {

  Serial.begin(115200);
  myRA.clear(); // explicitly start clean
  delay(5000);
  Serial.println("=== Enter Calibration Mode ===");

  scale.begin(SCALE_DOUT, SCALE_SCLK, SCALE_PDWN, SCALE_SPEED, FAST);

  Serial.println("Calibrating, it might take up to a few seconds...");      // print a raw reading from the ADC
  for (int i = 0; i < 10; i++)
  {
    long value_long;
    scale.read(value_long, true);
    initial_adc = value_long;
    delay(10);
  }
}

void loop() {
  long value_long;
  scale.read(value_long, true);
  myRA.addValue(value_long);
  int adc_filtered = myRA.getAverage();
  int setZero = abs(initial_adc-adc_filtered);
  if(setZero <= 200)
  {
    setZero = 0;  
  }
  else
  {
    setZero = abs(setZero-200);  
  }

  Serial.println("******************");
  Serial.print("ADC value : ");
  Serial.println(setZero);
  Serial.print("ADC voltage : ");
  Serial.println(adctoVoltage(setZero),7);
//  Serial.println("Pressure : ");
//  Serial.println(voltagetoKpa(adctoVoltage(setZero)),4);
  Serial.println("******************");
  
  if (samples == 30)
  {
    samples = 0;
    myRA.clear(); 
  }
  delay(500);
}
