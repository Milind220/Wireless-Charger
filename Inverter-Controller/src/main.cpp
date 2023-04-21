#include <eFlexPwm.h>    // Version: 0.2.5
#include <Arduino.h>
#include <PID_v1.h>
#include <Wire.h>

// TODO: Add i2c reading of the secondary voltage and setpoint votlage

// Pin definitions
#define DISABLE_PIN 24   // Used to disable/enable gate driver IC's outputs. low = enable, high = disable.
#define READ_MODE_PIN_0 36    // Used to read the 0 bit of the mode
#define READ_MODE_PIN_1 37    // Used to read the 1 bit of the mode

#define STANDBY_PHASE_SHIFT 20    // Small phase shift value for the standby mode.
#define OFFLINE_PHASE_SHIFT 50    // 50% or 90 degrees for offline standard mode.
#define DEADTIME_NS 100    // deadtime in nanoseconds.

// Phase shift range configuration // ! DOUBLE CHECK THESE VALUES !
#define MIN_OUTPUT_PS 25   // phase shift value that results in min inverter output
#define MAX_OUTPUT_PS 0    // phase shift value that results in max inverter output

enum modes{
  OFF = 0,
  STANDBY = 1,
  ACTIVE = 2,
  OFFLINE = 3,
};

uint16_t volt = 0; // TODO Later try changing to uint16_t
uint16_t sp_int = 0;
float voltageS = 0.0;
float sp = 0.0;
byte a, b, c, d, e;

// declare the funcs
uint32_t runOffMode();
uint32_t runStandbyMode();
uint32_t runOfflineMode();
// uint8_t checkMode();
// void handler(int numBytes);

uint32_t delayMicros = 0; // delay time in microseconds.
uint8_t mode = OFFLINE;
const uint32_t PwmFreq = 100000; // frequency of PWM = 100 kHz

// Add PID things
// Define Variables we'll be connecting to
double Setpoint, Input, Output;
//Specify the links and initial tuning parameters
double Kp=1, Ki=0, Kd=0;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

using namespace eFlex;
// eFlexPWM submodules (Hardware -> PWM2: SM[0], SM[2]
SubModule Sm20(4, 33);
SubModule Sm22(6, 9);
//  Tm2 simplifies access to the functions that concern all the sub-modules
Timer &Tm2 = Sm20.timer();

void setup() {
  Serial.begin(9600);
  Wire.begin(); 
  Wire.setSCL(18);
  Wire.setSDA(19);
  Wire.setClock(100000);

  pinMode(DISABLE_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);    // Temporarily used as a mode indicator. off = OFF, blink = STANDBY, on = ACTIVE, strobe = OFFLINE.
  // For the read mode pins, default is LOW. Should result in OFF mode in case of no input.
  pinMode(READ_MODE_PIN_0, INPUT_PULLDOWN);
  pinMode(READ_MODE_PIN_1, INPUT_PULLDOWN);

  digitalWrite(DISABLE_PIN, HIGH);    // high = disable, low = enable

  Config myConfig;
  myConfig.setReloadLogic(kPWM_ReloadPwmFullCycle);
  myConfig.setPairOperation(kPWM_ComplementaryPwmA);
  myConfig.setPwmFreqHz(PwmFreq); 
  // Initialize submodule
  Sm20.configure(myConfig);
 
  // Initialize submodule 2, make it use same counter clock as submodule 0. 
  myConfig.setClockSource(kPWM_Submodule0Clock);
  myConfig.setPrescale(kPWM_Prescale_Divide_1);
  myConfig.setInitializationControl(kPWM_Initialize_MasterSync);
  
  Sm22.configure(myConfig);
  
  // Sm23.configure (myConfig);
  Tm2.setupDeadtime(DEADTIME_NS, 1000000000); // deadtime 100 ns
  // synchronize registers and start all submodules
  Tm2.begin();

  Sm22.setupPwmPhaseShift(ChanA, STANDBY_PHASE_SHIFT, true); 

  // Wire.onReceive(handler);
}

unsigned long previousMillis = 0UL;
 
