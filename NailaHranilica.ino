
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>

#elif defined(ESP8266)

#include <ESP8266WiFi.h>

#endif

#include <Firebase_ESP_Client.h>

//Provide the token generation process info.

#include "addons/TokenHelper.h"

//Provide the RTDB payload printing info and other helper functions.

#include "addons/RTDBHelper.h"

#include <Servo.h>

// Insert your network credentials

#define WIFI_SSID "Adis"

#define WIFI_PASSWORD "central123"

// Insert Firebase project API Key

#define API_KEY "AIzaSyAc8HK9j163Ym7QZRF5JQ4yb0jUf6InWpo"

// Insert RTDB URLefine the RTDB URL */

#define DATABASE_URL "https://hranilica-3b2b7-default-rtdb.europe-west1.firebasedatabase.app/"

//Define Firebase Data object

FirebaseData fbdo;




FirebaseAuth auth;

Servo myServo;

FirebaseConfig config;




unsigned long sendDataPrevMillis = 0;

int intValue;






bool signupOK = false;

void setup() {
  pinMode(D1, OUTPUT);

  myServo.attach(D3);
  Serial.begin(115200);


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);




  Serial.print("Connecting to Wi-Fi");




  while (WiFi.status() != WL_CONNECTED) {

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




  if (Firebase.signUp(&config, &auth, "", "")) {




    Serial.println("ok");




    signupOK = true;




  }




  else {




    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */




  config.token_status_callback = tokenStatusCallback;  //see addons/TokenHelper.h




  Firebase.begin(&config, &auth);




  Firebase.reconnectWiFi(true);

  randomSeed(analogRead(0));
}

void loop() {

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 500 || sendDataPrevMillis == 0)) {

    sendDataPrevMillis = millis();

    if (Firebase.RTDB.getInt(&fbdo, "/hranilica/int")) {

      if (fbdo.dataType() == "int") {

        intValue = fbdo.intData();

        Serial.println(intValue);

        Serial.println("TRUE");

        if (intValue == 1) {
          myServo.write(120);
          digitalWrite(D1, HIGH);
        } 
        else if(intValue==3)
        {
          for(int i=0; i<=4; i++)
          {
            if(i%2==0)
            {
              myServo.write(120);
              digitalWrite(D1,HIGH);
            }
            else
            {
              myServo.write(0);
              digitalWrite(D1,LOW);
            }
             Firebase.RTDB.setInt(&fbdo, "hranilica/int", 0);
          }
        }
        else if (intValue != 1 && intValue != 0) {
          intValue = intValue * 1000;
          digitalWrite(D1, HIGH);
          myServo.write(120);
          Serial.println(intValue);
          delay(intValue);
          myServo.write(0);
          digitalWrite(D1, LOW);
          Firebase.RTDB.setInt(&fbdo, "hranilica/int", 0);
        } else if (intValue == 0) {
          myServo.write(0);
          digitalWrite(D1, LOW);
        }
      }
    }

  }


  else {



    Serial.println(fbdo.errorReason());
  }
}
