#include <Arduino.h>
#include <Wire.h>

#define VOLTAGE_PIN A0   // Analog input pin for voltage
#define CURRENT_PIN A1   // Analog input pin for current
 
// Floats for ADC voltage & Input voltage
float adc_voltage = 0.0;
float in_voltage = 0.0;
 
// Constants for voltage sensor. Floats for resistor values in divider (in ohms)
const float R1 = 30000.0;
const float R2 = 7500.0; 
const float ref_voltage = 5.0;    // Volts

// Variables for ADC value & voltage 
int adc_value = 0;
float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;
uint16_t i2c_curr = 0;

void requestEvent();

void setup()
{
   // Setup Serial Monitor
  Serial.begin(9600);
  Wire.begin(8);
  Serial.println("DC Voltage Test");
}
 
void loop(){
  // Read the Analog Input
  adc_value = analogRead(VOLTAGE_PIN);
   
  adc_voltage = (adc_value * ref_voltage) / 1024.0;    // Determine voltage at ADC input
  in_voltage = adc_voltage / (R2/(R1+R2));    // Calculate voltage at divider input
   
  float AcsValue=0.0,Samples=0.0,AvgAcs=0.0,AcsValueF=0.0;

  for (int x = 0; x < 50; x++){ //Get 50 samples
    AcsValue = analogRead(A0);     //Read current sensor values   
    Samples = Samples + AcsValue;  //Add samples together
    delay (2); // let ADC settle before next sample 3ms
  }
  AvgAcs=Samples/50.0;//Taking Average of Samples

  //((AvgAcs * (5.0 / 1024.0)) is converitng the read voltage in 0-5 volts
  //2.5 is offset(I assumed that arduino is working on 5v so the viout at no current comes
  //out to be 2.5 which is out offset. If your arduino is working on different voltage than 
  //you must change the offset according to the input voltage)
  //0.066v(66mV) is rise in output voltage when 1A current flows at input
  AcsValueF = ((AvgAcs * (5.0 / 1024.0))*0.22);
  i2c_curr = uint16_t(AcsValueF * 100);
  
  // Print results to Serial Monitor to 2 decimal places
  Serial.print("Voltage = ");
  Serial.print(in_voltage);
  Serial.print(" | ");
  Serial.print("Current = ");
  Serial.println(i2c_curr);//Print the read current on Serial monitor
  Wire.onRequest(requestEvent);          
}

void requestEvent()  
{
  uint16_t volt = uint16_t(in_voltage*100);

  byte myArray[4];
 
  myArray[0] = (volt >> 8) & 0xFF;
  myArray[1] = volt & 0xFF;
  myArray[2] = (i2c_curr >> 8) & 0xFF;
  myArray[3] = i2c_curr & 0xFF;

  Wire.write(myArray, 4);
}