void loop() {
  unsigned long currentMillis = millis();
  Wire.requestFrom(9, 5);
  Serial.println("Requested!");
// while (Wire.available()) {
    a = Wire.read();
    b = Wire.read();
    c = Wire.read();
    d = Wire.read();
    e = Wire.read();
  // }

  // Serial.println(a);
  // Serial.println(b);
  // Serial.println(c);
  // Serial.println(d);
  // Serial.println(e);

  volt = a;
  volt = (volt << 8) | b;
  sp_int = c;
  sp_int = (sp_int << 8) | d;

  // print voltage and current
  voltageS = float(volt) / 100.0;
  sp = float(sp_int) / 100.0;


  //print values
  Serial.print("S Voltage: ");
  Serial.print(voltageS);
  Serial.print(" | setpoint: ");
  Serial.println(sp);

  // Check mode
  mode = e;
  Serial.println(mode);
  // mode = OFFLINE;
  
  // Run the mode
  switch (mode) {
    case OFF:
      runOffMode();
      break;

    case STANDBY:
      // run every 500 ms
      if (currentMillis - previousMillis >= 500) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;
        runStandbyMode();
      }
      // delayMicros = runStandbyMode();
      break;

    case ACTIVE:
    // run every 10 ms
      if (currentMillis - previousMillis >= 10) {
        // save the last time you blinked the LED
        Serial.printf("{%s, %d}: mode: ACTIVE\n", __FILE__, __LINE__);

        digitalWrite(DISABLE_PIN, LOW);    // MOSFET driver IC outputs enabled.
        digitalWrite(LED_BUILTIN, HIGH);   // LED on continuously.

        myPID.Compute();    // Compute PID loop and set output value.

        // scale PID output between min and max output phase shift values.
        Sm22.setupPwmPhaseShift(ChanA, map(Output, 0, 255, MIN_OUTPUT_PS, MAX_OUTPUT_PS), true);

        // delayMicros = (1000000U / PwmFreq) * 100; // delay 100 PWM periods
        previousMillis = currentMillis;
        
      }
      
      break;

    case OFFLINE:
    // run every 100 ms
      if (currentMillis - previousMillis >= 100) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;
        runOfflineMode();
      }
      
      break;

    default:
      runOffMode();
      break;
  }
  // delayMicroseconds(delayMicros);   // loop delay for stability.
  delay(50);
}

uint32_t runOffMode() {
  // Switch off inverter.
  Serial.printf("{%s, %d}: mode: OFF\n", __FILE__, __LINE__);
  digitalWrite(DISABLE_PIN, HIGH);  // disables MOSFET driver ICs output.
  digitalWrite(LED_BUILTIN, LOW);   // Visual indication of mode. LED if off.
  return 0;
}

uint32_t runStandbyMode() { 
  // Switch on inverter in standby mode. This increases phase shift, pulses the output every 500ms, reduces power consumption.
  Serial.printf("{%s, %d}: mode: STANDBY\n", __FILE__, __LINE__);
  digitalWrite(DISABLE_PIN, !digitalRead(DISABLE_PIN));    // Toggles inverter output.
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));   // Visual indication of mode. LED blinks.
  Sm22.setupPwmPhaseShift(ChanA, STANDBY_PHASE_SHIFT, true);
  return 500000;    // 500ms
}

uint32_t runOfflineMode() {
  Serial.printf("{%s, %d}: mode: OFFLINE\n", __FILE__, __LINE__);
  digitalWrite(DISABLE_PIN, LOW);
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));   // Visual indication of mode. LED blinks.
  Sm22.setupPwmPhaseShift(ChanA, OFFLINE_PHASE_SHIFT, true);
  return 50000;    // 100ms
}

// void handler(int numBytes) {
//   a = Wire.read();
//   b = Wire.read();
//   c = Wire.read();
//   d = Wire.read();

//   volt = a;
//   volt = (volt << 8) | b;
//   sp_int = c;
//   sp_int = (sp_int << 8) | d;

//   // print voltage and current
//   voltageS = float(volt) / 100.0;
//   sp = float(sp_int) / 100.0;

//   //print values
//   Serial.print("S Voltage: ");
//   Serial.print(voltageS);
//   Serial.print(" | setpoint: ");
//   Serial.println(sp);
// }

// (1000000U / PwmFreq) * 100;