#include <eFlexPwm.h>    // Version: 0.2.5
#include <Arduino.h>

// TODO: Add PID loop for active mode.

// Pin definitions
#define DISABLE_PIN 24   // Used to disable/enable gate driver IC's outputs. low = enable, high = disable.
#define READ_MODE_PIN_0 25    // Used to read the 0 bit of the mode
#define READ_MODE_PIN_1 26    // Used to read the 1 bit of the mode

#define STANDBY_PHASE_SHIFT 20    // Small phase shift value for the standby mode.
#define OFFLINE_PHASE_SHIFT 50    // 50% or 90 degrees for offline standard mode.

enum mode{
  OFF = 0,
  STANDBY = 1,
  ACTIVE = 2,
  OFFLINE = 3,
};

uint8_t phaseShiftPercent = 0; // the phase shift in % .  // ? Is this even required?
uint32_t delayMicros = 0; // delay time in microseconds.
uint8_t mode = mode::OFF; 
uint8_t prevMode = mode::OFF;
const uint32_t PwmFreq = 100000; // frequency of PWM = 100kHz

using namespace eFlex;
// eFlexPWM submodules (Hardware -> PWM2: SM[0], SM[2]
SubModule Sm20(4, 33);
SubModule Sm22(6, 9);
//  Tm2 simplifies access to the functions that concern all the sub-modules
Timer &Tm2 = Sm20.timer();

// Read the mode pins and return the mode.
uint8_t checkMode() {
  uint8_t mode = 0;
  mode |= digitalRead(READ_MODE_PIN_0);
  mode |= digitalRead(READ_MODE_PIN_1) << 1;
  return mode;
}

void setup() {
  Serial.begin(9600);

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
  Tm2.setupDeadtime(100, 1000000000); // deadtime 100ns
  // synchronize registers and start all submodules
  Tm2.begin();

  Sm22.setupPwmPhaseShift(ChanA, phaseShiftPercent, true); 
}
 
void loop() {
  // Check mode
  mode = checkMode();

  // Check if mode changed from previous mode
  if (mode != prevMode) {
    // Mode changed, update previous mode
    // Set the mode.
    switch (mode) {
      case mode::OFF:
        // Switch off inverter.
        Serial.printf("{%s, %d}: mode: OFF\n", __FILE__, __LINE__);
        digitalWrite(DISABLE_PIN, HIGH);  // disables MOSFET driver ICs output.
        digitalWrite(LED_BUILTIN, LOW);   // Visual indication of mode. LED if off.
        delayMicros = 0;
        break;

      case mode::STANDBY:
        Serial.printf("{%s, %d}: mode: STANDBY\n", __FILE__, __LINE__);
        // Switch on inverter in standby mode. This increases phase shift, pulses the output every 500ms, reduces power consumption.
        digitalWrite(DISABLE_PIN, !digitalRead(DISABLE_PIN));    // Toggles inverter output.
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));   // Visual indication of mode. LED blinks.
        Sm22.setupPwmPhaseShift(ChanA, STANDBY_PHASE_SHIFT, true);
        delayMicros = 500000;    // 500ms
        break;

      case mode::ACTIVE:
        Serial.printf("{%s, %d}: mode: ACTIVE\n", __FILE__, __LINE__);
        digitalWrite(DISABLE_PIN, LOW);
        digitalWrite(LED_BUILTIN, HIGH);
        // run PID loop with secondary voltage until achieved
        // TODO MAKE THIS work
        // delay 100 PWM periods
        delayMicros = (1000000U / PwmFreq) * 100;
        break;

      case mode::OFFLINE:
        Serial.printf("{%s, %d}: mode: OFFLINE\n", __FILE__, __LINE__);
        digitalWrite(DISABLE_PIN, LOW);
        digitalWrite(LED_BUILTIN, LOW);
        Sm22.setupPwmPhaseShift(ChanA, OFFLINE_PHASE_SHIFT, true);
        break;

      default:
        digitalWrite(DISABLE_PIN, HIGH);
        digitalWrite(LED_BUILTIN, LOW);
        delayMicros = 0;
        break;
    }
    prevMode = mode;
  }
  
  delayMicroseconds(delayMicros);   // loop delay for stability.
}
