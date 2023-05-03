#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <Wire.h>
#include "config.h"

#define LED_BUILTIN 2    // Used to provide visual indicator of mode.

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;    // Used to ensure that sending of data is stable and not too frequent.
int count = 0;
bool signupOK = false;

float voltageS = 0.0;    // Secondary side voltage read from firebase database.
float setpoint = 0.0;    // Target voltage setpoint to send to inverter controller.
uint8_t mode = 3;   // OFFLINE MODE
unsigned long modeChangeMillis = 0UL;    // used to ensure that mode change from STANDBY to ACTIVE is stable.

uint16_t volt_i2c = 0;
uint16_t setpoint_i2c = 0;

void handler();

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(9600);
  Wire.begin(9);
  Wire.setClock(100000);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("sign up ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Serial.println("setup complete");
}

void loop() {
  // on i2c request, send data to inverter controller
  Wire.onRequest(handler);

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 50 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.getFloat(&fbdo, "/receiver/voltage")) {
      Serial.println(fbdo.dataType());
      // if (fbdo.dataType() == "float") {
        Serial.print("assigning voltage: ");
        voltageS = fbdo.floatData();
        Serial.println(voltageS);
      // }
    } else {
      Serial.print("voltage error: ");
      Serial.println(fbdo.errorReason());
    }
    delay(5);
    if (Firebase.RTDB.getInt(&fbdo, "transmitter/mode")) {
      Serial.println(fbdo.dataType());
      // if (fbdo.dataType() == "int") {
        Serial.print("assigning mode: ");
        mode = uint8_t(fbdo.intData());
        Serial.println(mode);
      // }
    } else {
      Serial.print("mode error: ");
      Serial.println(fbdo.errorReason());
    }
  }
  delay(5);

  if (Firebase.RTDB.getFloat(&fbdo, "/setpoint")) {
    Serial.println(fbdo.dataType());
    // if (fbdo.dataType() == "float") {
      Serial.print("assigning setpoint: ");
      setpoint = fbdo.floatData();
      Serial.println(setpoint);
    // }
  } else {
    Serial.print("setpoint error: ");
    Serial.println(fbdo.errorReason());
  }
  delay(5);

  // if Vs > 3V then set mode to 2 (ACTIVE)
  if (mode == 1 && voltageS > 3.0) {
    mode = 2;
    // also update the firebase database that mode change has happened
    if (Firebase.RTDB.setInt(&fbdo, "transmitter/mode", mode)) {
      Serial.println("mode updated");
    } else {
      Serial.print("mode update error: ");
      Serial.println(fbdo.errorReason());
    }
    delay(5);
  }
  else if (mode == 2 && voltageS < 3.0) {
    mode = 1;
    // also update the firebase database that mode change has happened
    if (Firebase.RTDB.setInt(&fbdo, "transmitter/mode", mode)) {
      Serial.println("mode updated");
    } else {
      Serial.print("mode update error: ");
      Serial.println(fbdo.errorReason());
    }
    delay(5);
  }
}

void handler() {
  byte myArray[5];

  volt_i2c = uint16_t(voltageS * 100);
  setpoint_i2c = uint16_t(setpoint * 100);
 
  myArray[0] = (volt_i2c >> 8) & 0xFF;
  myArray[1] = volt_i2c & 0xFF;
  myArray[2] = (setpoint_i2c >> 8) & 0xFF;
  myArray[3] = setpoint_i2c & 0xFF;
  myArray[4] = mode;
  
  Serial.println("Sendingggg magic numbers");
  for (int i=0; i<5; i++) {
    Serial.println(myArray[i]);
  }
  Wire.write(myArray, 5);
}