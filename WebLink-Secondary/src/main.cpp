#include <Arduino.h>
#include <Wire.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"    // Provide the token generation process info.
#include "addons/RTDBHelper.h"     // Provide the RTDB payload printing info and other helper functions.
#include "config.h"

#define UPDATE_DELAY 50UL

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

uint16_t volt = 0; // TODO Later try changing to uint16_t
uint16_t curr = 0;
float voltageS = 0.0;
float currentS = 0.0;
byte a, b, c, d;

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
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
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("ok");
    signupOK = true;
  }
  else
  {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop()
{
  Wire.requestFrom(8, 4);

  a = Wire.read();
  b = Wire.read();
  c = Wire.read();
  d = Wire.read();

  volt = a;
  volt = (volt << 8) | b;
  curr = c;
  curr = (curr << 8) | d;

  // print voltage and current
  voltageS = float(volt) / 100.0;
  currentS = float(curr) / 100.0;

  // Serial.print("ESP Voltage: ");
  // Serial.print(voltageS);
  // Serial.print(" ESP Current: ");
  // Serial.print(currentS);

  // Serial.print("\n");

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > UPDATE_DELAY || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "receiver/voltage", voltageS))
    {
      // Serial.printf("voltage updated: %f\n", voltageS);
    }
    else
    {
      Serial.println("FAILED: " + fbdo.errorReason());
      // Serial.printf("PATH: %s\n",fbdo.dataPath());
      // Serial.printf("TYPE: %s\n",fbdo.dataType());
    }
    delay(20);
    if (Firebase.RTDB.setFloat(&fbdo, "receiver/current", currentS))
    {
      // Serial.printf("current updated: %f\n", currentS);
    }
    else
    {
      Serial.println("FAILED: " + fbdo.errorReason());
      // Serial.printf("PATH: %s\n",fbdo.dataPath());
      // Serial.printf("TYPE: %s\n",fbdo.dataType());
    }
  }
  delay(20);
}

