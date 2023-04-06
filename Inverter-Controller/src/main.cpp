#include <eFlexPwm.h>
#include <Arduino.h>

#define DISABLE_PIN 24
#define READ_STATE_PIN 25


uint8_t phaseShiftPercent = 50; // the phase shift in %
const uint32_t PwmFreq = 100000; // frequency of PWM = 100kHz

enum stateOptions {OFF, ON};
uint8_t state = ON;
uint8_t prevState = ON;

using namespace eFlex;
// My eFlexPWM submodules (Hardware > PWM2: SM[0], SM[2]
SubModule Sm20(4, 33);
SubModule Sm22(6, 9);
//  Tm2 simplifies access to the functions that concern all the sub-modules
Timer &Tm2 = Sm20.timer();


void setup() {
  Serial.begin(9600);

  pinMode(DISABLE_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(READ_STATE_PIN, INPUT_PULLUP);    // Default HIGH, ie, default state is ON

  digitalWrite(DISABLE_PIN, LOW);    // Low = enabled

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
  Tm2.setupDeadtime(100, 1000000000); // deadtime 100ns
  // synchronize registers and start all submodules
  Tm2.begin();

  Sm22.setupPwmPhaseShift(ChanA, phaseShiftPercent, true);
}
 
void loop() {
  // Read the on/off state. If it has changed, update the inverter state.
  state = digitalRead(READ_STATE_PIN);
  if (state != prevState) {
    if (state == ON) {
      digitalWrite(LED_BUILTIN, HIGH);
      digitalWrite(DISABLE_PIN, LOW);
    } else if (state == OFF) {
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(DISABLE_PIN, HIGH);
    } else {
      Serial.println("Error: state not recognized");
      Serial.println("Inverter switched off for safety");
      digitalWrite(LED_BUILTIN, LOW);
      digitalWrite(DISABLE_PIN, HIGH);
    }
  }
  
  // // make the PWM phase shift oscillate between 0 and 50%
  // phaseShiftPercent = (phaseShiftPercent + 1) % 50;

  // // Set the PWM phase shift to the new value
  // Sm22.setupPwmPhaseShift(ChanA, phaseShiftPercent, true);

  // delayMicroseconds ( (1000000U / PwmFreq) * 100);     // Delay at least 100 PWM periods
  // delay(100);
  prevState = state;
}
